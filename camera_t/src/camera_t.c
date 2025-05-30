#include "camera_t.h"

#include <float.h>

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
        _camera->worldX = 0;
        _camera->worldY = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool camera_t$update( camera_t* _camera, player_t* _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        goto EXIT;
    }

    {
        float min_x = FLT_MAX, min_y = FLT_MAX;
        float max_x = -FLT_MAX, max_y = -FLT_MAX;

        float px = _player->object.worldX;
        float py = _player->object.worldY;

        min_x = __builtin_fminf( min_x, px );
        max_x = __builtin_fmaxf( max_x, px );
        min_y = __builtin_fminf( min_y, py );
        max_y = __builtin_fmaxf( max_y, py );

        _camera->worldX = ( min_x + max_x ) * 0.5f;
        _camera->worldY = ( min_y + max_y ) * 0.5f;

        float margin = 100.0f; // pixels
        float span_x = ( max_x - min_x ) + margin;
        float span_y = ( max_y - min_y ) + margin;

        float zoom_x = _camera->width / span_x;
        float zoom_y = _camera->height / span_y;

        float target_zoom = __builtin_fminf( zoom_x, zoom_y );
        target_zoom =
            __builtin_fminf( _camera->zoomMax,
                             __builtin_fmaxf( _camera->zoomMin, target_zoom ) );

        // Smooth zooming
        _camera->zoom += ( target_zoom - _camera->zoom ) * 0.1f;

        float half_width = ( _camera->width / _camera->zoom ) * 0.5f;
        float half_height = ( _camera->height / _camera->zoom ) * 0.5f;

        float left = half_width;
        float right = _camera->logicalWidth - half_width;
        float top = half_height;
        float bottom = _camera->logicalHeight - half_height;

        _camera->worldX =
            __builtin_fminf( __builtin_fmaxf( _camera->worldX, left ), right );
        _camera->worldY =
            __builtin_fminf( __builtin_fmaxf( _camera->worldY, top ), bottom );

        float cam_left =
            _camera->worldX - _camera->width * 0.5f / _camera->zoom;
        float cam_top =
            _camera->worldY - _camera->height * 0.5f / _camera->zoom;

        // Set your orthographic projection using cam_left, cam_top and zoom
        float sx = ( px - cam_left ) * _camera->zoom;
        float sy = ( py - cam_top ) * _camera->zoom;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
