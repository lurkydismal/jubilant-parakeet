#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "FPS.h"
#include "applicationState_t.h"
#include "log.h"
#include "player_t.h"
#include "stdfunc.h"

static FORCE_INLINE bool iterate(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        goto EXIT;
    }

    {
        static bool x = false;
        static object_t bg;
        static player_t p1;
        static player_t p2;
        static player_t p3;
        static player_t p4;

        if ( !x ) {
            x = true;

            {
                bg = object_t$create();

                char** files = createArray( char* );

                insertIntoArray( &files, "test.boxes" );
                insertIntoArray( &files, "test_1280x720_1-2.png" );

                bool ret = object_t$state$add$fromFiles(
                    &bg, _applicationState->renderer, files, false, false );

                FREE_ARRAY( files );

                if ( !ret ) {
                    goto EXIT;
                }
            }

            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test1.boxes" );
                insertIntoArray( &files, "test_100x100_1-2.png" );

                p1 = player_t$create();

                bool ret = player_t$state$add$fromFiles(
                    &( p1 ), _applicationState->renderer, files, false, false );

                p1.object.worldXMax = ( _applicationState->logicalWidth - 100 );
                p1.object.worldYMax =
                    ( _applicationState->logicalHeight - 100 );

                FREE_ARRAY( files );

                if ( !ret ) {
                    goto EXIT;
                }
            }

            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test2.boxes" );
                insertIntoArray( &files, "test_1280x720_1-2.png" );

                p2 = player_t$create();

                FREE_ARRAY( files );

                bool ret = true;

                if ( !ret ) {
                    goto EXIT;
                }
            }

            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test3.boxes" );
                insertIntoArray( &files, "test_1280x720_1-2.png" );

                p3 = player_t$create();

                FREE_ARRAY( files );

                bool ret = true;

                if ( !ret ) {
                    goto EXIT;
                }
            }

            {
                char** files = createArray( char* );

                insertIntoArray( &files, "test4.boxes" );
                insertIntoArray( &files, "test_1280x720_1-2.png" );

                p4 = player_t$create();

                FREE_ARRAY( files );

                bool ret = true;

                if ( !ret ) {
                    goto EXIT;
                }
            }

            _applicationState->camera.logicalWidth =
                _applicationState->logicalWidth;
            _applicationState->camera.logicalHeight =
                _applicationState->logicalHeight;
        }

        if ( UNLIKELY( !vsync$begin() ) ) {
            l_returnValue = false;

            log$transaction$query( ( logLevel_t )error, "Vsync begin\n" );

            goto EXIT;
        }

        // Render
        {
            SDL_RenderClear( _applicationState->renderer );

            if ( x ) {
                bool ret = object_t$render( &bg, &( _applicationState->camera ),
                                            true );

                if ( !ret ) {
                    goto EXIT;
                }

                ret = player_t$render( &p1, &( _applicationState->camera ),
                                       true );

                if ( !ret ) {
                    goto EXIT;
                }

                ret = player_t$move( &p1, 3, 3 );

                if ( !ret ) {
                    goto EXIT;
                }

                ret = camera_t$update( &( _applicationState->camera ), &p1 );

                if ( !ret ) {
                    goto EXIT;
                }
            }

            SDL_RenderPresent( _applicationState->renderer );
        }

        if ( UNLIKELY( !vsync$end() ) ) {
            l_returnValue = false;

            log$transaction$query( ( logLevel_t )error, "Vsync end\n" );

            goto EXIT;
        }

        ( _applicationState->totalFramesRendered )++;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

SDL_AppResult SDL_AppIterate( void* _applicationState ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    if ( UNLIKELY( !iterate( l_applicationState ) ) ) {
        goto EXIT;
    }

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
