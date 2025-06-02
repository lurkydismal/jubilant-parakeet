#include "player_t.h"

#include <SDL3/SDL_rect.h>

#include "stdfunc.h"

player_t player_t$create( void ) {
    player_t l_returnValue = DEFAULT_PLAYER;

    {
        l_returnValue.object = object_t$create();
    }

    return ( l_returnValue );
}

bool player_t$destroy( player_t* _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$destroy( &( _player->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// First file - boxes
// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// After - animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool player_t$state$add$fromFiles( player_t* restrict _player,
                                   SDL_Renderer* _renderer,
                                   char* const* restrict _files,
                                   bool _isActionable,
                                   bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_files ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$state$add$fromFiles(
            &( _player->object ), _renderer, _files, _isActionable, _canLoop );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$step( player_t* restrict _player,
                    float _velocityX,
                    float _velocityY ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    {
        l_returnValue =
            object_t$step( &( _player->object ), _velocityX, _velocityY );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$render( const player_t* restrict _player,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$render( &( _player->object ), _cameraRectangle,
                                         _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
