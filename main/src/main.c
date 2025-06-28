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

#if 0
// --- Plugin definition ---

typedef struct {
    const char* path;  // original path as given (may be relative)
    char* canon_path;  // canonical absolute path via realpath
    time_t last_mtime; // last seen modification time
} plugin_t;

typedef struct {
    char* name;
    void* addr;
} export_symbol_t;

// --- Patch context for a single plugin reload ---

typedef struct {
    const char* plugin_path; // canonical path to match against info->dlpi_name
    const char** names;      // array of symbol names
    void** addrs;            // array of new addresses
    size_t count;
} patch_ctx_t;

// Collect all dynamic-exported functions from so_path on disk, resolve their
// addresses via dlsym(new_handle). Returns an allocated array of
// export_symbol_t (caller must free); sets *out_count. On error or no exports,
// returns NULL and *out_count=0.
static export_symbol_t* collect_exports( const char* so_path,
                                         void* new_handle,
                                         size_t* out_count ) {
    *out_count = 0;
    if ( !new_handle ) {
        fprintf( stderr, "collect_exports: new_handle is NULL\n" );
        return NULL;
    }
    if ( elf_version( EV_CURRENT ) == EV_NONE ) {
        fprintf( stderr, "collect_exports: libelf initialization failed\n" );
        return NULL;
    }
    int fd = open( so_path, O_RDONLY );
    if ( fd < 0 ) {
        perror( "collect_exports: open .so" );
        return NULL;
    }
    Elf* e = elf_begin( fd, ELF_C_READ, NULL );
    if ( !e ) {
        fprintf( stderr, "collect_exports: elf_begin failed: %s\n",
                 elf_errmsg( -1 ) );
        close( fd );
        return NULL;
    }
    size_t shstrndx;
    if ( elf_getshdrstrndx( e, &shstrndx ) != 0 ) {
        fprintf( stderr, "collect_exports: elf_getshdrstrndx failed: %s\n",
                 elf_errmsg( -1 ) );
        elf_end( e );
        close( fd );
        return NULL;
    }

    // First pass: find .dynsym section and count suitable symbols
    Elf_Scn* scn = NULL;
    Elf64_Shdr shdr;
    Elf_Data* data = NULL;
    size_t total = 0;
    while ( ( scn = elf_nextscn( e, scn ) ) != NULL ) {
        if ( gelf_getshdr( scn, &shdr ) != &shdr )
            continue;
        if ( shdr.sh_type != SHT_DYNSYM )
            continue;
        data = elf_getdata( scn, NULL );
        if ( !data )
            continue;
        size_t symcount = shdr.sh_size / shdr.sh_entsize;
        for ( size_t i = 0; i < symcount; i++ ) {
            GElf_Sym sym;
            if ( !gelf_getsym( data, i, &sym ) )
                continue;
            if ( GELF_ST_TYPE( sym.st_info ) == STT_FUNC &&
                 sym.st_shndx != SHN_UNDEF && sym.st_name != 0 ) {
                const char* name = elf_strptr( e, shdr.sh_link, sym.st_name );
                if ( name && *name ) {
                    total++;
                }
            }
        }
        break; // assume only one .dynsym
    }
    if ( total == 0 ) {
        elf_end( e );
        close( fd );
        return NULL;
    }

    // Allocate array
    export_symbol_t* arr = calloc( total, sizeof( export_symbol_t ) );
    if ( !arr ) {
        perror( "collect_exports: calloc" );
        elf_end( e );
        close( fd );
        return NULL;
    }

    // Second pass: fill names and resolve addresses
    // Reset to beginning
    elf_end( e );
    lseek( fd, 0, SEEK_SET );
    e = elf_begin( fd, ELF_C_READ, NULL );
    if ( !e ) {
        fprintf( stderr, "collect_exports: elf_begin (2) failed: %s\n",
                 elf_errmsg( -1 ) );
        close( fd );
        free( arr );
        return NULL;
    }
    if ( elf_getshdrstrndx( e, &shstrndx ) != 0 ) {
        elf_end( e );
        close( fd );
        free( arr );
        return NULL;
    }
    scn = NULL;
    size_t idx = 0;
    while ( ( scn = elf_nextscn( e, scn ) ) != NULL ) {
        if ( gelf_getshdr( scn, &shdr ) != &shdr )
            continue;
        if ( shdr.sh_type != SHT_DYNSYM )
            continue;
        data = elf_getdata( scn, NULL );
        if ( !data )
            continue;
        size_t symcount = shdr.sh_size / shdr.sh_entsize;
        for ( size_t i = 0; i < symcount; i++ ) {
            GElf_Sym sym;
            if ( !gelf_getsym( data, i, &sym ) )
                continue;
            if ( GELF_ST_TYPE( sym.st_info ) == STT_FUNC &&
                 sym.st_shndx != SHN_UNDEF && sym.st_name != 0 ) {
                const char* name = elf_strptr( e, shdr.sh_link, sym.st_name );
                if ( name && *name ) {
                    // resolve via dlsym
                    dlerror();
                    void* addr = dlsym( new_handle, name );
                    const char* err = dlerror();
                    if ( addr && !err ) {
                        arr[ idx ].name = strdup( name );
                        arr[ idx ].addr = addr;
                        idx++;
                    } else {
                        // skip if not found
                    }
                }
            }
        }
        break;
    }
    elf_end( e );
    close( fd );
    *out_count = idx;
    if ( idx == 0 ) {
        // no symbols resolved
        free( arr );
        return NULL;
    }
    return arr;
}

