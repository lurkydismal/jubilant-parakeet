#include "applicationState_t.h"

#include "log.h"
#include "stdfunc.h"

applicationState_t applicationState_t$create( void ) {
    applicationState_t l_returnValue = DEFAULT_APPLICATION_STATE;

    {
        l_returnValue.settings = settings_t$create();
        l_returnValue.config = config_t$create();
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

        l_returnValue = config_t$destroy( &( _applicationState->config ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        /*
         * Background is a reference to config background
         */

        l_returnValue = camera_t$destroy( &( _applicationState->camera ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = player_t$destroy( &( _applicationState->localPlayer ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        FOR_ARRAY( player_t* const*, _applicationState->remotePlayers ) {
            l_returnValue = player_t$destroy( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        FREE_ARRAY( _applicationState->remotePlayers );

        _applicationState->remotePlayers = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$load( applicationState_t* _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        l_returnValue = background_t$load( _applicationState->background,
                                           _applicationState->renderer );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Loading background\n" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$unload( applicationState_t* _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        l_returnValue = background_t$unload( _applicationState->background );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
