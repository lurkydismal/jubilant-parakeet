#include "state_t.h"

#include "log.h"
#include "stdfunc.h"

state_t state_t$create( void ) {
    state_t l_returnValue = DEFAULT_STATE;

    l_returnValue.animation = animation_t$create();
    l_returnValue.boxes = boxes_t$create();

    return ( l_returnValue );
}

bool state_t$destroy( state_t* restrict _state ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = animation_t$destroy( &( _state->animation ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying animation" );

            goto EXIT;
        }

        l_returnValue = boxes_t$destroy( &( _state->boxes ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying boxes" );

            goto EXIT;
        }

        _state->renderer = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$load$fromPaths( state_t* restrict _state,
                             char* restrict _boxesPath,
                             char* const* restrict _animationPath ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesPath ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationPath ) ||
         UNLIKELY( !arrayLength( _animationPath ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        // Animation
        {
            l_returnValue = animation_t$load$fromPaths(
                &( _state->animation ), _state->renderer, _animationPath );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading animation from paths" );

                goto EXIT;
            }
        }

        // Boxes
        {
            char** l_boxes = createArray( char* );

            insertIntoArray( &l_boxes, _boxesPath );

            l_returnValue =
                boxes_t$load$fromPaths( &( _state->boxes ), l_boxes );

            FREE_ARRAY( l_boxes );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading boxes from paths" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$load$fromGlob( state_t* restrict _state,
                            const char* restrict _boxesGlob,
                            const char* restrict _animationGlob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
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
        // Animation
        {
            l_returnValue = animation_t$load$fromGlob(
                &( _state->animation ), _state->renderer, _animationGlob );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading animation from glob" );

                goto EXIT;
            }
        }

        // Boxes
        {
            l_returnValue =
                boxes_t$load$fromGlob( &( _state->boxes ), _boxesGlob );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading boxes from glob" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$unload( state_t* restrict _state ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = animation_t$unload( &( _state->animation ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Unloading animation" );

            goto EXIT;
        }

        l_returnValue = boxes_t$unload( &( _state->boxes ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Unloading boxes" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$step( state_t* restrict _state ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            animation_t$step( &( _state->animation ), _state->canLoop );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping animation" );

            goto EXIT;
        }

        l_returnValue = boxes_t$step( &( _state->boxes ), _state->canLoop );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping boxes" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$render$rotated( const state_t* restrict _state,
                             const double _angle,
                             SDL_FlipMode _flipMode,
                             const SDL_FRect* restrict _cameraRectangle,
                             bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const boxes_t* l_targetBoxes = &( _state->animation.targetBoxes );

        l_returnValue = ( l_targetBoxes->currentFrame <
                          arrayLength( l_targetBoxes->frames ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid target boxes current frame" );

            goto EXIT;
        }

        const size_t l_targetBoxesKeyFrameIndex = arrayFirstElement(
            l_targetBoxes->frames[ l_targetBoxes->currentFrame ] );

        l_returnValue = ( l_targetBoxesKeyFrameIndex <
                          arrayLength( l_targetBoxes->keyFrames ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Invalid target boxes key frame index" );

            goto EXIT;
        }

        // Always single box
        const SDL_FRect* l_targetBox =
            l_targetBoxes->keyFrames[ l_targetBoxesKeyFrameIndex ];

        const SDL_FRect l_targetRectangle = {
            ( _cameraRectangle->x + l_targetBox->x ),
            ( _cameraRectangle->y + l_targetBox->y ), l_targetBox->w,
            l_targetBox->h };

        l_returnValue = animation_t$render$rotated(
            &( _state->animation ), _state->renderer, _angle, _flipMode,
            &l_targetRectangle );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering animation" );

            goto EXIT;
        }

        if ( _doDrawBoxes ) {
            l_returnValue =
                boxes_t$render( &( _state->boxes ), _state->renderer,
                                &l_targetRectangle, true );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Rendering boxes" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool state_t$render( const state_t* restrict _state,
                     const SDL_FRect* restrict _cameraRectangle,
                     bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_state ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const SDL_FRect* l_targetBox =
            animation_t$currentTargetRectangle$get( &( _state->animation ) );

        const SDL_FRect l_targetRectangle = {
            ( _cameraRectangle->x + l_targetBox->x ),
            ( _cameraRectangle->y + l_targetBox->y ), l_targetBox->w,
            l_targetBox->h };

        l_returnValue = animation_t$render(
            &( _state->animation ), _state->renderer, &l_targetRectangle );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering animation" );

            goto EXIT;
        }

        if ( _doDrawBoxes ) {
            l_returnValue =
                boxes_t$render( &( _state->boxes ), _state->renderer,
                                &l_targetRectangle, true );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Rendering boxes" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
