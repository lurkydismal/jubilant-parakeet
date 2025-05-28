#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "applicationState_t.h"
#include "camera_t.h"
#include "log.h"
#include "stdfunc.h"

SDL_AppResult SDL_AppIterate( void* _applicationState ) {
    SDL_AppResult l_returnValue = SDL_APP_FAILURE;

    applicationState_t* l_applicationState =
        ( applicationState_t* )_applicationState;

    static bool x = false;
    static camera_t cam;
    static object_t p1;
    static object_t p2;
    static object_t p3;
    static object_t p4;

    if ( !x ) {
        x = true;

        cam = camera_t$create();

        char** files = createArray( char* );

        insertIntoArray( &files, "test.boxes" );
        insertIntoArray( &files, "test_100x100_1-2.png" );

        bool ret = object_t$state$add$fromFiles( &( cam.object ),
                                                 l_applicationState->renderer,
                                                 files, false, false );

        FREE_ARRAY( files );

        if ( !ret ) {
            log$transaction$query( ( logLevel_t )error, "1\n" );
        }

        p1 = object_t$create();
        p2 = object_t$create();
        p3 = object_t$create();
        p4 = object_t$create();
    }

    if ( UNLIKELY( !vsync$begin() ) ) {
        goto EXIT;
    }

    // Render
    {
        SDL_RenderClear( l_applicationState->renderer );

        SDL_RenderPresent( l_applicationState->renderer );
    }

    if ( UNLIKELY( !vsync$end() ) ) {
        goto EXIT;
    }

    ( l_applicationState->totalFramesRendered )++;

    l_returnValue = SDL_APP_CONTINUE;

EXIT:
    return ( l_returnValue );
}
