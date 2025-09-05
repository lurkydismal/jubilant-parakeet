#include "camera_t.h"

#include "log.h"
#include "stdfunc.h"

auto camera_t$create( void ) -> camera_t {
    camera_t l_returnValue = DEFAULT_CAMERA;

    return ( l_returnValue );
}

auto camera_t$destroy( camera_t* restrict _camera ) -> bool {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
auto camera_t$update( camera_t* restrict _camera, const player_t* _player )
    -> bool {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
