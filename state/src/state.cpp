#include "state.hpp"

namespace state {

namespace {

template < typename... Arguments >
void _render( const animation::animation_t& _animation,
              const boxes::boxes_t& _boxes,
              const slickdl::renderer_t& _renderer,
              const boxes::box_t& _cameraBoxCoordinates,
              bool _doDrawBoxes,
              bool _doFillBoxes,
              Arguments&&... _arguments ) {
    boxes::box_t l_targetBox = _animation.currentTargetBox();

    l_targetBox.x += _cameraBoxCoordinates.x;
    l_targetBox.y += _cameraBoxCoordinates.y;

    _animation.render( _renderer, l_targetBox,
                       std::forward< Arguments >( _arguments )... );

    if ( _doDrawBoxes ) {
        _boxes.render( _renderer, l_targetBox, _doFillBoxes );
    }
}

} // namespace

void state_t::render( const boxes::box_t& _cameraBoxCoordinates,
                      bool _doDrawBoxes,
                      bool _doFillBoxes ) const {
    _render( _animation, _boxes, _renderer, _cameraBoxCoordinates, _doDrawBoxes,
             _doFillBoxes );
}

void state_t::render( const boxes::box_t& _cameraBoxCoordinates,
                      bool _doDrawBoxes,
                      bool _doFillBoxes,
                      double _angle,
                      SDL_FlipMode _flipMode ) const {
    _render( _animation, _boxes, _renderer, _cameraBoxCoordinates, _doDrawBoxes,
             _doFillBoxes, _angle, _flipMode );
}

} // namespace state
