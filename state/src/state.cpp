#include "state.hpp"

namespace state {

namespace {

template < typename... Arguments >
void render( const animation::animation_t& _animation,
             const boxes::boxes_t& _boxes,
             slickdl::renderer_t& _renderer,
             const slickdl::box_t< float >& _cameraBoxCoordinates,
             bool _doDrawBoxes,
             bool _doFillBoxes,
             Arguments&&... _arguments ) {
    slickdl::box_t< float > l_targetBox = _animation.currentTargetBox();

    l_targetBox.x += _cameraBoxCoordinates.x;
    l_targetBox.y += _cameraBoxCoordinates.y;

    _animation.render( _renderer, l_targetBox,
                       std::forward< Arguments >( _arguments )... );

    if ( _doDrawBoxes ) {
        _boxes.render( _renderer, l_targetBox, _doFillBoxes );
    }
}

} // namespace

void state_t::render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                      bool _doDrawBoxes,
                      bool _doFillBoxes ) {
    ::state::render( _animation, _boxes, _renderer, _cameraBoxCoordinates,
                     _doDrawBoxes, _doFillBoxes );
}

void state_t::render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                      bool _doDrawBoxes,
                      bool _doFillBoxes,
                      double _angle,
                      SDL_FlipMode _flipMode ) {
    ::state::render( _animation, _boxes, _renderer, _cameraBoxCoordinates,
                     _doDrawBoxes, _doFillBoxes, _angle, _flipMode );
}

} // namespace state
