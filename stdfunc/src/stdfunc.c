#include "stdfunc.h"

#if 0
#include <omp.h>
#endif

#if 0
#include "_useCallback.h"
#endif

#define arrayLengthPointer( _array ) ( ( size_t* )( &( _array[ 0 ] ) ) )

size_t lengthOfNumber( size_t _number ) {
    if ( _number == 0 ) {
        return ( 1 );
    }

    size_t l_length = 0;

    do {
        l_length++;
        _number /= 10;
    } while ( _number );

    return ( l_length );
}

Sint64 power( Sint64 _base, Uint8 _exponent ) {
    const Uint8 l_highestBitSet[] = {
        0,
        1,
        2,
        2,
        3,
        3,
        3,
        3,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        6,
        255, // anything past 63 is a
             // guaranteed overflow with (
             // _base > 1 )
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
    };

    Sint64 l_returnValue = 1;

    switch ( l_highestBitSet[ _exponent ] ) {
        case 255: // we use 255 as an overflow marker and return 0 on
                  // overflow/underflow
            if ( _base == 1 ) {
                return ( 1 );
            }

            if ( _base == -1 ) {
                return ( 1 - 2 * ( _exponent & 1 ) );
            }

            return ( 0 );

        case 6: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }

            _exponent >>= 1;
            _base *= _base;
        }

        case 5: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }

            _exponent >>= 1;
            _base *= _base;
        }

        case 4: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }

            _exponent >>= 1;
            _base *= _base;
        }

        case 3: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }

            _exponent >>= 1;
            _base *= _base;
        }

        case 2: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }

            _exponent >>= 1;
            _base *= _base;
        }

        case 1: {
            if ( _exponent & 1 ) {
                l_returnValue *= _base;
            }
        }

        default: {
            return ( l_returnValue );
        }
    }
}

char* convertNumberToString( size_t _number ) {
    const size_t l_lengthOfNumber = lengthOfNumber( _number );
    char* l_buffer = ( char* )SDL_malloc( l_lengthOfNumber + 1 );

#pragma omp simd
    for ( ssize_t _characterIndex = ( l_lengthOfNumber - 1 );
          _characterIndex >= 0; _characterIndex-- ) {
        l_buffer[ _characterIndex ] =
            ( char )( ( ( _number / ( power( 10, ( l_lengthOfNumber -
                                                   _characterIndex - 1 ) ) ) ) %
                        10 ) +
                      '0' );
    }

    l_buffer[ l_lengthOfNumber ] = '\0';

    return ( l_buffer );
}

char* duplicateString( const char* _string ) {
    const size_t l_stringLength = SDL_strlen( _string );

    char* l_duplicatedString =
        ( char* )SDL_malloc( ( l_stringLength + 1 ) * sizeof( char ) );

    SDL_memcpy( l_duplicatedString, _string, ( l_stringLength + 1 ) );

    return ( l_duplicatedString );
}

ssize_t findSymbolInString( const char* _string, const char _symbol ) {
    ssize_t l_returnValue = -1;

    const size_t l_stringLength = SDL_strlen( _string );

    for ( size_t _index = 0; _index < l_stringLength; _index++ ) {
        const char l_symbol = _string[ _index ];

        if ( l_symbol == _symbol ) {
            l_returnValue = _index;

            break;
        }
    }

    return ( l_returnValue );
}

ssize_t findLastSymbolInString( const char* _string, const char _symbol ) {
    ssize_t l_returnValue = -1;

    const size_t l_stringLength = SDL_strlen( _string );

    // TODO: Start from the end and break on first match
    for ( size_t _index = 0; _index < l_stringLength; _index++ ) {
        const char l_symbol = _string[ _index ];

        if ( l_symbol == _symbol ) {
            l_returnValue = _index;
        }
    }

    return ( l_returnValue );
}

size_t concatBeforeAndAfterString( char** _string,
                                   const char* _beforeString,
                                   const char* _afterString ) {
    const size_t l_stringLength = SDL_strlen( *_string );
    const size_t l_beforeStringLength = SDL_strlen( _beforeString );
    const size_t l_afterStringLegnth = SDL_strlen( _afterString );
    const size_t l_totalLength =
        ( l_beforeStringLength + l_stringLength + l_afterStringLegnth );

    char* l_buffer = ( char* )SDL_malloc( l_stringLength * sizeof( char ) );

    SDL_memcpy( l_buffer, *_string, l_stringLength );

    *_string = ( char* )SDL_realloc( *_string,
                                     ( l_totalLength + 1 ) * sizeof( char ) );

    SDL_memcpy( *_string, _beforeString, l_beforeStringLength );
    SDL_memcpy( ( l_beforeStringLength + *_string ), l_buffer, l_stringLength );
    SDL_memcpy( ( l_beforeStringLength + l_stringLength + *_string ),
                _afterString, l_afterStringLegnth );

    ( *_string )[ l_totalLength ] = '\0';

    SDL_free( l_buffer );

    return ( l_totalLength );
}

char* sanitizeString( const char* _string ) {
    const size_t l_stringLength = SDL_strlen( _string );
    char* l_buffer =
        ( char* )SDL_malloc( ( l_stringLength + 1 ) * sizeof( char ) );
    size_t l_bufferLength = 0;

#define COMMENT_SYMBOL ( '#' )

    for ( const char* _symbol = _string; _symbol < ( _string + l_stringLength );
          _symbol++ ) {
        if ( SDL_isspace( *_symbol ) ) {
            continue;

        } else if ( *_symbol == COMMENT_SYMBOL ) {
            break;
        }

        l_buffer[ l_bufferLength ] = *_symbol;
        l_bufferLength++;
    }

#undef COMMENT_SYMBOL

    l_buffer[ l_bufferLength ] = '\0';
    l_bufferLength++;

    l_buffer = ( char* )SDL_realloc( l_buffer, l_bufferLength );

    return ( l_buffer );
}

