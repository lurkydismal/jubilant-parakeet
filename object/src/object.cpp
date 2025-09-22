#include "object.hpp"

#include <SDL3/SDL_rect.h>

#include "log.hpp"
#include "stdfunc.hpp"

bool object_t$state$add$fromPaths( object_t* restrict _object,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxesPath,
                                   char* const* restrict _animationPaths,
                                   char* restrict _name,
                                   bool _isActionable,
                                   bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesPath ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationPaths ) ||
         UNLIKELY( !arrayLength( _animationPaths ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        state_t l_state = state_t$create();

        l_state.renderer = _renderer;
        l_state.canLoop = _canLoop;
        l_state.isActionable = _isActionable;

        l_returnValue =
            state_t$load$fromPaths( &l_state, _boxesPath, _animationPaths );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Loading state from paths" );

            state_t$destroy( &l_state );

            goto EXIT;
        }

        insertIntoArray( &( _object->states ), clone( &l_state ) );
        insertIntoArray( &( _object->stateNames ), duplicateString( _name ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$state$add$fromGlob( object_t* restrict _object,
                                  SDL_Renderer* _renderer,
                                  const char* restrict _boxesGlob,
                                  const char* restrict _animationGlob,
                                  char* restrict _name,
                                  bool _isActionable,
                                  bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        state_t l_state = state_t$create();

        l_state.renderer = _renderer;
        l_state.canLoop = _canLoop;
        l_state.isActionable = _isActionable;

        l_returnValue =
            state_t$load$fromGlob( &l_state, _boxesGlob, _animationGlob );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Loading state from glob" );

            state_t$destroy( &l_state );

            goto EXIT;
        }

        insertIntoArray( &( _object->states ), clone( &l_state ) );
        insertIntoArray( &( _object->stateNames ), duplicateString( _name ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static bool object_t$state$remove( object_t* restrict _object,
                                   state_t* restrict _state ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = state_t$unload( _state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Unloading state" );

            goto EXIT;
        }

        l_returnValue = state_t$destroy( _state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying state" );

            goto EXIT;
        }

        {
            const size_t l_stateIndex = findInArray( _object->states, _state );
            char* l_name = _object->stateNames[ l_stateIndex ];

            l_returnValue = ( l_stateIndex != SIZE_MAX );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_REMOVE;
            }

            l_returnValue =
                pluckArrayByIndex( &( _object->states ), l_stateIndex );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_REMOVE;
            }

            l_returnValue =
                pluckArrayByIndex( &( _object->stateNames ), l_stateIndex );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_REMOVE;
            }

            free( l_name );

        EXIT_REMOVE:
            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing state from object" );

                goto EXIT;
            }
        }

        free( _state );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$state$remove$byName( object_t* restrict _object,
                                   char* restrict _name ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_name ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const ssize_t l_index = findStringInArray( _object->stateNames, _name );

        char* l_name = _object->stateNames[ l_index ];
        state_t* l_state = _object->states[ l_index ];

        l_returnValue = object_t$state$remove( _object, l_state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Removing state from object" );

            goto EXIT;
        }

        l_returnValue = pluckArrayByValue( &( _object->stateNames ), l_name );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Removing state name from object" );

            goto EXIT;
        }

        free( l_name );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$states$remove( object_t* restrict _object ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FOR_RANGE( arrayLength_t, 0, arrayLength( _object->states ) ) {
            l_returnValue =
                object_t$state$remove( _object, _object->states[ _index ] );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing state from object" );

                goto EXIT;
            }
        }

        _object->currentState = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$state$change$byName( object_t* restrict _object,
                                   char* restrict _name ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_name ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const ssize_t l_index = findStringInArray( _object->stateNames, _name );

        state_t* l_state = _object->states[ l_index ];

        _object->currentState = l_state;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$move( object_t* restrict _object,
                    const float _x,
                    const float _y ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( ( _x == 0 ) && ( _y == 0 ) ) ) {
        l_returnValue = true;

        goto EXIT;
    }

    {
        _object->worldX += _x;

        _object->worldX = clamp$float( _object->worldX, _object->worldXMin,
                                       _object->worldXMax );

        _object->worldY += _y;

        _object->worldY = clamp$float( _object->worldY, _object->worldYMin,
                                       _object->worldYMax );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$step( object_t* restrict _object,
                    float _velocityX,
                    float _velocityY ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_object->currentState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = state_t$step( _object->currentState );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping state" );

            goto EXIT;
        }

        l_returnValue = object_t$move( _object, _velocityX, _velocityY );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping state" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$render$rotated( const object_t* restrict _object,
                              const double _angle,
                              SDL_FlipMode _flipMode,
                              const SDL_FRect* restrict _cameraRectangle,
                              bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const SDL_FRect l_targetRectangle = {
            .x = ( _object->worldX - _cameraRectangle->x ),
            .y = ( _object->worldY - _cameraRectangle->y ),
            .w = 0,
            .h = 0,
        };

        l_returnValue =
            state_t$render$rotated( _object->currentState, _angle, _flipMode,
                                    &l_targetRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering state" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$render( const object_t* restrict _object,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const SDL_FRect l_targetRectangle = {
            .x = ( _object->worldX - _cameraRectangle->x ),
            .y = ( _object->worldY - _cameraRectangle->y ),
            .w = 0,
            .h = 0,
        };

        l_returnValue = state_t$render( _object->currentState,
                                        &l_targetRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering state" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
