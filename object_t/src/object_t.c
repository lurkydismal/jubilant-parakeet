#include "object_t.h"

#include <SDL3/SDL_log.h>

#include "stdfunc.h"

object_t object_t$create( float _worldX, float _worldY ) {
    object_t l_returnValue = DEFAULT_OBJECT;

    l_returnValue.states = ( state_t** )createArray( sizeof( state_t* ) );

    l_returnValue.worldX = _worldX;
    l_returnValue.worldY = _worldY;

    return ( l_returnValue );
}

void object_t$destroy( object_t* _object ) {
    FOR_ARRAY( state_t**, _object->states ) {
        state_t$unload( *_element );
    }
}

void object_t$step( object_t* _object, float _velocityX, float _velocityY ) {
    _object->worldX += _velocityX;
    _object->worldY += _velocityY;

    state_t$step( _object->currentState );
}

void object_t$render( const object_t* _object,
                      const object_t* _camera,
                      bool _doDrawBoxes ) {
    const SDL_FRect l_targetRectangle = {
        .x = ( _object->worldX - _camera->worldX ),
        .y = ( _object->worldY - _camera->worldY ),
        .w = 0,
        .w = 0 };

    state_t$render( _object->currentState, &l_targetRectangle, _doDrawBoxes );
}

void object_t$add$state( object_t* _object,
                         SDL_Renderer* _renderer,
                         const char* _path,
                         const char* _pattern,
                         bool _isActionable,
                         bool _canLoop ) {
    state_t l_state =
        state_t$load( _renderer, _path, _pattern, _isActionable, _canLoop );

    state_t* l_stateAllocated = ( state_t* )SDL_malloc( sizeof( state_t ) );
    SDL_memcpy( l_stateAllocated, &l_state, sizeof( state_t ) );

    insertIntoArray( ( void*** )( &( _object->states ) ), l_stateAllocated );

    if ( arrayLength( _object->states ) == 1 ) {
        _object->currentState = _object->states[ 1 ];
    }
}
