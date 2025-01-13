#include "state_t.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include "stdfunc.h"

state_t state_t$load( SDL_Renderer* _renderer,
                      const char* _path,
                      const char* _name,
                      bool _isActionable,
                      bool _canLoop ) {
    state_t l_returnValue = DEFAULT_STATE;

    l_returnValue.renderer = _renderer;
    l_returnValue.isActionable = _isActionable;
    l_returnValue.canLoop = _canLoop;

    // Load animation
    {
        char* l_pattern = duplicateString( "_*x*_*-*." );

        // _name_*-*.bmp
        concatBeforeAndAfterString( &l_pattern, _name, "bmp" );

        l_returnValue.animation =
            animation_t$load( _renderer, _path, l_pattern );

        SDL_free( l_pattern );
    }

    // Load boxes
    {
        char* l_pattern = duplicateString( "_*." );

        // _name.txt
        concatBeforeAndAfterString( &l_pattern, _name, "txt" );

        l_returnValue.boxes = boxes_t$load( _path, l_pattern );

        SDL_free( l_pattern );
    }

    return ( l_returnValue );
}

void state_t$unload( state_t* _state ) {
    animation_t$unload( &( _state->animation ) );
    boxes_t$unload( &( _state->boxes ) );
}

void state_t$step( state_t* _state ) {
    animation_t$step( &( _state->animation ), _state->canLoop );
    boxes_t$step( &( _state->boxes ), _state->canLoop );
}

void state_t$render( const state_t* _state,
                     const SDL_FRect* _targetRectangle,
                     bool _doDrawBoxes ) {
    const animation_t* l_animation = &( _state->animation );
    const boxes_t* l_targetBoxes = &( l_animation->targetBoxes );

    // Always a single box
    const SDL_FRect* l_targetBox =
        l_targetBoxes->keyFrames
            [ l_targetBoxes->frames[ l_targetBoxes->currentFrame ][ 1 ] ];

    const SDL_FRect l_targetRectangle = {
        ( _targetRectangle->x + l_targetBox->x ),
        ( _targetRectangle->y + l_targetBox->y ), l_targetBox->w,
        l_targetBox->h };

    animation_t$render( _state->renderer, &( _state->animation ),
                        _targetRectangle );

    if ( _doDrawBoxes ) {
        boxes_t$render( _state->renderer, &( _state->boxes ), _targetRectangle,
                        true );
    }
}
