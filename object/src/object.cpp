#include "object.hpp"

namespace object {

namespace {

template < typename... Arguments >
void _render( const object_t::position_t& _position,
              const state::state_t& _state,
              const boxes::box_t& _cameraBoxCoordinates,
              bool _doDrawBoxes,
              bool _doFillBoxes,
              Arguments&&... _arguments ) {
    const boxes::box_t l_targetRectangle(
        ( _position.x - _cameraBoxCoordinates.x ),
        ( _position.y - _cameraBoxCoordinates.y ), 0, 0 );

    _state.render( l_targetRectangle, _doDrawBoxes, _doFillBoxes,
                   std::forward< Arguments >( _arguments )... );
}

} // namespace

void object_t::render( const boxes::box_t& _cameraBoxCoordinates,
                       bool _doDrawBoxes,
                       bool _doFillBoxes ) const {
    _render( _position, _currentState(), _cameraBoxCoordinates, _doDrawBoxes,
             _doFillBoxes );
}

void object_t::render( const boxes::box_t& _cameraBoxCoordinates,
                       bool _doDrawBoxes,
                       bool _doFillBoxes,
                       double _angle,
                       SDL_FlipMode _flipMode ) const {
    _render( _position, _currentState(), _cameraBoxCoordinates, _doDrawBoxes,
             _doFillBoxes, _angle, _flipMode );
}

} // namespace object
