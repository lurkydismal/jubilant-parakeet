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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        l_returnValue = settings_t$destroy( &( _applicationState->settings ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying settings\n" );

            goto EXIT;
        }

        l_returnValue = config_t$destroy( &( _applicationState->config ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying config\n" );

            goto EXIT;
        }

        /*
         * HUD is a reference to config HUD
         * Background is a reference to config background
         */

        l_returnValue = camera_t$destroy( &( _applicationState->camera ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying camera\n" );

            goto EXIT;
        }

        l_returnValue = player_t$destroy( &( _applicationState->localPlayer ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying player\n" );

            goto EXIT;
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
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
        // Background
        {
            l_returnValue = background_t$load( _applicationState->background,
                                               _applicationState->renderer );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading background\n" );

                goto EXIT;
            }
        }

        // HUD
        {
            l_returnValue = HUD_t$load(
                _applicationState->HUD, _applicationState->renderer,
                ( arrayLength( _applicationState->remotePlayers ) + 1 ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Loading HUD\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$unload( applicationState_t* _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument\n" );

        goto EXIT;
    }

    {
#define TRY_UNLOAD_OR_EXIT( _field )                                    \
    do {                                                                \
        l_returnValue = _field##_t$unload( _applicationState->_field ); \
        if ( UNLIKELY( !l_returnValue ) ) {                             \
            log$transaction$query( ( logLevel_t )error,                 \
                                   "Unloading " #_field "\n" );         \
            goto EXIT;                                                  \
        }                                                               \
    } while ( 0 )

        TRY_UNLOAD_OR_EXIT( background );
        TRY_UNLOAD_OR_EXIT( HUD );

#undef TRY_UNLOAD_OR_EXIT

        FOR_ARRAY( player_t* const*, _applicationState->remotePlayers ) {
            l_returnValue = player_t$destroy( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying player\n" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
