#include "config_t.h"

#include <ini.h>
#include "stdfunc.h"

config_t config_t$create() {
    config_t l_returnValue = DEFAULT_CONFIG;

    {
        l_returnValue.backgrounds = createArray( background_t* );
    }

    return ( l_returnValue );
}

bool config_t$destroy( config_t* restrict _config ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    {
        FOR_ARRAY( background_t* const*, _config->backgrounds ) {
            l_returnValue = background_t$destroy( *_element );

            if (UNLIKELY(!l_returnValue ) ) {
                goto EXIT;
            }
        }

        FREE_ARRAY( _config->backgrounds );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static int handler(void* _config, const char* _sectionName, const char* _key, const char* _value, int _lineNumber ) {
    config_t* l_config = (config_t*)_config;

    // Error
    // return 0;

    // Success
    return 1;
}

bool config_t$load$fromString( config_t* restrict _config, const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_string ) || UNLIKELY( !__builtin_strlen( _string ) ) ) {
        goto EXIT;
    }

    {
        const int l_status = ini_parse_string( _string, handler, _config );

        l_returnValue = !( l_status < 0 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( (logLevel_t)error, "Config loading from string\n" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromAsset( config_t* restrict _config, asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
#if defined( LOG_CONFIG )

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Config loading asset: Size = %zu\n",
                                      _asset->size );

#endif

        char* l_dataWithNull =
            ( char* )malloc( ( _asset->size + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_dataWithNull, _asset->data, _asset->size );

        l_dataWithNull[ _asset->size ] = '\0';

        l_returnValue = config_t$load$one$fromString( _config, l_dataWithNull );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        free( l_dataWithNull );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromPath( config_t* restrict _config,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension ) {

    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_fileName ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_fileExtension ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )debug,
                                      "Config path: '%s.%s'\n", _fileName,
                                      _fileExtension );

        // Parse config file
        {
            asset_t l_configAsset = asset_t$create();

            {
                char* l_filePath = duplicateString( "." );

                l_returnValue = !!( concatBeforeAndAfterString(
                    &l_filePath, _fileName, _fileExtension ) );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_FILE_PATH_CONCAT;
                }

                l_returnValue = asset_t$load( &l_configAsset, l_filePath );

            EXIT_FILE_PATH_CONCAT:
                free( l_filePath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }

            l_returnValue =
                config_t$load$fromAsset( _config, &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = asset_t$unload( &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = asset_t$destroy( &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$unload( config_t* restrict _config ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    {
        FOR_ARRAY( background_t* const*, _config->backgrounds ) {
            l_returnValue = background_t$unload( *_element );

            if (UNLIKELY(!l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
