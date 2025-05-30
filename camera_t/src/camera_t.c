#include "camera_t.h"

#include "stdfunc.h"

camera_t camera_t$create( void ) {
    camera_t l_returnValue = DEFAULT_CAMERA;

    return ( l_returnValue );
}

bool camera_t$destroy( camera_t* _camera ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool camera_t$load( camera_t* _camera, const SDL_FRect* _targetRectangle ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_targetRectangle ) ) {
        goto EXIT;
    }

    {
        // Width
        {
            if ( UNLIKELY( !_targetRectangle->w ) ) {
                l_returnValue = false;

                goto EXIT;
            }

            _camera->rectangle.w = _targetRectangle->w;
        }

        // Height
        {
            if ( UNLIKELY( !_targetRectangle->h ) ) {
                l_returnValue = false;

                goto EXIT;
            }

            _camera->rectangle.w = _targetRectangle->h;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool camera_t$unload( camera_t* _camera ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
bool camera_t$update( camera_t* _camera, player_t* _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
