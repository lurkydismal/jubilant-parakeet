#include "state_t.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include "stdfunc.h"

state_t state_t$create( SDL_Renderer* _renderer,
                        const char* _path,
                        const char* _name,
                        bool _isActionable ) {
    state_t l_returnValue = DEFAULT_STATE;

    l_returnValue.renderer = _renderer;
    l_returnValue.isActionable = _isActionable;

    // Load animation
    {
        char* l_pattern = duplicateString( "_*-*." );

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

void state_t$destroy( state_t* _state ) {
    animation_t$unload( &( _state->animation ) );
    boxes_t$unload( &( _state->boxes ) );
}

void state_t$step( state_t* _state, bool _canLoop ) {
    animation_t$step( &( _state->animation ), _canLoop );
    boxes_t$step( &( _state->boxes ), _canLoop );
}

void state_t$render( state_t* _state,
                     const SDL_FRect* _targetRectanble,
                     bool _doDrawBoxes ) {
    animation_t$render( _state->renderer, &( _state->animation ),
                        _targetRectanble );
    if ( _doDrawBoxes ) {
        boxes_t$render( _state->renderer, &( _state->boxes ), _targetRectanble,
                        true );
    }
}
