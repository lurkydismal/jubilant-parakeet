#include "applicationState_t.h"

#include "stdfunc.h"

applicationState_t applicationState_t$create( void ) {
    applicationState_t l_returnValue = DEFAULT_APPLICATION_STATE;

    {
        l_returnValue.settings = settings_t$create();
        l_returnValue.background = object_t$create();
        l_returnValue.camera = camera_t$create();
        l_returnValue.localPlayer = player_t$create();
        l_returnValue.remotePlayers = createArray( player_t* );
    }

    return ( l_returnValue );
}

bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        l_returnValue = settings_t$destroy( &( _applicationState->settings ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = object_t$destroy( &( _applicationState->background ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = camera_t$destroy( &( _applicationState->camera ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = camera_t$destroy( &( _applicationState->localPlayer ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        FOR_ARRAY( player_t* const*, &( _applicationState->remotePlayers ) ) {
            l_returnValue = camera_t$destroy( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        FREE_ARRAY( &(_applicationState->remotePlayers ) );

        _applicationState->remotePlayers = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