static inline void patch_got_entry( void** got_addr, void* new_func ) {
    *got_addr = new_func;
}

// Callback for dl_iterate_phdr: only patch if info->dlpi_name equals
// plugin_path
static int patch_phdr_callback( struct dl_phdr_info* info,
                                size_t size,
                                void* data ) {
    patch_ctx_t* ctx = ( patch_ctx_t* )data;
    const char* obj_path = info->dlpi_name;

    // Only patch if this module’s path matches the plugin_path exactly.
    // Skip everything else.
    if ( !obj_path ) {
        return 0;
    }
    // Compare strings: info->dlpi_name is typically absolute path.
    if ( strcmp( obj_path, ctx->plugin_path ) != 0 ) {
        return 0;
    }

    // Now obj_path == plugin_path: open its PLT/GOT for patching
    plthook_t* plthook = NULL;
    int open_ret = plthook_open( &plthook, obj_path );
    if ( open_ret != 0 ) {
        // Could not open PLT for this object; skip
        fprintf( stderr, "[patch] plthook_open failed for %s: %s\n", obj_path,
                 plthook_error() );
        return 0;
    }

    unsigned int idx = 0;
    const char* symname;
    void** got_addr;
    while ( plthook_enum( plthook, &idx, &symname, &got_addr ) == 0 ) {
        for ( size_t j = 0; j < ctx->count; j++ ) {
            if ( strcmp( symname, ctx->names[ j ] ) == 0 ) {
                void* newAddr = ctx->addrs[ j ];

                printf( "[before patch] %s in %s -> %p [ %p ]\n", symname,
                        obj_path, newAddr, *got_addr );

                patch_got_entry( got_addr, newAddr );

                printf( "[after patch] %s in %s -> %p [ %p ]\n", symname,
                        obj_path, newAddr, *got_addr );
                break;
            }
        }
    }
    plthook_close( plthook );
    return 0;
}

