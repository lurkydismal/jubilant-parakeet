#include "object.hpp"

namespace object {

namespace {

template < typename... Arguments >
void render( const slickdl::point_t< float >& _position,
             state::state_t& _state,
             const slickdl::box_t< float >& _cameraBoxCoordinates,
             bool _doDrawBoxes,
             bool _doFillBoxes,
             Arguments&&... _arguments ) {
    const slickdl::box_t< float > l_targetRectangle(
        ( _position.x - _cameraBoxCoordinates.x ),
        ( _position.y - _cameraBoxCoordinates.y ), 0, 0 );

    _state.render( l_targetRectangle, _doDrawBoxes, _doFillBoxes,
                   std::forward< Arguments >( _arguments )... );
}

} // namespace

void object_t::render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                       bool _doDrawBoxes,
                       bool _doFillBoxes ) {
    ::object::render( _position, _currentState(), _cameraBoxCoordinates,
                      _doDrawBoxes, _doFillBoxes );
}

void object_t::render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                       bool _doDrawBoxes,
                       bool _doFillBoxes,
                       double _angle,
                       SDL_FlipMode _flipMode ) {
    ::object::render( _position, _currentState(), _cameraBoxCoordinates,
                      _doDrawBoxes, _doFillBoxes, _angle, _flipMode );
}

} // namespace object
