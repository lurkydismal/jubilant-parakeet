#include "camera_t.h"

#include "stdfunc.h"

camera_t camera_t$create( void ) {
    camera_t l_returnValue = DEFAULT_CAMERA;

    {
        l_returnValue.object = object_t$create();
    }

    return ( l_returnValue );
}

bool camera_t$destroy( camera_t* _camera ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_camera ) ) {
        goto EXIT;
    }

    {
        l_returnValue = object_t$destroy( &( _camera->object ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
