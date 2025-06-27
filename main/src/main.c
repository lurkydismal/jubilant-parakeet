#if defined( __SANITIZE_LEAK__ )

#include <sanitizer/lsan_interface.h>

#endif

#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <limits.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "event.h"
#include "init.h"
#include "iterate.h"
#include "quit.h"
#include "stdfunc.h"

int main( int _argumentCount, char** _argumentVector ) {
    bool l_returnValue = false;

    applicationState_t l_applicationState;

    l_returnValue =
        init( &l_applicationState, _argumentCount, _argumentVector );

    if ( UNLIKELY( !l_returnValue ) ) {
        goto EXIT;
    }

    while ( true ) {
        SDL_PumpEvents();

        event_t l_event;

        SDL_PollEvent( &l_event );

        l_returnValue = event( &l_applicationState, &l_event );

        if ( UNLIKELY( !l_returnValue ) ) {
            break;
        }

        l_returnValue = iterate( &l_applicationState );

        if ( UNLIKELY( !l_returnValue ) ) {
            break;
        }
    }

EXIT:
    quit( &l_applicationState, l_returnValue );

    SDL_Quit();

#if defined( __SANITIZE_LEAK__ )

    __lsan_do_leak_check();

#endif

    return ( !l_returnValue );
}
