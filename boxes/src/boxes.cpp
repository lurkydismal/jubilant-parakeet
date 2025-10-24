#include "boxes.hpp"

#include <SDL3/SDL_render.h>

#include "slickdl.hpp"

namespace boxes {

void boxes_t::render( slickdl::renderer_t& _renderer,
                      const slickdl::box_t< float >& _screenSpaceTarget,
                      bool _doFill ) const {
    slickdl::color_t l_colorBefore;

    // Store current draw color
    {
        const bool l_result = SDL_GetRenderDrawColor(
            _renderer, &l_colorBefore.red, &l_colorBefore.green,
            &l_colorBefore.blue, &l_colorBefore.alpha );

        slickdl::assert( l_result );
    }

    const auto l_setRenderDrawColor =
        [ & ]( auto _function, const slickdl::color_t& _color ) -> void {
        const bool l_result = _function( _renderer, _color.red, _color.green,
                                         _color.blue, _color.alpha );

        slickdl::assert( l_result );
    };

    l_setRenderDrawColor( SDL_SetRenderDrawColor, _color );

    const auto l_currentFrame = currentFrame();

    for ( const auto& _box : l_currentFrame ) {
        const slickdl::box_t< float > l_targetRectangle = {
            ( _screenSpaceTarget.x + _box.x ),
            ( _screenSpaceTarget.y + _box.y ),
            _box.width,
            _box.height,
        };

        auto l_render = [ & ]( auto _renderFunction ) -> void {
            const SDL_FRect l_temp = l_targetRectangle;

            const bool l_result = _renderFunction( _renderer, &l_temp );

            slickdl::assert( l_result );
        };

        l_render( ( _doFill ) ? ( SDL_RenderFillRect ) : ( SDL_RenderRect ) );
    }

    l_setRenderDrawColor( SDL_SetRenderDrawColor, l_colorBefore );
}

} // namespace boxes