// Reload one plugin if its file changed: dlopen, dlsym each symbol, then patch
// only that .so Returns 1 if reloaded & patched, 0 if not changed, -1 on error.
static int reload_plugin_if_needed( plugin_t* plug ) {
    struct stat st;
    if ( stat( plug->canon_path, &st ) != 0 ) {
        fprintf( stderr, "[reload] stat(%s) failed: %s\n", plug->canon_path,
                 strerror( errno ) );
        return -1;
    }
    if ( st.st_mtime <= plug->last_mtime ) {
        // No change
        return 0;
    }
    printf( "[reload] Detected change in %s (old mtime=%ld, new mtime=%ld)\n",
            plug->canon_path, ( long )plug->last_mtime, ( long )st.st_mtime );

    // dlopen (or dlmopen) the plugin .so
    void* handle = NULL;
    dlerror();
    handle = dlmopen( LM_ID_NEWLM, plug->canon_path, RTLD_NOW | RTLD_LOCAL );
    if ( !handle ) {
        fprintf( stderr,
                 "[reload] dlmopen(%s) failed: %s; falling back to dlopen\n",
                 plug->canon_path, dlerror() );
        dlerror();
        handle = dlopen( plug->canon_path, RTLD_NOW | RTLD_LOCAL );
    }
    if ( !handle ) {
        fprintf( stderr, "[reload] dlopen(%s) failed: %s\n", plug->canon_path,
                 dlerror() );
        return -1;
    }

    // Resolve each symbol via dlsym
#if 0
    {
        void **addrs = malloc(sizeof(void*) * plug->symbol_count);
        if (!addrs) {
            perror("malloc");
            return -1;
        }
        for (size_t i = 0; i < plug->symbol_count; i++) {
            const char *name = plug->symbols[i];
            dlerror();
            void *addr = dlsym(handle, name);
            const char *err = dlerror();
            if (!addr || err) {
                fprintf(stderr, "[reload] dlsym(%s) failed: %s; skipping\n",
                        name, err?err:"<no error>");
                addrs[i] = NULL;
            } else {
                addrs[i] = addr;
                printf("[reload] dlsym(%s) -> %p\n", name, addr);
            }
        }

        // Build patch context
        patch_ctx_t ctx = {
            .plugin_path = plug->canon_path,
            .names = plug->symbols,
            .addrs = addrs,
            .count = plug->symbol_count
        };
        // Patch only that plugin’s PLT
        dl_iterate_phdr(patch_phdr_callback, &ctx);

        plug->last_mtime = st.st_mtime;
        free(addrs);
    }
#endif

    {
        // After dlopen/dlmopen succeeded, collect all exported functions
        // dynamically:
        size_t n_exports = 0;
        export_symbol_t* exports =
            collect_exports( plug->canon_path, handle, &n_exports );
        if ( !exports || n_exports == 0 ) {
            fprintf(
                stderr,
                "[reload] No exported functions found or none resolved in %s\n",
                plug->canon_path );
            // Still update last_mtime so we don't keep retrying immediately
            plug->last_mtime = st.st_mtime;
            return 0; // or return 1? up to you: treat as reloaded but no
                      // exports
        }
        // Build arrays for patch context
        const char** names = malloc( sizeof( char* ) * n_exports );
        void** addrs = malloc( sizeof( void* ) * n_exports );
        if ( !names || !addrs ) {
            perror( "malloc" );
            // free exports
            for ( size_t i = 0; i < n_exports; i++ )
                free( exports[ i ].name );
            free( exports );
            free( names );
            free( addrs );
            return -1;
        }
        for ( size_t i = 0; i < n_exports; i++ ) {
            names[ i ] = exports[ i ].name;
            addrs[ i ] = exports[ i ].addr;
            printf( "[reload] export: %s -> %p\n", exports[ i ].name,
                    exports[ i ].addr );
        }

        // Patch only that plugin’s PLT
        patch_ctx_t ctx = { .plugin_path = plug->canon_path,
                            .names = names,
                            .addrs = addrs,
                            .count = n_exports };
        dl_iterate_phdr( patch_phdr_callback, &ctx );

        // Cleanup
        plug->last_mtime = st.st_mtime;
        // Free names from exports
        for ( size_t i = 0; i < n_exports; i++ ) {
            free( exports[ i ].name );
        }
        free( exports );
        free( names );
        free( addrs );
    }

    // Do NOT dlclose(handle), so the new mapping stays valid for patched PLT
    // entries
    return 1;
}

// Check all plugins, reload if needed
static void check_and_reload_all( plugin_t* plugins, size_t plugin_count ) {
    for ( size_t i = 0; i < plugin_count; i++ ) {
        int res = reload_plugin_if_needed( &plugins[ i ] );
        if ( res > 0 ) {
            printf( "[manager] Reloaded plugin: %s\n",
                    plugins[ i ].canon_path );
        }
        // res==0: not changed; res<0: error printed
    }
}
#endif

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

static void* reload( const char* _soPath ) {
    if ( check( _soPath ) ) {
        printf( "[reload] Detected change in %s\n", _soPath );

        void* handle = dlmopen( LM_ID_NEWLM, _soPath, RTLD_NOW | RTLD_LOCAL );

        void* new_iter = dlsym( handle, "iterate" );
        printf( "new@%p old@%p\n", new_iter, dlsym( NULL, "iterate" ) );

        printf( "%s %p %s\n", _soPath, handle, dlerror() );

        return ( handle );
    }

    return ( NULL );
}

static int patch_main_plt( const char* sym, void* new_addr, plthook_t* ph ) {
    int res = plthook_replace( ph, sym, new_addr, NULL );
    if ( res != 0 ) {
        fprintf( stderr, "patch %s: %s\n", sym, plthook_error() );
        return -1;
    } else if ( res == 0 ) {
        printf( "patch %s: %p\n", sym, new_addr );
    }
    return 0;
}

