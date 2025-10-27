#include "boxes.hpp"

namespace boxes {

void boxes_t::render( slickdl::renderer_t& _renderer,
                      const slickdl::box_t< float >& _screenSpaceTarget,
                      bool _doFill ) const {
    slickdl::color_t l_colorBefore = _renderer.color();

    _renderer.color( _color );

    const auto l_currentFrame = currentFrame();

    for ( const auto& _box : l_currentFrame ) {
        _renderer.box(
            slickdl::box_t< float >{
                ( _screenSpaceTarget.x + _box.x ),
                ( _screenSpaceTarget.y + _box.y ),
                _box.width,
                _box.height,
            },
            _doFill );
    }

    _renderer.color( l_colorBefore );
}

} // namespace boxes
