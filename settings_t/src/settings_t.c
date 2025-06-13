#include "settings_t.h"

#include <stdlib.h>

#include "log.h"
#include "settingsOption_t.h"
#include "stdfunc.h"

settings_t settings_t$create( void ) {
    settings_t l_returnValue = DEFAULT_SETTINGS;

    {
        l_returnValue.window = window_t$create();
        l_returnValue.controls = controls_t$create();
        l_returnValue.version = duplicateString( DEFAULT_SETTINGS_VERSION );
        l_returnValue.identifier = duplicateString( l_returnValue.window.name );
        l_returnValue.description =
            duplicateString( DEFAULT_SETTINGS_DESCRIPTION );
        l_returnValue.contactAddress =
            duplicateString( DEFAULT_SETTINGS_CONTACT_ADDRESS );
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

        free( _settings->description );

        _settings->description = NULL;

        free( _settings->contactAddress );

        _settings->contactAddress = NULL;

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

        char* l_dataWithNull =
            ( char* )malloc( ( _asset->size + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_dataWithNull, _asset->data, _asset->size );

        l_dataWithNull[ _asset->size ] = '\0';

        {
            char** l_lines =
                splitStringIntoArrayBySymbol( l_dataWithNull, '\n' );

            free( l_dataWithNull );

            if ( UNLIKELY( !arrayLength( l_lines ) ) ) {
                goto EXIT_SETTINGS_DATA_LINES;
            }

            {
                settingsOption_t** l_settingsOptions =
                    createArray( settingsOption_t* );

                {
#define INSERT_SETTINGS_OPTION( _array, _key, _storage )               \
    ( {                                                                \
        settingsOption_t l_settingsOption = settingsOption_t$create(); \
        settingsOption_t$map( &l_settingsOption, _key, _storage );     \
        insertIntoArray( _array, clone( &l_settingsOption ) );         \
    } )

                    // Window
                    {
                        INSERT_SETTINGS_OPTION( &l_settingsOptions,
                                                "window_name",
                                                &( _settings->window.name ) );
                        INSERT_SETTINGS_OPTION( &l_settingsOptions,
                                                "window_width",
                                                &( _settings->window.width ) );
                        INSERT_SETTINGS_OPTION( &l_settingsOptions,
                                                "window_height",
                                                &( _settings->window.height ) );
                        INSERT_SETTINGS_OPTION(
                            &l_settingsOptions, "window_desired_FPS",
                            &( _settings->window.desiredFPS ) );
                        INSERT_SETTINGS_OPTION( &l_settingsOptions,
                                                "window_vsync",
                                                &( _settings->window.vsync ) );
                    }

                    // Controls
                    {
                        // Directions by names
                        {
                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "up",
                                &( _settings->controls.up.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "down",
                                &( _settings->controls.down.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "left",
                                &( _settings->controls.left.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "right",
                                &( _settings->controls.right.scancode ) );
                        }

                        // Directions by notation
                        {
                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "8",
                                &( _settings->controls.up.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "2",
                                &( _settings->controls.down.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "4",
                                &( _settings->controls.left.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "6",
                                &( _settings->controls.right.scancode ) );
                        }

                        // Buttons by names
                        {
                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "light_attack",
                                &( _settings->controls.A.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "medium_attack",
                                &( _settings->controls.B.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "heavy_attack",
                                &( _settings->controls.C.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "shield",
                                &( _settings->controls.D.scancode ) );
                        }

                        // Buttons by notation
                        {
                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "A",
                                &( _settings->controls.A.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "B",
                                &( _settings->controls.B.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "C",
                                &( _settings->controls.C.scancode ) );

                            INSERT_SETTINGS_OPTION(
                                &l_settingsOptions, "D",
                                &( _settings->controls.D.scancode ) );
                        }

                        INSERT_SETTINGS_OPTION(
                            &l_settingsOptions, "background_index",
                            &( _settings->backgroundIndex ) );

                        INSERT_SETTINGS_OPTION( &l_settingsOptions, "HUD_index",
                                                &( _settings->HUDIndex ) );
                    }

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
                {
                    char* l_filePath = duplicateString( "." );

                    l_returnValue = !!( concatBeforeAndAfterString(
                        &l_filePath, _fileName, _fileExtension ) );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        goto EXIT_PATH_LOAD;
                    }

                    l_returnValue =
                        asset_t$load$fromPath( &l_settingsAsset, l_filePath );

                EXIT_PATH_LOAD:
                    free( l_filePath );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        goto EXIT_SETTINGS_LOAD;
                    }
                }

                l_returnValue =
                    settings_t$load$fromAsset( _settings, &l_settingsAsset );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_SETTINGS_LOAD;
                }

                l_returnValue = asset_t$unload( &l_settingsAsset );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_SETTINGS_LOAD;
                }
            }

        EXIT_SETTINGS_LOAD:
            if ( UNLIKELY( !asset_t$destroy( &l_settingsAsset ) ) ) {
                l_returnValue = false;

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
