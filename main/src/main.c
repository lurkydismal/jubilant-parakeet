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
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "applicationState_t.h"
#include "event.h"
#include "init.h"
#include "iterate.h"
#include "plthook.h"
#include "quit.h"
#include "stdfunc.h"

#define HOT_RELOAD_ROOT_SHARED_OBJECT_FILE_NAME "root"

#if defined( HOT_RELOAD )

static bool check( const char* _soPath ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_soPath ) ) {
        goto EXIT;
    }

    {
        static long l_lastModificationTime = 0;

        struct stat st;

        if ( stat( _soPath, &st ) != 0 ) {
            fprintf( stderr, "[reload] stat(%s) failed: %s\n", _soPath,
                     strerror( errno ) );

            goto EXIT;
        }

        if ( st.st_mtime <= l_lastModificationTime ) {
            goto EXIT;
        }

        l_lastModificationTime = st.st_mtime;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static bool reload( const char* _soPath ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_soPath ) ) {
        goto EXIT;
    }

    {
        if ( check( _soPath ) ) {
            static void* baseHandle = NULL;

            if ( UNLIKELY( !baseHandle ) ) {
                baseHandle = dlmopen( LM_ID_BASE, NULL, RTLD_NOW );
            }

            printf( "[reload] Detected change in %s\n", _soPath );

            void* handle =
                dlmopen( LM_ID_NEWLM, _soPath, RTLD_LAZY | RTLD_DEEPBIND );
            printf( "%s %p %s\n", _soPath, handle, dlerror() );

#if 0
            void* new_iter = dlsym( handle, "iterate" );
            printf( "new@%p old@%p\n", new_iter,
                    dlsym( baseHandle, "iterate" ) );
#endif
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif

int main( int _argumentCount, char** _argumentVector ) {
    bool l_returnValue = false;

#if defined( HOT_RELOAD )

    char* l_rootSharedObjectPath =
        duplicateString( HOT_RELOAD_ROOT_SHARED_OBJECT_FILE_NAME ".so" );

    {
        char* l_directoryPath = getApplicationDirectoryAbsolutePath();

        concatBeforeAndAfterString( &l_rootSharedObjectPath, l_directoryPath,
                                    NULL );

        free( l_directoryPath );
    }

    reload( l_rootSharedObjectPath );

#endif

    applicationState_t l_applicationState;

    l_returnValue =
        init( &l_applicationState, _argumentCount, _argumentVector );

    if ( UNLIKELY( !l_returnValue ) ) {
        goto EXIT;
    }

    {
#if defined( HOT_RELOAD )

        size_t l_iterationCount = 0;

#endif

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

#if defined( HOT_RELOAD )

            l_iterationCount++;

            if ( ( l_iterationCount % 60 ) == 0 ) {
                reload( l_rootSharedObjectPath );
            }
#endif
        }
    }

EXIT:
    quit( &l_applicationState, l_returnValue );

#if defined( HOT_RELOAD )

    free( l_rootSharedObjectPath );

#endif

#if defined( __SANITIZE_LEAK__ )

    __lsan_do_leak_check();

#endif

    return ( !l_returnValue );
}
