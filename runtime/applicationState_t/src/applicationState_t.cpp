#include "applicationState_t.h"

#include "log.hpp"
#include "stdfunc.hpp"

bool applicationState_t$load( applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if 0
        {
            insertIntoArray( &( _applicationState->HUD->players ),
                             &( _applicationState->localPlayer ) );

            // TODO
            player_t newP = player_t$create();
            *( newP.isGuardBroken ) = true;

            insertIntoArray( &( _applicationState->remotePlayers ),
                             clone( &newP ) );

            FOR_ARRAY( player_t* const*, _applicationState->remotePlayers ) {
                insertIntoArray( &( _applicationState->HUD->players ),
                                 *_element );
            }

            _applicationState->HUD->logicalWidth =
                _applicationState->logicalWidth;
            _applicationState->HUD->logicalHeight =
                _applicationState->logicalHeight;
        }
#endif

#define TRY_LOAD_OR_EXIT( _field )                                            \
    do {                                                                      \
        l_returnValue = _field##_t$load( _applicationState->_field,           \
                                         _applicationState->renderer );       \
        if ( UNLIKELY( !l_returnValue ) ) {                                   \
            log$transaction$query( ( logLevel_t )error, "Loading " #_field ); \
            goto EXIT;                                                        \
        }                                                                     \
    } while ( 0 )

#if 0
        TRY_LOAD_OR_EXIT( background );
#endif
        // TRY_LOAD_OR_EXIT( HUD );
        // TRY_LOAD_OR_EXIT( character );

#undef TRY_LOAD_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$unload(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define TRY_UNLOAD_OR_EXIT( _field )                                         \
    do {                                                                     \
        l_returnValue = _field##_t$unload( &( _applicationState->_field ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                                  \
            log$transaction$query( ( logLevel_t )error,                      \
                                   "Unloading " #_field );                   \
            goto EXIT;                                                       \
        }                                                                    \
    } while ( 0 )

        TRY_UNLOAD_OR_EXIT( settings );
        TRY_UNLOAD_OR_EXIT( config );

#undef TRY_UNLOAD_OR_EXIT

#if 0
        l_returnValue = player_t$unload( &( _applicationState->localPlayer ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Unloading local player" );

            goto EXIT;
        }

        FOR_ARRAY( player_t* const*, _applicationState->remotePlayers ) {
            l_returnValue = player_t$unload( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Unloading remote player" );

                goto EXIT;
            }

            l_returnValue = player_t$destroy( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying remote player" );

                goto EXIT;
            }
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
