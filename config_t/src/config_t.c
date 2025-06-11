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
        goto EXIT;
    }

    {
#define MACRO( _field )                                       \
    do {                                                      \
        FOR_ARRAY( _field##_t* const*, _config->_field##s ) { \
            l_returnValue = _field##_t$destroy( *_element );  \
            if ( UNLIKELY( !l_returnValue ) ) {               \
                goto EXIT;                                    \
            }                                                 \
        }                                                     \
        FREE_ARRAY( _config->_field##s );                     \
    } while ( 0 )

        MACRO( background );
        MACRO( HUD );

#undef MACRO

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Improve
static int lineHandler( void* _config,
                        const char* _sectionName,
                        const char* _key,
                        const char* _value ) {
    // Error - 0
    // Success - >0
    int l_returnValue = 0;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !__builtin_strlen( _sectionName ) ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( ( _key ) && !__builtin_strlen( _key ) ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( ( _value ) && !__builtin_strlen( _value ) ) ) {
        goto EXIT;
    }

    {
        config_t* l_config = ( config_t* )_config;

        static char l_name[ PATH_MAX ] = { '\0' };
        static char l_folder[ PATH_MAX ] = { '\0' };
        static char l_extension[ PATH_MAX ] = { '\0' };

        if ( UNLIKELY( l_name[ 0 ] == '\0' ) ) {
            __builtin_memset( l_name, 0, sizeof( l_name ) );
        }

        if ( UNLIKELY( l_folder[ 0 ] == '\0' ) ) {
            __builtin_memset( l_folder, 0, sizeof( l_folder ) );
        }

        if ( UNLIKELY( l_extension[ 0 ] == '\0' ) ) {
            __builtin_memset( l_extension, 0, sizeof( l_extension ) );
        }

        // TODO: Improve
#define MATCH_STRING( _string1, _string2 ) \
    ( ( _string1 ) && ( _string2 ) &&      \
      ( __builtin_strcmp( _string1, _string2 ) == 0 ) )

        if ( MATCH_STRING( _sectionName, "background" ) ) {
            if ( !_key && !_value ) {
                log$transaction$query$format( ( logLevel_t )debug, "[ %s ]\n",
                                              _sectionName );

                if ( l_name && l_folder && l_extension ) {
                    background_t l_background = background_t$create();

                    l_background.name = duplicateString( l_name );
                    l_background.folder = duplicateString( l_folder );
                    l_background.extension = duplicateString( l_extension );

                    log$transaction$query$format(
                        ( logLevel_t )info, "[ '%s' : '%s' ]: '%s'\n",
                        l_background.name, l_background.folder,
                        l_background.extension );

                    insertIntoArray( &( l_config->backgrounds ),
                                     clone( &l_background ) );
                }

                __builtin_memset( l_name, 0, sizeof( l_name ) );
                __builtin_memset( l_folder, 0, sizeof( l_folder ) );
                __builtin_memset( l_extension, 0, sizeof( l_extension ) );

            } else if ( _key && _value ) {
                log$transaction$query$format( ( logLevel_t )debug,
                                              "'%s' = '%s'\n", _key, _value );

                const size_t l_valueLength = __builtin_strlen( _value );

                if ( UNLIKELY( l_valueLength >= PATH_MAX ) ) {
                    // TODO: Improve
                    __builtin_trap();
                }

                if ( MATCH_STRING( _key, "name" ) ) {
                    __builtin_memcpy( l_name, _value, ( l_valueLength + 1 ) );

                } else if ( MATCH_STRING( _key, "folder" ) ) {
                    __builtin_memcpy( l_folder, _value, ( l_valueLength + 1 ) );

                } else if ( MATCH_STRING( _key, "extension" ) ) {
                    __builtin_memcpy( l_extension, _value, ( l_valueLength + 1 ) );
                }
            }
        }

#undef MATCH_STRING

        l_returnValue = 1;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Improve
bool config_t$load$fromString( config_t* restrict _config,
                               const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_string ) || UNLIKELY( !__builtin_strlen( _string ) ) ) {
        goto EXIT;
    }

    {
        const int l_errorLineNumber =
            ini_parse_string( _string, lineHandler, _config );

        l_returnValue = ( !( l_errorLineNumber < 0 ) || ( l_errorLineNumber ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Config loading from string: line %d\n",
                l_errorLineNumber );

            goto EXIT;
        }

        // TODO: Improve
        {
            // Background
            {
                const int l_result =
                    lineHandler( _config, "background", NULL, NULL );

                l_returnValue = !!( l_result );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Config loading\n" );

                    goto EXIT;
                }
            }

            // HUD
            {
                const int l_result = lineHandler( _config, "HUD", NULL, NULL );

                l_returnValue = !!( l_result );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Config loading\n" );

                    goto EXIT;
                }
            }
        }

        if ( UNLIKELY( !( _config->backgrounds ) ) ) {
            l_returnValue = false;

            log$transaction$query( ( logLevel_t )error,
                                   "Config loaded no backgrounds\n" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromAsset( config_t* restrict _config,
                              asset_t* restrict _asset ) {
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

        l_returnValue = config_t$load$fromString( _config, l_dataWithNull );

        free( l_dataWithNull );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Add HUD
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
                    goto EXIT_CONFIG_LOAD;
                }
            }

            l_returnValue = config_t$load$fromAsset( _config, &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_CONFIG_LOAD;
            }

            l_returnValue = asset_t$unload( &l_configAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_CONFIG_LOAD;
            }

        EXIT_CONFIG_LOAD:
            if ( UNLIKELY( !asset_t$destroy( &l_configAsset ) ) ) {
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
        goto EXIT;
    }

    {
#if 0
        // TODO
        FOR_ARRAY( background_t* const*, _config->backgrounds ) {
            l_returnValue = background_t$unload( *_element );

            if (UNLIKELY(!l_returnValue ) ) {
                goto EXIT;
            }
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
