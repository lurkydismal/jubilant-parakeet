#include "event.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>

#include <cstddef>
#include <ranges>
#include <span>

#include "input.hpp"
#include "log.hpp"
#include "runtime.hpp"
#include "stddebug.hpp"

namespace runtime {

namespace {

auto onWindowResize( applicationState_t& _applicationState,
                     float _width,
                     float _height ) -> bool {
    bool l_returnValue = false;

    do {
        stdfunc::assert( ( _width && _height ),
                         "Invalid window width {} or height {}", _width,
                         _height );

        static size_t l_lastResizeFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState.renderContext.totalFramesRendered;

        if ( l_lastResizeFrame < l_totalFramesRendered ) [[likely]] {
            const float l_logicalWidth =
                _applicationState.renderContext.logicalWidth;
            const float l_logicalHeigth =
                _applicationState.renderContext.logicalHeight;

            const float l_scaleX = ( _width / l_logicalWidth );
            const float l_scaleY = ( _height / l_logicalHeigth );

            if ( !SDL_SetRenderScale( _applicationState.renderContext.renderer,
                                      l_scaleX, l_scaleY ) ) [[unlikely]] {
                logg::error( "Setting render scale: '{}'", SDL_GetError() );

                break;
            }
        }

        l_lastResizeFrame = l_totalFramesRendered;

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

auto handleKeyboardState( applicationState_t& _applicationState ) -> bool {
    bool l_returnValue = false;

    do {
        static size_t l_lastInputFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState.renderContext.totalFramesRendered;

        if ( l_lastInputFrame < l_totalFramesRendered ) [[likely]] {
            input::input_t l_input;

            {
                int l_keysAmount = 0;
                const bool* l_keysState = SDL_GetKeyboardState( &l_keysAmount );

                [[assume( l_keysAmount == SDL_SCANCODE_COUNT )]];

                for ( auto [ _index, _isPressed ] :
                      std::span( l_keysState, l_keysAmount ) |
                          std::views::enumerate ) {
                    // Unlikely because of all keys not much is pressed at once
                    if ( _isPressed ) [[unlikely]] {
                        [[maybe_unused]] auto l_scancode =
                            static_cast< SDL_Scancode >( _index );

                        // TODO: Implement
#if 0
                    const control::control_t& l_control =
                        _applicationState.settings.controls.get( l_scancode );

                    if ( l_control.scancode != SDL_SCANCODE_UNKNOWN ) {
                        l_input.direction |= l_control.input.direction;
                        l_input.button |= l_control.input.button;
                    }
#endif
                    }
                }
            }

            _applicationState.currentInput = l_input;
        }

        l_lastInputFrame = l_totalFramesRendered;
    } while ( false );

    return ( l_returnValue );
}

} // namespace

auto event( applicationState_t& _applicationState, const event_t& _event )
    -> bool {
    bool l_returnValue = false;

    do {
        const event_t l_emptyEVent{};

        // Empty means last event on current frame
        const bool l_isEventEmpty =
            ( __builtin_memcmp( &_event, &l_emptyEVent, sizeof( _event ) ) ==
              0 );

        if ( l_isEventEmpty ) [[unlikely]] {
            if ( !handleKeyboardState( _applicationState ) ) {
                break;
            }

        } else {
            auto l_handleEvent = [ & ] -> bool {
                bool l_returnValue = false;

                switch ( _event.type ) {
                    case SDL_EVENT_QUIT:
                        [[unlikely]] {
                            _applicationState.status = true;

                            l_returnValue = false;

                            break;
                        }

                    case SDL_EVENT_WINDOW_RESIZED: {
                        const float l_newWidth = _event.window.data1;
                        const float l_newHeight = _event.window.data2;

                        l_returnValue = onWindowResize(
                            _applicationState, l_newWidth, l_newHeight );

                        break;
                    }

                    default: {
                    }
                }

                return ( l_returnValue );
            };

            if ( !l_handleEvent() ) [[unlikely]] {
                break;
            }
        }

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

} // namespace runtime
