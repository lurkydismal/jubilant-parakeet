#include "stdfunc.h"

#include <ctype.h>
#include <glob.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "asset_t.h"
#include "log.h"

#define COMPILATION_TIME_AS_SEED                                      \
    ( __TIME__[ 0 ] + __TIME__[ 1 ] + __TIME__[ 3 ] + __TIME__[ 4 ] + \
      __TIME__[ 6 ] + __TIME__[ 7 ] )

static size_t g_seed = COMPILATION_TIME_AS_SEED;

#undef COMPILATION_TIME_AS_SEED

void randomNumber$seed$set( const size_t _seed ) {
    g_seed = _seed;
}

size_t randomNumber$seed$get( void ) {
    return ( g_seed );
}

size_t randomNumber( void ) {
    size_t l_returnValue = 0;

    {
        g_seed ^= ( g_seed << 13 );
        g_seed ^= ( g_seed >> 17 );
        g_seed ^= ( g_seed << 5 );

        l_returnValue = g_seed;
    }

    return ( l_returnValue );
}

size_t concatBeforeAndAfterString( char* restrict* restrict _string,
                                   const char* restrict _beforeString,
                                   const char* restrict _afterString ) {
    size_t l_returnValue = 0;

    {
        if ( UNLIKELY( !_string ) || UNLIKELY( !*_string ) ) {
            log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

            goto EXIT;
        }

        const size_t l_stringLength = __builtin_strlen( *_string );

        const size_t l_beforeStringLength =
            ( ( _beforeString ) ? ( __builtin_strlen( _beforeString ) )
                                : ( 0 ) );

        const size_t l_afterStringLegnth =
            ( ( _afterString ) ? ( __builtin_strlen( _afterString ) ) : ( 0 ) );

        const size_t l_totalLength =
            ( l_beforeStringLength + l_stringLength + l_afterStringLegnth );

        if ( UNLIKELY( !l_totalLength ) ) {
            goto EXIT;
        }

        {
            // String
            {
                *_string = ( char* )realloc(
                    *_string, ( l_totalLength + 1 ) * sizeof( char ) );

                if ( l_stringLength && l_beforeStringLength ) {
                    __builtin_memmove( ( l_beforeStringLength + *_string ),
                                       *_string, l_stringLength );
                }
            }

            // Before
            if ( l_beforeStringLength ) {
                __builtin_memcpy( *_string, _beforeString,
                                  l_beforeStringLength );
            }

            // After
            if ( l_afterStringLegnth ) {
                __builtin_memcpy(
                    ( l_beforeStringLength + l_stringLength + *_string ),
                    _afterString, l_afterStringLegnth );
            }

            ( *_string )[ l_totalLength ] = '\0';
        }

        l_returnValue = l_totalLength;
    }

EXIT:
    return ( l_returnValue );
}

char* sanitizeString( const char* restrict _string ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        const size_t l_stringLength = __builtin_strlen( _string );
        char* l_buffer =
            ( char* )malloc( ( l_stringLength + 1 ) * sizeof( char ) );
        size_t l_bufferLength = 0;

        for ( const char* _symbol = _string;
              _symbol < ( _string + l_stringLength ); _symbol++ ) {
            if ( isspace( *_symbol ) ) {
                continue;

            } else if ( *_symbol == COMMENT_SYMBOL ) {
                break;
            }

            l_buffer[ l_bufferLength ] = *_symbol;
            l_bufferLength++;
        }

        l_buffer[ l_bufferLength ] = '\0';
        l_bufferLength++;

        l_buffer = ( char* )realloc( l_buffer, l_bufferLength );

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}

char** splitStringIntoArray( const char* restrict _string,
                             const char* restrict _delimiter ) {
    char** l_returnValue = createArray( char* );

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_delimiter ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        char* l_string = duplicateString( _string );
        char* l_splitted = strtok( l_string, _delimiter );

        while ( l_splitted ) {
            insertIntoArray( &l_returnValue, duplicateString( l_splitted ) );

            l_splitted = strtok( NULL, _delimiter );
        }

        free( l_string );
    }

EXIT:
    return ( l_returnValue );
}

char** splitStringIntoArrayBySymbol( const char* restrict _string,
                                     const char _symbol ) {
    char** l_returnValue = createArray( char* );

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        char* l_string = duplicateString( _string );

        {
            char* l_buffer = l_string;
            char* l_previousSplitted = ( l_buffer - 1 );

            while ( *l_buffer ) {
                if ( *l_buffer == _symbol ) {
                    *l_buffer = '\0';

                    {
                        if ( ( l_buffer - l_previousSplitted ) < ( 1 + 1 ) ) {
                            goto LOOP_CONTINUE;
                        }

                        insertIntoArray(
                            &l_returnValue,
                            duplicateString( l_previousSplitted + 1 ) );
                    }

                LOOP_CONTINUE:
                    l_previousSplitted = l_buffer;
                }

                l_buffer++;
            }

            if ( ( l_buffer - l_previousSplitted ) >= ( 1 + 1 ) ) {
                insertIntoArray( &l_returnValue,
                                 duplicateString( l_previousSplitted + 1 ) );
            }
        }

        free( l_string );
    }

