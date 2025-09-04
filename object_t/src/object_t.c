#include "object_t.h"

#include <SDL3/SDL_rect.h>

#include "log.h"
#include "stdfunc.h"

object_t object_t$create( void ) {
    object_t l_returnValue = DEFAULT_OBJECT;

    l_returnValue.states = createArray( state_t* );

    return ( l_returnValue );
}

bool object_t$destroy( object_t* restrict _object ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        FOR_ARRAY( state_t* const*, _object->states ) {
            l_returnValue = state_t$destroy( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying state\n" );

                goto EXIT;
            }
        }

        FREE_ARRAY_ELEMENTS( _object->states );

        FREE_ARRAY( _object->states );

        _object->states = NULL;

        _object->currentState = NULL;
        _object->worldX = 0;
        _object->worldY = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// After - animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool object_t$state$add$fromPaths( object_t* restrict _object,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxesPath,
                                   char* const* restrict _animationPaths,
                                   bool _isActionable,
                                   bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesPath ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationPaths ) ||
         UNLIKELY( !arrayLength( _animationPaths ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

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
                                   "Loading state from paths\n" );

            state_t$destroy( &l_state );

            goto EXIT;
        }

        insertIntoArray( &( _object->states ), clone( &l_state ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$state$add$fromGlob( object_t* restrict _object,
                                  SDL_Renderer* _renderer,
                                  const char* restrict _boxesGlob,
                                  const char* restrict _animationGlob,
                                  bool _isActionable,
                                  bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

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
                                   "Loading state from glob\n" );

            state_t$destroy( &l_state );

            goto EXIT;
        }

        insertIntoArray( &( _object->states ), clone( &l_state ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$state$remove( object_t* restrict _object,
                            state_t* restrict _state ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = state_t$unload( _state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Unloading state\n" );

            goto EXIT;
        }

        l_returnValue = state_t$destroy( _state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying state\n" );

            goto EXIT;
        }

        l_returnValue = pluckArray( &( _object->states ), _state );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Removing state from array\n" );

            goto EXIT;
        }

        free( _state );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool object_t$states$remove( object_t* restrict _object ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_object ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        FOR_ARRAY( state_t* const*, _object->states ) {
            l_returnValue = object_t$state$remove( _object, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing state from object\n" );

                goto EXIT;
            }
        }

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_object->currentState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = state_t$step( _object->currentState );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping state\n" );

            goto EXIT;
        }

        l_returnValue = object_t$move( _object, _velocityX, _velocityY );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping state\n" );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        const SDL_FRect l_targetRectangle = {
            .x = ( _object->worldX - _cameraRectangle->x ),
            .y = ( _object->worldY - _cameraRectangle->y ),
            .w = 0,
            .h = 0 };

        l_returnValue = state_t$render( _object->currentState,
                                        &l_targetRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering state\n" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
