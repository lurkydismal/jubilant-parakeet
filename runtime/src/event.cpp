#include "event.hpp"

#include <cstddef>
#include <ranges>

#include "input.hpp"
#include "runtime.hpp"
#include "slickdl/keyboard.hpp"
#include "slickdl/scancode.hpp"
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
            _applicationState.renderContext.window.totalFramesRendered;

        if ( l_lastResizeFrame < l_totalFramesRendered ) [[likely]] {
            const float l_logicalWidth =
                _applicationState.renderContext.logicalWidth;
            const float l_logicalHeigth =
                _applicationState.renderContext.logicalHeight;

            const float l_scaleX = ( _width / l_logicalWidth );
            const float l_scaleY = ( _height / l_logicalHeigth );

            _applicationState.renderContext.renderer.scale( l_scaleX,
                                                            l_scaleY );
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
            _applicationState.renderContext.window.totalFramesRendered;

        if ( l_lastInputFrame < l_totalFramesRendered ) [[likely]] {
            input::input_t l_input;

            {
                const std::array l_keysState = slickdl::keyboard::state();

                for ( auto [ _index, _isPressed ] :
                      l_keysState | std::views::enumerate ) {
                    // Unlikely because of all keys not much is pressed at once
                    if ( _isPressed ) [[unlikely]] {
                        [[maybe_unused]] auto l_scancode =
                            static_cast< slickdl::scancode_t >( _index );

                        // TODO: Implement
#if 0
                    const control::control_t& l_control =
                        _applicationState.settings.controls.get( l_scancode );

                    if ( l_control.scancode != slickdl::scancode_t::unknown ) {
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
