#include "boxes.hpp"

namespace boxes {

void boxes_t::render( const slickdl::renderer_t& _renderer,
                      const box_t& _screenSpaceTarget,
                      bool _doFill ) const {
    color::color_t l_colorBefore;

    // Store current draw color
    {
        const bool l_result = SDL_GetRenderDrawColor(
            _renderer, &l_colorBefore.red, &l_colorBefore.green,
            &l_colorBefore.blue, &l_colorBefore.alpha );

        stdfunc::assert( l_result, "Getting renderer draw color: '{}'",
                         SDL_GetError() );
    }

    const auto l_setRenderDrawColor =
        [ & ]( auto _function, const color::color_t& _color ) -> void {
        const bool l_result = _function( _renderer, _color.red, _color.green,
                                         _color.blue, _color.alpha );

        stdfunc::assert( l_result, "Setting renderer draw color: '{}'",
                         SDL_GetError() );
    };

    l_setRenderDrawColor( SDL_SetRenderDrawColor, _color );

    const auto l_currentFrame = currentFrame();

    for ( const auto& _box : l_currentFrame ) {
        const SDL_FRect l_targetRectangle = {
            .x = ( _screenSpaceTarget.x + _box.x ),
            .y = ( _screenSpaceTarget.y + _box.y ),
            .w = _box.width,
            .h = _box.height,
        };

        auto l_render = [ & ]( auto _renderFunction ) -> void {
            const bool l_result =
                _renderFunction( _renderer, &l_targetRectangle );

            stdfunc::assert( l_result, "Render rectangle: '{}'",
                             SDL_GetError() );
        };

        l_render( ( _doFill ) ? ( SDL_RenderFillRect ) : ( SDL_RenderRect ) );
    }

    l_setRenderDrawColor( SDL_SetRenderDrawColor, l_colorBefore );
}

} // namespace boxes