char** splitStringIntoArray( const char* _string, const char* _delimiter ) {
    char** l_returnValue = ( char** )createArray( sizeof( char* ) );
    char* l_string = duplicateString( _string );
    char* l_splitted = strtok( l_string, _delimiter );

    while ( l_splitted ) {
        insertIntoArray( ( void*** )&l_returnValue,
                         duplicateString( l_splitted ) );

        l_splitted = strtok( NULL, _delimiter );
    }

    SDL_free( l_string );

    return ( l_returnValue );
}

inline void** createArray( const size_t _elementSize ) {
    void** l_array = ( void** )SDL_malloc( 1 * _elementSize );

    *arrayLengthPointer( l_array ) = ( size_t )( 1 );

    return ( l_array );
}

void preallocateArray( void*** _array, const size_t _length ) {
    const size_t l_currentArrayLength = arrayLength( *_array );

    *_array = ( void** )SDL_realloc( *_array,
                                     ( ( l_currentArrayLength + _length + 1 ) *
                                       sizeof( ( *_array )[ 0 ] ) ) );

    *arrayLengthPointer( *_array ) =
        ( size_t )( l_currentArrayLength + _length + 1 );
}

size_t insertIntoArray( void*** _array, void* _value ) {
    const size_t l_arrayLength = arrayLength( *_array );
    const size_t l_index = ( 1 + l_arrayLength );

    *_array = ( void** )SDL_realloc(
        *_array, ( l_index + 1 ) * sizeof( ( *_array )[ 0 ] ) );

    ( *_array )[ l_index ] = _value;

    ( *arrayLengthPointer( *_array ) )++;

    return ( l_index );
}

inline void insertIntoArrayByIndex( void*** _array,
                                    const size_t _index,
                                    void* _value ) {
    ( *_array )[ _index ] = _value;
}

ssize_t findStringInArray( const char** _array,
                           const size_t _arrayLength,
                           const char* _value ) {
    ssize_t l_index = -1;

    for ( size_t _index = 0; _index < _arrayLength; _index++ ) {
        if ( SDL_strcmp( _array[ _index ], _value ) == 0 ) {
            l_index = _index;

            break;
        }
    }

    return ( l_index );
}

ssize_t findInArray( const size_t* _array,
                     const size_t _arrayLength,
                     const size_t _value ) {
    ssize_t l_index = -1;

    for ( size_t _index = 0; _index < _arrayLength; _index++ ) {
        if ( _array[ _index ] == _value ) {
            l_index = _index;

            break;
        }
    }

    return ( l_index );
}

inline bool containsString( const char** _array,
                            const size_t _arrayLength,
                            const char* _value ) {
    return ( findStringInArray( _array, _arrayLength, _value ) >= 0 );
}

inline bool contains( const size_t* _array,
                      const size_t _arrayLength,
                      const size_t _value ) {
    return ( findInArray( _array, _arrayLength, _value ) >= 0 );
}

#if 0
enum SETTINGS_ITEM_TYPE { KEY, VALUE };

void freeSettingsContent( char*** _content ) {
#pragma omp simd
    FOR_ARRAY( char***, _content ) {
        SDL_free( ( *_element )[ 0 ] );

        SDL_free( ( *_element )[ 1 ] );

        SDL_free( ( *_element ) );
    }

    SDL_free( _content );
}

static inline ssize_t findInSettings( char** const* _settings,
                                      const char* _string,
                                      const enum SETTINGS_ITEM_TYPE _type ) {
    ssize_t l_index = -1;

    FOR_ARRAY( char** const*, _settings ) {
        const char* l_string = ( *_element )[ _type ];

        if ( SDL_strcmp( l_string, _string ) == 0 ) {
            l_index = ( _element - arrayFirstElementPointer( _settings ) + 1 );

            break;
        }
    }

    return ( l_index );
}

inline ssize_t findKeyInSettings( char*** _settings, const char* _key ) {
    return ( findInSettings( _settings, _key, KEY ) );
}

inline ssize_t findValueInSettings( char*** _settings, const char* _value ) {
    return ( findInSettings( _settings, _value, VALUE ) );
}

inline bool containsKeyInSettings( char*** _settings, const char* _value ) {
    return ( findKeyInSettings( _settings, _value ) >= 0 );
}

char*** getLabelFromSettingsOrDefault( const char* _label,
                                       const char* _default ) {
    char*** l_returnValue = NULL;

    if ( _useCallback( "core$getSettingsContentByLabel", &l_returnValue,
                       _label ) != 0 ) {
        _useCallback( "core$readSettingsFromString", _default );

        _useCallback( "core$getSettingsContentByLabel", &l_returnValue,
                      _label );
    }

    return ( l_returnValue );
}

char* getKeyFromSettingsOrDefault( const char* _label,
                                   const char* _key,
                                   const char* _default ) {
    char* l_returnValue = NULL;
    char*** l_settings;

    if ( _useCallback( "core$getSettingsContentByLabel", &l_settings,
                       _label ) == 0 ) {
        const ssize_t l_settingIndex = findKeyInSettings( l_settings, _key );

        if ( l_settingIndex >= 0 ) {
            l_returnValue = duplicateString( l_settings[ l_settingIndex ][ 1 ] );

            freeSettingsContent( l_settings );

        } else {
            _useCallback( "core$changeSettingsKeyByLabel", _key, _label,
                          _default );

            l_returnValue = duplicateString( _default );
        }
    }

    return ( l_returnValue );
}
#endif
