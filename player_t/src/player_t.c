#include "player_t.h"

#include "camera_t.h"
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

bool player_t$move( player_t* restrict _player, float _x, float _y ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( ( _x == 0 ) && ( _y == 0 ) ) ) {
        l_returnValue = true;

        goto EXIT;
    }

    {
        l_returnValue = object_t$move( &( _player->object ), _x, _y );

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
                      const camera_t* restrict _camera,
                      bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    {
        const SDL_FRect l_targetRectangle = {
            .x = ( _player->object.worldX - _camera->worldX ),
            .y = ( _player->object.worldY - _camera->worldY ),
            .w = 0,
            .h = 0 };

        l_returnValue = state_t$render(
            _player->object.states[ _player->object.currentState ],
            &l_targetRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