EXIT:
    return ( l_returnValue );
}

ssize_t findStringInArray( const char** restrict _array,
                           const size_t _arrayLength,
                           const char* restrict _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_arrayLength ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        ssize_t l_index = -1;

        FOR_RANGE( size_t, 0, _arrayLength ) {
            const char* l_value = _array[ _index ];

            if ( UNLIKELY( !l_value ) ) {
                continue;
            }

            if ( __builtin_strcmp( l_value, _value ) == 0 ) {
                l_index = _index;

                break;
            }
        }

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

ssize_t findInArray( const size_t* restrict _array,
                     const size_t _arrayLength,
                     const size_t _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_arrayLength ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        ssize_t l_index = -1;

        FOR_RANGE( size_t, 0, _arrayLength ) {
            if ( _array[ _index ] == _value ) {
                l_index = _index;

                break;
            }
        }

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

char* getApplicationDirectoryAbsolutePath( void ) {
    char* l_returnValue = NULL;

    {
        char* l_executablePath = ( char* )malloc( PATH_MAX * sizeof( char ) );

        // Get executable path
        {
            ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath, ( PATH_MAX - 1 ) );

            if ( UNLIKELY( l_executablePathLength == -1 ) ) {
                log$transaction$query( ( logLevel_t )error, "readlink\n" );

                free( l_executablePath );

                goto EXIT;
            }

            l_executablePath[ l_executablePathLength ] = '\0';
        }

        char* l_directoryPath = NULL;

        // Get directory path
        {
            char* l_lastSlash = __builtin_strrchr( l_executablePath, '/' );

            if ( UNLIKELY( !l_lastSlash ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Extracting directory: '%s'\n",
                                              l_executablePath );

                goto EXIT;
            }

            const ssize_t l_lastSlashIndex = ( l_lastSlash - l_executablePath );

            l_directoryPath = l_executablePath;

            // Do not move the beginning
            trim( &l_directoryPath, 0, l_lastSlashIndex );

            if ( UNLIKELY( !concatBeforeAndAfterString( &l_directoryPath, NULL,
                                                        "/" ) ) ) {
                free( l_directoryPath );

                goto EXIT;
            }
        }

        l_returnValue = l_directoryPath;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Remove asset_t
char** getPathsByGlob( const char* restrict _glob,
                       const char* restrict _directory ) {
    char** l_returnValue = NULL;

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = createArray( char* );

        {
            glob_t l_globBuffer;

            // Get files
            {
                char* l_glob = duplicateString( _glob );

                concatBeforeAndAfterString( &l_glob, _directory, NULL );

                concatBeforeAndAfterString(
                    &l_glob, asset_t$loader$assetsDirectory$get(), NULL );

                int l_result = glob(
                    l_glob,
                    ( GLOB_NOSORT |
                      GLOB_TILDE_CHECK // Carry out tilde expansion. If a tilde
                                       // '~' is the only character in the
                                       // pattern, or an initial tilde is
                                       // followed immediately by a slash '/',
                                       // then the home directory of the caller
                                       // is substituted for the tilde. If an
                                       // initial tilde is followed by a
                                       // username '~andrea/bin', then the tilde
                                       // and username are substituted by the
                                       // home directory of that user. If the
                                       // username is invalid, or the home
                                       // directory cannot be determined, then
                                       // no substitution is performed If the
                                       // username is invalid, or the home
                                       // directory cannot be determined, then
                                       // glob() returns GLOB_NOMATCH to
                                       // indicate an error
                      ),
                    NULL, &l_globBuffer );

                free( l_glob );

                if ( UNLIKELY( l_result == GLOB_ABORTED ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Glob failed\n" );

                    goto EXIT;
                }
            }

            if ( !( l_globBuffer.gl_pathc ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Glob empty: '%s' in '%s'\n",
                                              _glob, _directory );

                goto EXIT_GLOB;
            }

            FOR_RANGE( size_t, 0, ( l_globBuffer.gl_pathc ) ) {
                char* l_path = NULL;

                // Strip directory
                {
                    const char* l_fullPath = l_globBuffer.gl_pathv[ _index ];
                    const size_t l_fullPathLength =
                        __builtin_strlen( l_fullPath );
                    // 0 if NULL
                    const size_t l_directoryLength =
                        ( ( _directory ) ? ( __builtin_strlen( _directory ) )
                                         : ( 0 ) );
                    const size_t l_fileNameLength =
                        ( l_fullPathLength - l_directoryLength );

                    l_path = ( char* )malloc( ( l_fileNameLength + 1 ) *
                                              sizeof( char ) );

                    __builtin_memcpy( l_path,
                                      ( l_fullPath + l_directoryLength ),
                                      l_fileNameLength );

                    l_path[ l_fileNameLength ] = '\0';
                }

                insertIntoArray( &l_returnValue, l_path );
            }

        EXIT_GLOB:
            globfree( &l_globBuffer );
        }
    }

EXIT:
    return ( l_returnValue );
}