int main( int _argumentCount, char** _argumentVector ) {
    bool l_returnValue = false;

    char** funcs_to_patch = createArray( char* );

    plthook_t* ph;
    /* 1) get a handle to the main executable */
    void* main_h = dlopen( NULL, RTLD_NOW );
    if ( !main_h ) {
        return 1;
    }

    /* 2) open plthook on that handle */
    if ( plthook_open_by_handle( &ph, main_h ) != 0 ) {
        return 1;
    }

    {
        /* 3) enumerate all imported (PLT) symbols */
        printf( "Imported functions (no @…):\n" );
        unsigned int idx = 0;
        const char* symname;
        void** got_addr;
        while ( plthook_enum( ph, &idx, &symname, &got_addr ) == 0 ) {
            if ( strchr( symname, '@' ) )
                continue;
            printf( "%s\n", symname );
            insertIntoArray( &funcs_to_patch, duplicateString( symname ) );
        }
    }

    insertIntoArray( &funcs_to_patch, NULL );

#if defined( HOT_RELOAD )

    char* l_rootSharedObjectPath =
        duplicateString( HOT_RELOAD_ROOT_SHARED_OBJECT_FILE_NAME ".so" );

    {
        char* l_directoryPath = getApplicationDirectoryAbsolutePath();

        concatBeforeAndAfterString( &l_rootSharedObjectPath, l_directoryPath,
                                    NULL );

        free( l_directoryPath );
    }

    char* dirPath = getApplicationDirectoryAbsolutePath();

    void* newHandle = reload( l_rootSharedObjectPath );

    struct link_map* root_lm;
    if ( dlinfo( newHandle, RTLD_DI_LINKMAP, &root_lm ) != 0 ) {
        perror( "dlinfo" );
        return 1;
    }

    for ( struct link_map* lm = root_lm; lm; lm = lm->l_next ) {
        if ( lm->l_name == NULL || lm->l_name[ 0 ] == '\0' )
            continue; /* skip anonymous mappings */

        void* so_h = dlopen( lm->l_name, RTLD_NOW | RTLD_NOLOAD );
        if ( !so_h )
            continue;

        /* 5) For each symbol in your list, lookup & patch */
        for ( char** p = funcs_to_patch; *p; ++p ) {
            // void* new_addr = dlsym( so_h, *p );
            void* new_addr = dlsym( newHandle, *p );

            // find out which .so this new_addr lives in
            Dl_info info;
            if ( !dladdr( new_addr, &info ) || !info.dli_fname )
                continue;

            // only patch if that .so path starts with dirPath
            if ( strncmp( info.dli_fname, dirPath, strlen( dirPath ) ) != 0 )
                continue;

            if ( new_addr ) {
                if ( __builtin_strcmp( "iterate", *p ) == 0 ) {
                    patch_main_plt( *p, new_addr, ph );
                }
            }
            /* else: symbol not in this .so, that’s fine */
        }

        dlclose( so_h );
    }

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
                newHandle = reload( l_rootSharedObjectPath );

                if ( dlinfo( newHandle, RTLD_DI_LINKMAP, &root_lm ) != 0 ) {
                    perror( "dlinfo" );
                    return 1;
                }

                for ( struct link_map* lm = root_lm; lm; lm = lm->l_next ) {
                    if ( lm->l_name == NULL || lm->l_name[ 0 ] == '\0' )
                        continue; /* skip anonymous mappings */

                    void* so_h = dlopen( lm->l_name, RTLD_NOW | RTLD_NOLOAD );
                    if ( !so_h )
                        continue;

                    /* 5) For each symbol in your list, lookup & patch */
                    for ( char** p = funcs_to_patch; *p; ++p ) {
                        // void* new_addr = dlsym( so_h, *p );
                        void* new_addr = dlsym( newHandle, *p );

                        // find out which .so this new_addr lives in
                        Dl_info info;
                        if ( !dladdr( new_addr, &info ) || !info.dli_fname )
                            continue;

                        // only patch if that .so path starts with dirPath
                        if ( strncmp( info.dli_fname, dirPath,
                                      strlen( dirPath ) ) != 0 )
                            continue;

                        if ( new_addr ) {
                            patch_main_plt( *p, new_addr, ph );
                        }
                        /* else: symbol not in this .so, that’s fine */
                    }

                    dlclose( so_h );
                }
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
