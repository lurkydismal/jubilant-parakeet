#include "boxes.hpp"

#include "asset.hpp"

bool boxes_t$load$one( boxes_t* _boxes,
                       const SDL_FRect* _targetRectangle,
                       size_t _startIndex,
                       size_t _endIndex ) {}

bool boxes_t$render( const boxes_t* _boxes,
                     SDL_Renderer* _renderer,
                     const SDL_FRect* _targetRectangle,
                     bool _doFill ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_boxes ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        color_t l_colorBefore;

        l_returnValue = SDL_GetRenderDrawColor(
            _renderer, &l_colorBefore.red, &l_colorBefore.green,
            &l_colorBefore.blue, &l_colorBefore.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Getting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = SDL_SetRenderDrawColor(
            _renderer, _boxes->color.red, _boxes->color.green,
            _boxes->color.blue, _boxes->color.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Setting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        SDL_FRect** l_currentKeyFrames = boxes_t$currentKeyFrames$get( _boxes );

        FOR_ARRAY( SDL_FRect* const*, l_currentKeyFrames ) {
            const SDL_FRect l_targetRectangle = {
                .x = ( _targetRectangle->x + ( *_element )->x ),
                .y = ( _targetRectangle->y + ( *_element )->y ),
                .w = ( *_element )->w,
                .h = ( *_element )->h,
            };

            if ( _doFill ) {
                l_returnValue =
                    SDL_RenderFillRect( _renderer, &l_targetRectangle );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error, "Render filled rectangle: '%s'",
                        SDL_GetError() );

                    break;
                }

            } else {
                l_returnValue = SDL_RenderRect( _renderer, &l_targetRectangle );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Render rectangle: '%s'",
                                                  SDL_GetError() );

                    break;
                }
            }
        }

        FREE_ARRAY( l_currentKeyFrames );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = SDL_SetRenderDrawColor(
            _renderer, l_colorBefore.red, l_colorBefore.green,
            l_colorBefore.blue, l_colorBefore.alpha );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Setting renderer draw color: '%s'",
                                          SDL_GetError() );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
