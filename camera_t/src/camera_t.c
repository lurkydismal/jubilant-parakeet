#include "camera_t.h"

#include "log.h"
#include "stdfunc.h"

camera_t camera_t$create( void ) {
    camera_t l_returnValue = DEFAULT_CAMERA;

    return ( l_returnValue );
}

bool camera_t$destroy( camera_t* _camera ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
bool camera_t$update( camera_t* _camera, const player_t* _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
