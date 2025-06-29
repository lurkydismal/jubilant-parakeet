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
            printf( "[reload] Detected change in %s\n", _soPath );

            void* handle =
                dlmopen( LM_ID_NEWLM, _soPath, RTLD_LAZY | RTLD_DEEPBIND );
            printf( "%s %p %s\n", _soPath, handle, dlerror() );

            Lmid_t nsid;
            if ( dlinfo( handle, RTLD_DI_LMID, &nsid ) != 0 ) {
                // error, usually invalid handle
                perror( "dlinfo" );
            } else {
                printf( "Namespace ID: %lu\n", ( unsigned long )nsid );
            }

            struct link_map* lm;
            dlinfo( handle, RTLD_DI_LINKMAP, &lm );

            void* newA = NULL;

            const char* names[] = {
                "snappy_max_compressed_length",
                "snappy_compress",
                "snappy_validate_compressed_buffer",
                "snappy_uncompressed_length",
                "snappy_uncompress",
                "ini_parse_string_length",
                "SDL_SetRenderVSync",
                "SDL_GetError",
                "SDL_GetRenderDrawColor",
                "SDL_SetRenderDrawColor",
                "SDL_RenderRect",
                "SDL_RenderFillRect",
                "SDL_GetError",
                "SDL_IOFromConstMem",
                "IMG_LoadTexture_IO",
                "SDL_CloseIO",
                "SDL_DestroyTexture",
                "SDL_RenderTexture",
                "SDL_GetError",
                "SDL_GetKeyboardState",
                "SDL_SetRenderScale",
                "SDL_GetError",
                "SDL_SetAppMetadata",
                "SDL_Init",
                "SDL_CreateWindowAndRenderer",
                "SDL_SetDefaultTextureScaleMode",
                "SDL_HasGamepad",
                "SDL_RenderClear",
                "SDL_RenderPresent",
                "SDL_DestroyRenderer",
                "SDL_DestroyWindow",
                "SDL_Quit",
            };

            for ( struct link_map* m = lm; m; m = m->l_next ) {
                if ( !( m->l_name ) || !( *( m->l_name ) ) ) {
                    continue;
                }

                void* existing =
                    dlmopen( nsid, m->l_name, RTLD_LAZY | RTLD_NOLOAD );

                plthook_t* ph;

                if ( plthook_open_by_handle( &ph, existing ) == 0 ) {
                    FOR( const char* const*, names ) {
                        void* old;
                        if ( plthook_replace( ph, *_element,
                                              dlsym( RTLD_DEFAULT, *_element ),
                                              &old ) == 0 ) {
                            void* p = dlsym( existing, "iterate" );

                            if ( p ) {
                                newA = p;
                            }

                            printf( "new    %p , %p\n", newA, old );
                        }
                    }

                    plthook_close( ph );
                }
            }

            void* main_handle = dlopen( NULL, RTLD_NOW );

            plthook_t* ph;

            if ( plthook_open_by_handle( &ph, main_handle ) != 0 ) {
                fprintf( stderr, "plthook_open: %s\n", plthook_error() );
            }

            void* old = NULL;
            if ( plthook_replace( ph, "iterate", newA, &old ) == 0 ) {
                printf( "newO    %p , %p\n", newA, old );
            }

            plthook_close( ph );
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
