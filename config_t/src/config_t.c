#include "config_t.h"

#include <ini.h>

#include "log.h"
#include "stdfunc.h"

config_t config_t$create() {
    config_t l_returnValue = DEFAULT_CONFIG;

    {
        l_returnValue.backgrounds = createArray( background_t* );
        l_returnValue.HUDs = createArray( HUD_t* );
    }

    return ( l_returnValue );
}

bool config_t$destroy( config_t* restrict _config ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
#define MACRO( _field )                                              \
    do {                                                             \
        FOR_ARRAY( _field##_t* const*, _config->_field##s ) {        \
            l_returnValue = _field##_t$destroy( *_element );         \
            if ( UNLIKELY( !l_returnValue ) ) {                      \
                log$transaction$query( ( logLevel_t )error,          \
                                       "Destroying " #_field "\n" ); \
                goto EXIT;                                           \
            }                                                        \
        }                                                            \
        FREE_ARRAY( _config->_field##s );                            \
    } while ( 0 )

        MACRO( background );
        MACRO( HUD );

#undef MACRO

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static int lineHandler( void* _config,
                        const char* _sectionName,
                        const char* _key,
                        const char* _value ) {
    // Error - 0
    // Success - >0
    int l_returnValue = 0;

    if ( UNLIKELY( !_config ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !__builtin_strlen( _sectionName ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( ( _key ) && !__builtin_strlen( _key ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( ( _value ) && !__builtin_strlen( _value ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        config_t* l_config = ( config_t* )_config;

        static char l_previouSsectionName[ PATH_MAX ] = { '\0' };
        static char l_name[ PATH_MAX ] = { '\0' };
        static char l_folder[ PATH_MAX ] = { '\0' };
        static char l_extension[ PATH_MAX ] = { '\0' };

#define MATCH_STRING( _string1, _string2 ) \
    ( ( _string1 ) && ( _string2 ) &&      \
      ( __builtin_strcmp( _string1, _string2 ) == 0 ) )

        if ( _key && _value ) {
            const size_t l_valueLength = __builtin_strlen( _value );

            if ( UNLIKELY( l_valueLength >= PATH_MAX ) ) {
                log$transaction$query( ( logLevel_t )error, "Value length\n" );

                trap();

                goto EXIT;
            }

#if defined( LOG_CONFIG )

            log$transaction$query$format( ( logLevel_t )debug, "'%s' = '%s'\n",
                                          _key, _value );

#endif

            if ( MATCH_STRING( _key, "name" ) ) {
                __builtin_memcpy( l_name, _value, ( l_valueLength + 1 ) );

            } else if ( MATCH_STRING( _key, "folder" ) ) {
                __builtin_memcpy( l_folder, _value, ( l_valueLength + 1 ) );

            } else if ( MATCH_STRING( _key, "extension" ) ) {
                __builtin_memcpy( l_extension, _value, ( l_valueLength + 1 ) );
            }

        } else if ( !_key && !_value ) {
            const size_t l_sectionNameLength = __builtin_strlen( _sectionName );

            if ( UNLIKELY( l_sectionNameLength >= PATH_MAX ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Section name length\n" );

                trap();

                goto EXIT;
            }

            if ( __builtin_strlen( l_name ) && __builtin_strlen( l_folder ) &&
                 __builtin_strlen( l_extension ) ) {
                log$transaction$query$format( ( logLevel_t )info,
                                              "[ '%s' : '%s' ]: '%s'\n", l_name,
                                              l_folder, l_extension );

                // TODO: Name macro
#define MACRO( _x )                                                    \
    do {                                                               \
        if ( MATCH_STRING( l_previouSsectionName, #_x ) ) {            \
            _x##_t l_##_x = _x##_t$create();                           \
            l_##_x.name = duplicateString( l_name );                   \
            l_##_x.folder = duplicateString( l_folder );               \
            l_##_x.extension = duplicateString( l_extension );         \
            insertIntoArray( &( l_config->_x##s ), clone( &l_##_x ) ); \
        }                                                              \
    } while ( 0 )

                MACRO( background );
                MACRO( HUD );

#undef MACRO
            }

            __builtin_memset( l_name, 0, sizeof( l_name ) );
            __builtin_memset( l_folder, 0, sizeof( l_folder ) );
            __builtin_memset( l_extension, 0, sizeof( l_extension ) );

#if defined( LOG_CONFIG )

            log$transaction$query$format( ( logLevel_t )debug, "[ %s ]\n",
                                          _sectionName );

#endif

            __builtin_memcpy( l_previouSsectionName, _sectionName,
                              ( l_sectionNameLength + 1 ) );
        }

#undef MATCH_STRING

        l_returnValue = 1;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE void finishLineHandling( char* _fieldName, void* _config ) {
    config_t* l_config = ( config_t* )_config;

    trim( &_fieldName, 0, ( __builtin_strlen( _fieldName ) - 1 ) );

    const int l_result = lineHandler( l_config, _fieldName, NULL, NULL );

    if ( UNLIKELY( !l_result ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "Finishing config loading\n" );
    }
}

bool config_t$load$fromString( config_t* restrict _config,
                               const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_string ) || UNLIKELY( !__builtin_strlen( _string ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        const int l_errorLineNumber =
            ini_parse_string( _string, lineHandler, _config );

        l_returnValue = ( l_errorLineNumber == 0 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Loading config from string: line %d\n",
                l_errorLineNumber );

            goto EXIT;
        }

        iterateTopMostFields( config_t, finishLineHandling, _config );

// TODO: Improve
#define MACRO( _field )                                         \
    do {                                                        \
        l_returnValue = !!( arrayLength(_config->_field) );                  \
        if ( UNLIKELY( !l_returnValue ) ) {                     \
            log$transaction$query( ( logLevel_t )error,         \
                                   "Loaded no " #_field "\n" ); \
            goto EXIT;                                          \
        }                                                       \
    } while ( 0 )

        MACRO( backgrounds );
        MACRO( HUDs );

#undef MACRO

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromAsset( config_t* restrict _config,
                              asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

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

        l_returnValue = config_t$load$fromString( _config, l_dataWithNull );

        free( l_dataWithNull );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Loading config from string\n" );

            goto EXIT;
        }

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fileName ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fileExtension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

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

                concatBeforeAndAfterString( &l_filePath, _fileName,
                                            _fileExtension );

                l_returnValue =
                    asset_t$load$fromPath( &l_configAsset, l_filePath );

                free( l_filePath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading asset from path\n" );

                    goto EXIT_CONFIG_LOAD;
                }
            }

            l_returnValue = config_t$load$fromAsset( _config, &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading config from asset\n" );

                goto EXIT_CONFIG_LOAD2;
            }

        EXIT_CONFIG_LOAD2:
            l_returnValue = asset_t$unload( &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Unloading asset\n" );

                goto EXIT_CONFIG_LOAD;
            }

        EXIT_CONFIG_LOAD:
            if ( UNLIKELY( !asset_t$destroy( &l_configAsset ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying asset\n" );

                goto EXIT;
            }

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
#define MACRO( _field )                                             \
    do {                                                            \
        FOR_ARRAY( _field##_t* const*, _config->_field##s ) {       \
            l_returnValue = _field##_t$unload( *_element );         \
            if ( UNLIKELY( !l_returnValue ) ) {                     \
                log$transaction$query( ( logLevel_t )error,         \
                                       "Unloading " #_field "\n" ); \
                goto EXIT;                                          \
            }                                                       \
        }                                                           \
    } while ( 0 )

        MACRO( background );
        MACRO( HUD );

#undef MACRO

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
