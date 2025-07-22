#include <pthread.h>
#undef clone

#include <errno.h>
#include <fcntl.h>
#include <snappy-c.h>
#include <stdlib.h>
#include <unistd.h>

#include "asset_t.h"
#include "log.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

#define MAX_REQUESTS 1000

struct saveRequest {
    asset_t* asset;
    char* path;
    bool needTruncate;
};

static char* g_assetsDirectory = NULL;
static pthread_t g_assetSaveQueueResolveThread;
static bool g_shouldAssetSaveQueueResolveThreadWork = false;
static struct saveRequest g_saveQueue[ MAX_REQUESTS ];
static size_t g_saveQueueLength = 0;
static pthread_mutex_t g_saveQueueMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_saveQueueCondition = PTHREAD_COND_INITIALIZER;

static void* asset_t$saveQueue$resolve( void* _data ) {
    UNUSED( _data );

    for ( ;; ) {
        struct saveRequest l_saveRequest = { 0 };

        {
            pthread_mutex_lock( &g_saveQueueMutex );

            while ( !g_saveQueueLength &&
                    LIKELY( g_shouldAssetSaveQueueResolveThreadWork ) ) {
                pthread_cond_wait( &g_saveQueueCondition, &g_saveQueueMutex );
            }

            if ( UNLIKELY( !g_shouldAssetSaveQueueResolveThreadWork ) &&
                 !g_saveQueueLength ) {
                pthread_mutex_unlock( &g_saveQueueMutex );

                break;
            }

            // Copy request locally to process without holding lock
            l_saveRequest = g_saveQueue[ 0 ];

            // Move the rest of queue
            __builtin_memmove(
                &( g_saveQueue[ 0 ] ), &( g_saveQueue[ 1 ] ),
                ( ( g_saveQueueLength - 1 ) * sizeof( struct saveRequest ) ) );

            g_saveQueueLength--;

            pthread_mutex_unlock( &g_saveQueueMutex );
        }

        {
            bool l_result = asset_t$save$sync$toPath(
                l_saveRequest.asset, l_saveRequest.path,
                l_saveRequest.needTruncate );

            if ( UNLIKELY( !l_result ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error,
                    "Saving asset of size %zu to path: '%s'",
                    l_saveRequest.asset->size, l_saveRequest.path );
            }

            l_result = asset_t$unload( l_saveRequest.asset );

            if ( UNLIKELY( !l_result ) ) {
                log$transaction$query( ( logLevel_t )error, "Unloading asset" );
            }

            l_result = asset_t$destroy( l_saveRequest.asset );

            if ( UNLIKELY( !l_result ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying asset" );
            }

            free( l_saveRequest.path );
        }
    }

    return ( NULL );
}

bool asset_t$loader$init( const char* restrict _assetsDirectory ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Alraedy initialized" );

        goto EXIT;
    }

    {
        char* l_assetsDirectory = duplicateString( _assetsDirectory );

        {
            char* l_directoryPath = getApplicationDirectoryAbsolutePath();

            concatBeforeAndAfterString( &l_assetsDirectory, l_directoryPath,
                                        "/" );

            free( l_directoryPath );
        }

        g_assetsDirectory = l_assetsDirectory;

        g_shouldAssetSaveQueueResolveThreadWork = true;

        if ( UNLIKELY( pthread_create( &g_assetSaveQueueResolveThread, NULL,
                                       asset_t$saveQueue$resolve, NULL ) ) ) {
            log$transaction$query$format(
                ( logLevel_t )error,
                "%d: Insufficient resources to create another thread, or a "
                "system-imposed limit on the number of threads was "
                "encountered",
                EAGAIN );

            asset_t$loader$quit();

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$loader$quit( void ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Not initialized" );

        goto EXIT;
    }

    {
        if ( LIKELY( g_shouldAssetSaveQueueResolveThreadWork ) ) {
            pthread_mutex_lock( &g_saveQueueMutex );

            g_shouldAssetSaveQueueResolveThreadWork = false;

            pthread_cond_signal( &g_saveQueueCondition );

            pthread_mutex_unlock( &g_saveQueueMutex );

            pthread_join( g_assetSaveQueueResolveThread, NULL );
        }

        free( g_assetsDirectory );

        g_assetsDirectory = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

asset_t asset_t$create( void ) {
    asset_t l_returnValue = DEFAULT_ASSET;

    return ( l_returnValue );
}

bool asset_t$destroy( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        _asset->data = NULL;
        _asset->size = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$fromPath( asset_t* restrict _asset,
                            const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        int l_fileDescriptor = -1;

        {
            char* l_path = duplicateString( _path );

            concatBeforeAndAfterString( &l_path, g_assetsDirectory, NULL );

            l_fileDescriptor = open( l_path, O_RDONLY );

            free( l_path );
        }

        l_returnValue = ( l_fileDescriptor != -1 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Opening asset: '%s'", _path );

            goto FILE_EXIT;
        }

        {
            // Get file size
            off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );

            l_returnValue = !!( l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Getting asset size" );

                goto FILE_EXIT;
            }

            lseek( l_fileDescriptor, 0, SEEK_SET );

            _asset->data = ( uint8_t* )malloc( l_fileSize );
            _asset->size = l_fileSize;

            const ssize_t l_readenCount =
                read( l_fileDescriptor, _asset->data, l_fileSize );

            l_returnValue = ( l_readenCount == l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Reading asset" );

                asset_t$unload( _asset );

                goto FILE_EXIT;
            }
        }

    FILE_EXIT:
        if ( UNLIKELY( l_fileDescriptor == -1 ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        close( l_fileDescriptor );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$fromGlob( asset_t* restrict _asset,
                            const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        {
            char** l_paths = getPathsByGlob( _glob, g_assetsDirectory, false );

            l_returnValue = asset_t$load$fromPath( _asset, l_paths[ 0 ] );

            FREE_ARRAY_ELEMENTS( l_paths );
            FREE_ARRAY( l_paths );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Loading asset from glob: '%s'",
                                              _glob );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$array$load$fromGlob( asset_t*** restrict _assetArray,
                                  const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetArray ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        char** l_paths = getPathsByGlob( _glob, g_assetsDirectory, false );

        FOR_ARRAY( char* const*, l_paths ) {
            asset_t l_asset = asset_t$create();

            l_returnValue = asset_t$load$fromPath( &l_asset, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Loading asset from path: '%s'",
                                              *_element );

                asset_t$destroy( &l_asset );

                goto EXIT_LOADING;
            }

            insertIntoArray( _assetArray, clone( &l_asset ) );
        }

        l_returnValue = true;

    EXIT_LOADING:
        FREE_ARRAY_ELEMENTS( l_paths );
        FREE_ARRAY( l_paths );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$compressed( asset_t* restrict _asset,
                              const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$load$fromPath( _asset, _path );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Loading asset from path: '%s'", _path );

            goto EXIT;
        }

        l_returnValue = asset_t$compress( _asset );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$fromGlob$compressed( asset_t* restrict _asset,
                                       const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$load$fromGlob( _asset, _glob );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Loading asset from glob: '%s'", _glob );

            goto EXIT;
        }

        l_returnValue = asset_t$compress( _asset );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$array$load$fromGlob$compressed( asset_t*** restrict _assetArray,
                                             const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetArray ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$array$load$fromGlob( _assetArray, _glob );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Loading asset array from glob: '%s'",
                                          _glob );

            goto EXIT;
        }

        FOR_ARRAY( asset_t* const*, *_assetArray ) {
            l_returnValue = asset_t$compress( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Compressing asset" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$unload( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        if ( LIKELY( _asset->data ) ) {
            free( _asset->data );
            _asset->data = NULL;
        }

        _asset->size = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$compress( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        size_t l_compressedLength =
            snappy_max_compressed_length( _asset->size );

        uint8_t* l_data =
            ( uint8_t* )malloc( l_compressedLength * sizeof( uint8_t ) );

        l_returnValue = ( snappy_compress( ( char* )( _asset->data ),
                                           _asset->size, ( char* )( l_data ),
                                           &l_compressedLength ) == SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            free( l_data );

            goto EXIT;
        }

        l_data = ( uint8_t* )realloc(
            l_data, ( l_compressedLength * sizeof( uint8_t ) ) );

        free( _asset->data );

        _asset->data = l_data;
        _asset->size = ( l_compressedLength * sizeof( uint8_t ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$uncompress( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            ( snappy_validate_compressed_buffer( ( char* )( _asset->data ),
                                                 _asset->size ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Validating compressed asset" );

            goto EXIT;
        }

        size_t l_uncompressedLength = 0;

        l_returnValue = ( snappy_uncompressed_length(
                              ( char* )( _asset->data ), _asset->size,
                              &l_uncompressedLength ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Getting uncompressed asset length" );

            goto EXIT;
        }

        uint8_t* l_data =
            ( uint8_t* )malloc( l_uncompressedLength * sizeof( uint8_t ) );

        l_returnValue =
            ( snappy_uncompress( ( char* )( _asset->data ), _asset->size,
                                 ( char* )( l_data ),
                                 &l_uncompressedLength ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Uncompressing asset" );

            free( l_data );

            goto EXIT;
        }

        l_data = ( uint8_t* )realloc(
            l_data, ( l_uncompressedLength * sizeof( uint8_t ) ) );

        free( _asset->data );

        _asset->data = l_data;
        _asset->size = ( l_uncompressedLength * sizeof( uint8_t ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$save$sync$toPath( asset_t* restrict _asset,
                               const char* restrict _path,
                               const bool _needTruncate ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        int l_fileDescriptor = -1;

        {
            {
                char* l_path = duplicateString( _path );

                concatBeforeAndAfterString( &l_path, g_assetsDirectory, NULL );

                // Open file descriptor
                {
                    size_t l_openFlags = ( O_WRONLY | O_CREAT );

                    if ( _needTruncate ) {
                        l_openFlags |= O_TRUNC;
                    }

                    // 0 - No special bits
                    // 6 - Read & Write for owner
                    // 4 - Read for group members
                    // 4 - Read for others
                    l_fileDescriptor = open( l_path, l_openFlags, 0644 );
                }

                free( l_path );
            }

            l_returnValue = ( l_fileDescriptor != -1 );

            if ( !l_returnValue ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Opening file for saving: '%s'",
                                              _path );

                goto EXIT;
            }

            const ssize_t l_writtenCount =
                write( l_fileDescriptor, _asset->data, _asset->size );

            l_returnValue = ( l_writtenCount == ( ssize_t )( _asset->size ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Writing to file" );

                goto EXIT_SAVE;
            }
        }

        l_returnValue = true;

    EXIT_SAVE:
        close( l_fileDescriptor );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$save$async$toPath( asset_t* restrict _asset,
                                const char* restrict _path,
                                const bool _needTruncate ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        {
            pthread_mutex_lock( &g_saveQueueMutex );

            if ( g_saveQueueLength >= MAX_REQUESTS ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Save queue length is already full" );

                pthread_mutex_unlock( &g_saveQueueMutex );

                goto EXIT;
            }

            asset_t l_asset = asset_t$create();

            l_asset.data = ( uint8_t* )malloc( _asset->size );
            __builtin_memcpy( l_asset.data, _asset->data, _asset->size );
            l_asset.size = _asset->size;

            struct saveRequest l_saveRequest = {
                .asset = clone( &l_asset ),
                .path = duplicateString( _path ),
                .needTruncate = _needTruncate,
            };

            g_saveQueue[ g_saveQueueLength ] = l_saveRequest;

            g_saveQueueLength++;

            pthread_cond_signal( &g_saveQueueCondition );
            pthread_mutex_unlock( &g_saveQueueMutex );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

const char* asset_t$loader$assetsDirectory$get( void ) {
    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "Assets loader not initialized" );

        trap();

        return ( "/tmp/" );
    }

    return ( g_assetsDirectory );
}

#if defined( HOT_RELOAD )

struct state {
    char* g_assetsDirectory;
};

EXPORT bool hotReload$unload( void** restrict _state,
                              size_t* restrict _stateSize,
                              applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        *_stateSize = sizeof( struct state );
        *_state = malloc( *_stateSize );

        struct state l_state = {
            .g_assetsDirectory = g_assetsDirectory,
        };

        __builtin_memcpy( *_state, &l_state, *_stateSize );

        l_returnValue = true;
    }

    return ( l_returnValue );
}

EXPORT bool hotReload$load( void* restrict _state,
                            size_t _stateSize,
                            applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        const size_t l_stateSize = sizeof( struct state );

        if ( UNLIKELY( _stateSize != l_stateSize ) ) {
            goto EXIT;
        }

        struct state* l_state = ( struct state* )_state;

        g_assetsDirectory = l_state->g_assetsDirectory;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif
