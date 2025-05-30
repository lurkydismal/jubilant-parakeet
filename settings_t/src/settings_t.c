#include "settings_t.h"

#include <stdlib.h>

#include "log.h"
#include "settingsOption_t.h"
#include "stdfunc.h"

settings_t settings_t$create( void ) {
    settings_t l_returnValue = DEFAULT_SETTINGS;

    {
        l_returnValue.window = window_t$create();
        l_returnValue.version = duplicateString( DEFAULT_SETTINGS_VERSION );
        l_returnValue.identifier = duplicateString( l_returnValue.window.name );
    }

    return ( l_returnValue );
}

bool settings_t$destroy( settings_t* restrict _settings ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    {
        l_returnValue = window_t$destroy( &( _settings->window ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        free( _settings->version );

        _settings->version = NULL;

        free( _settings->identifier );

        _settings->identifier = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$load$fromAsset( settings_t* restrict _settings,
                                const asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )debug,
                                      "Settings size: %zu\n", _asset->size );

        char* l_data = ( char* )malloc( ( _asset->size + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_data, _asset->data, _asset->size );

        l_data[ _asset->size ] = '\0';

        {
            char** l_lines = splitStringIntoArrayBySymbol( l_data, '\n' );

            if ( UNLIKELY( !arrayLength( l_lines ) ) ) {
                goto EXIT_SETTINGS_DATA_LINES;
            }

            {
                settingsOption_t** l_settingsOptions =
                    createArray( settingsOption_t* );

                {
#define INSERT_SETTINGS_OPTION( _array, _key, _storage )                \
    ( {                                                                 \
        settingsOption_t l_settingsOption = settingsOption_t$create();  \
        settingsOption_t$map( &l_settingsOption, _key, _storage );      \
        settingsOption_t* l_settingsOptionAllocated =                   \
            ( settingsOption_t* )malloc( sizeof( settingsOption_t ) );  \
        __builtin_memcpy( l_settingsOptionAllocated, &l_settingsOption, \
                          sizeof( settingsOption_t ) );                 \
        insertIntoArray( _array, l_settingsOptionAllocated );           \
    } )

                    INSERT_SETTINGS_OPTION( &l_settingsOptions, "window_name",
                                            &( _settings->window.name ) );
                    INSERT_SETTINGS_OPTION( &l_settingsOptions, "window_width",
                                            &( _settings->window.width ) );
                    INSERT_SETTINGS_OPTION( &l_settingsOptions, "window_height",
                                            &( _settings->window.height ) );
                    INSERT_SETTINGS_OPTION( &l_settingsOptions,
                                            "window_desired_FPS",
                                            &( _settings->window.desiredFPS ) );
                    INSERT_SETTINGS_OPTION( &l_settingsOptions, "window_vsync",
                                            &( _settings->window.vsync ) );
                    INSERT_SETTINGS_OPTION(
                        &l_settingsOptions, "limited_loop_desired_FPS",
                        &( _settings->limitedLoopDesiredFPS ) );

#undef INSERT_SETTINGS_OPTION
                }

                FOR_ARRAY( char* const*, l_lines ) {
                    const char* l_line = sanitizeString( *_element );

                    if ( ( l_line ) && ( __builtin_strlen( l_line ) ) ) {
                        char** l_keyAndValue =
                            splitStringIntoArrayBySymbol( l_line, '=' );

                        if ( arrayLength( l_keyAndValue ) != 2 ) {
                            log$transaction$query$format(
                                ( logLevel_t )error, "Settings line: '%s'\n",
                                l_line );

                            goto LOOP_CONTINUE;
                        }

                        const char* l_key = arrayFirstElement( l_keyAndValue );
                        const char* l_value = arrayLastElement( l_keyAndValue );

                        bool l_result = false;

                        FOR_ARRAY( settingsOption_t* const*,
                                   l_settingsOptions ) {
                            l_result = settingsOption_t$bind( *_element, l_key,
                                                              l_value );

                            if ( l_result ) {
                                break;
                            }
                        }

                        if ( UNLIKELY( !l_result ) ) {
                            log$transaction$query$format(
                                ( logLevel_t )error,
                                "Corrupted settings key: '%s'\n", l_key );

                            goto LOOP_CONTINUE;
                        }

                    LOOP_CONTINUE:
                        FREE_ARRAY_ELEMENTS( l_keyAndValue );
                        FREE_ARRAY( l_keyAndValue );
                    }
                }

                FOR_ARRAY( settingsOption_t* const*, l_settingsOptions ) {
                    if ( !settingsOption_t$unmap( *_element ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Settings option unmap\n" );

                        break;
                    }

                    if ( !settingsOption_t$destroy( *_element ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Settings option destroy\n" );

                        break;
                    }
                }

                FREE_ARRAY_ELEMENTS( l_settingsOptions );
                FREE_ARRAY( l_settingsOptions );
            }

        EXIT_SETTINGS_DATA_LINES:
            FREE_ARRAY_ELEMENTS( l_lines );
            FREE_ARRAY( l_lines );
        }

        free( l_data );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$load$fromPath( settings_t* restrict _settings,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
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
                                      "Settings path: '%s.%s'\n", _fileName,
                                      _fileExtension );

        // Parse settings file
        {
            asset_t l_settingsAsset = asset_t$create();

            {
                char* l_filePath = duplicateString( "." );

                l_returnValue = !!( concatBeforeAndAfterString(
                    &l_filePath, _fileName, _fileExtension ) );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_FILE_PATH_CONCAT;
                }

                l_returnValue = asset_t$load( &l_settingsAsset, l_filePath );

            EXIT_FILE_PATH_CONCAT:
                free( l_filePath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }

            l_returnValue =
                settings_t$load$fromAsset( _settings, &l_settingsAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = asset_t$unload( &l_settingsAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = asset_t$destroy( &l_settingsAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$unload( settings_t* restrict _settings ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
