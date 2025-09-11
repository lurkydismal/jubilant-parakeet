#if defined( __SANITIZE_LEAK__ )

#include <sanitizer/lsan_interface.h>

#endif

#if defined( HOT_RELOAD )

#include <dlfcn.h>
#include <link.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/stat.h>

#include <bit>
#include <format>
#include <gsl/pointers>
#include <iterator>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "plthook.h"
#include "stdfunc.hpp"

#endif

#include <experimental/scope>

#include "event.hpp"
#include "init.hpp"
#include "iterate.hpp"
#include "quit.hpp"
#include "runtime.hpp"

namespace {

runtime::applicationState_t g_applicationState;

#if defined( HOT_RELOAD )

constexpr std::string_view g_hotReloadRootSharedObjectFileName = "root";
constexpr size_t g_hotReloadCheckDelayFrames = 2;

const std::string g_hotReloadUnloadFunctionSignature = "hotReload$unload";
constexpr std::string_view g_hotReloadLoadFunctionSignature = "hotReload$load";

using hotReload$unload_t = bool ( * )( void** _state,
                                       size_t* _stateSize,
                                       applicationState_t* _applicationState );
using hotReload$load_t = bool ( * )( void* _state,
                                     size_t _stateSize,
                                     applicationState_t* _applicationState );

using state_t = struct state {
    void* data;
    size_t size;
};

const std::string g_rootSharedObjectPath =
    std::format( "{}{}.so",
                 getApplicationDirectoryAbsolutePath(),
                 g_hotReloadRootSharedObjectFileName );
sigjmp_buf g_jumpBuffer;

void crashHandler( [[maybe_unused]] int _signal ) {
    siglongjmp( g_jumpBuffer, 1 );
}

auto hasPathChanged( std::string_view _path ) -> bool {
    bool l_returnValue = false;

    do {
        static long l_lastModificationTime = 0;

        struct stat l_statisticss{};

        assert( ( stat( std::string( _path ).c_str(), &l_statisticss ) == 0 ),
                "stat failed" );

        if ( l_statisticss.st_mtime <= l_lastModificationTime ) [[likely]] {
            break;
        }

        l_lastModificationTime = l_statisticss.st_mtime;

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

// TODO: Improve
auto getNamespaceId( gsl::not_null< void* >& _handle ) -> Lmid_t {
    Lmid_t l_returnValue = dlinfo( _handle, RTLD_DI_LMID, &l_returnValue );

    assert( l_returnValue != -1 );

    return ( l_returnValue );
}

auto collectStatesFromHandle( gsl::not_null< void* >& _handle,
                              std::vector< std::string >& _stateNames,
                              std::vector< state_t >& _states ) -> bool {
    bool l_returnValue = false;

    do {
        const Lmid_t l_namespaceId = getNamespaceId( _handle );

        struct link_map* l_linkMap = nullptr;

        // Get dymanic library link map
        {
            const int l_result = dlinfo( _handle, RTLD_DI_LINKMAP, &l_linkMap );

            assert( l_result != -1 );
        }

        // Call each module state saving function
        for ( auto _linkMap = l_linkMap; _linkMap;
              _linkMap = _linkMap->l_next ) {
            const std::string_view l_sharedObjectPath = _linkMap->l_name;

            if ( ( UNLIKELY( l_sharedObjectPath.empty() ) ) ||
                 ( UNLIKELY( l_sharedObjectPath ==
                             g_rootSharedObjectPath ) ) ) {
                continue;
            }

            if ( !stdfunc::comparePathsDirectories( l_sharedObjectPath,
                                                    g_rootSharedObjectPath ) ) {
                continue;
            }

            gsl::not_null< void* > l_handle = dlmopen(
                l_namespaceId, std::string( l_sharedObjectPath ).c_str(),
                ( RTLD_LAZY | RTLD_NOLOAD ) );

            auto l_unloadCallback = std::bit_cast< hotReload$unload_t >(
                dlsym( l_handle, g_hotReloadUnloadFunctionSignature.c_str() ) );

            auto l_callUnloadCallback =
                [ & ]( hotReload$unload_t& _unloadCallback ) {
                    bool l_returnValue = false;

                    do {
                        if ( !_unloadCallback ) {
                            break;
                        }

                        state_t l_state;

                        const bool l_result = _unloadCallback(
                            &( l_state.data ), &( l_state.size ),
                            &g_applicationState );

                        if ( UNLIKELY( !l_result ) ) {
                            break;
                        }

                        _stateNames.emplace_back( l_sharedObjectPath );
                        _states.emplace_back( l_state );

                        l_returnValue = true;
                    } while ( false );

                    return ( l_returnValue );
                };

            l_returnValue = l_callUnloadCallback( l_unloadCallback );

            if ( !l_returnValue ) {
                break;
            }
        }
    } while ( false );

    return ( l_returnValue );
}

// FIX: Implement
constexpr auto getUndefinedFunctionsFromSoPath(
    [[maybe_unused]] std::string_view _sharedObjectPath ) {
    constexpr std::array< std::string_view, 35 > l_returnValue = {
        "__extendhfsf2",
        "__truncsfhf2",
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
        "SDL_RenderTextureRotated",
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

    return ( l_returnValue );
}

constexpr auto getMainExecutableFunctionNamesToPatch() {
    constexpr std::array< std::string_view, 5 > l_returnValue = {
        // Main loop
        "init",
        "event",
        "iterate",
        "quit",

        // Other
        "concatBeforeAndAfterString",
    };

    return ( l_returnValue );
}

auto hotReloadSo( std::string_view _sharedObjectPath ) -> bool {
    bool l_returnValue = false;

    {
        if ( hasPathChanged( _sharedObjectPath ) ) {
            static gsl::not_null< void* > l_managerHandle =
                dlopen( nullptr, RTLD_NOW );

            {
                std::vector< std::string > l_stateNames;
                std::vector< state_t > l_states;

                l_returnValue = collectStatesFromHandle(
                    l_managerHandle, l_stateNames, l_states );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT2;
                }

                static void* l_previouManagerHandle = nullptr;

                // Can not be NULL
                l_previouManagerHandle = l_managerHandle;

                // Load new
                l_managerHandle = dlmopen(
                    LM_ID_NEWLM, std::string( _sharedObjectPath ).c_str(),
                    ( RTLD_LAZY | RTLD_DEEPBIND ) );

                if ( sigsetjmp( g_jumpBuffer, 0 ) != 0 ) {
                    l_managerHandle = l_previouManagerHandle;
                }

                assert( l_managerHandle != nullptr );

                {
                    const auto l_functionNames =
                        getMainExecutableFunctionNamesToPatch();

                    assert( !l_functionNames.empty() );

                    std::array< void*, l_functionNames.size() >
                        l_functionAddresses{};

                    {
                        const auto l_undefinedFunctions =
                            getUndefinedFunctionsFromSoPath(
                                _sharedObjectPath );

                        assert( !l_undefinedFunctions.empty() );

                        const Lmid_t l_namespaceId =
                            getNamespaceId( l_managerHandle );

                        assert( l_namespaceId != LONG_MAX );

                        struct link_map* l_linkMap;
                        const int l_result = dlinfo(
                            l_managerHandle, RTLD_DI_LINKMAP, &l_linkMap );

                        assert( l_result >= 0 );

                        for ( struct link_map* _linkMap = l_linkMap; _linkMap;
                              _linkMap = _linkMap->l_next ) {
                            const char* l_sharedObjectPath = _linkMap->l_name;

                            if ( UNLIKELY( !l_sharedObjectPath ||
                                           !( *l_sharedObjectPath ) ) ) {
                                continue;
                            }

                            if ( UNLIKELY(
                                     __builtin_strcmp(
                                         l_sharedObjectPath,
                                         g_rootSharedObjectPath.c_str() ) ==
                                     0 ) ) {
                                continue;
                            }

                            if ( !comparePathsDirectories(
                                     l_sharedObjectPath,
                                     g_rootSharedObjectPath.c_str() ) ) {
                                continue;
                            }

                            void* l_handle =
                                dlmopen( l_namespaceId, l_sharedObjectPath,
                                         ( RTLD_LAZY | RTLD_NOLOAD ) );

                            assert( l_handle != NULL );

                            {
                                hotReload$load_t l_loadCallback =
                                    ( hotReload$load_t )dlsym(
                                        l_handle,
                                        std::string(
                                            g_hotReloadLoadFunctionSignature )
                                            .c_str() );

                                if ( l_loadCallback ) {
                                    const auto l_stateNamesIterator =
                                        std::ranges::find( l_stateNames,
                                                           l_sharedObjectPath );

                                    if ( LIKELY( l_stateNamesIterator !=
                                                 l_stateNames.end() ) ) {
                                        const size_t l_stateNameIndex =
                                            std::ranges::distance(
                                                l_stateNames.begin(),
                                                l_stateNamesIterator );
                                        state_t& l_state =
                                            l_states[ l_stateNameIndex ];

                                        void* l_stateData = l_state.data;
                                        size_t l_stateDataSize = l_state.size;

                                        l_returnValue = l_loadCallback(
                                            l_stateData, l_stateDataSize,
                                            &g_applicationState );

                                        if ( UNLIKELY( !l_returnValue ) ) {
#if 0
                                            // FIX: Port trap() to C++
                                            trap(
                                                std::format(
                                                    "{} failed",
                                                    g_hotReloadLoadFunctionSignature )
                                                    .c_str() );
#endif
                                        }
                                    }
                                }
                            }

                            // Patch undefined functions with base namespace
                            {
                                plthook_t* l_plthookHandle = nullptr;

                                const int l_result = plthook_open_by_handle(
                                    &l_plthookHandle, l_handle );

                                if ( l_result != 0 ) {
                                    continue;
                                }

                                for ( auto _undefinedFunction :
                                      l_undefinedFunctions ) {
                                    plthook_replace(
                                        l_plthookHandle,
                                        std::string( _undefinedFunction )
                                            .c_str(),
                                        dlsym( RTLD_DEFAULT,
                                               std::string( _undefinedFunction )
                                                   .c_str() ),
                                        nullptr );
                                }

                                plthook_close( l_plthookHandle );
                            }

                            // Locate functions used in main executable for
                            // patching
                            for ( auto [ _functionName, _functionAddress ] :
                                  std::views::zip( l_functionNames,
                                                   l_functionAddresses ) ) {
                                _functionAddress = dlsym(
                                    l_handle,
                                    std::string( _functionName ).c_str() );
                            }
                        }
                    }

                    // Patch functions in main executable
                    {
                        void* l_baseHandle = dlopen( nullptr, RTLD_NOW );

                        plthook_t* l_plthookHandle = nullptr;

                        const int l_result = plthook_open_by_handle(
                            &l_plthookHandle, l_baseHandle );

                        assert( l_result == 0 );

                        for ( const auto [ _functionName, _functionAddress ] :
                              std::views::zip( l_functionNames,
                                               l_functionAddresses ) ) {
                            assert(
                                _functionAddress,
                                "Function for main executable was not found" );

                            const int l_result = plthook_replace(
                                l_plthookHandle,
                                std::string( _functionName ).c_str(),
                                _functionAddress, nullptr );

                            assert( l_result == 0 );
                        }

                        plthook_close( l_plthookHandle );
                    }
                }

            EXIT2:
            }
        }

        l_returnValue = true;
    }

    return ( l_returnValue );
}

#endif

} // namespace

auto main( int _argumentCount, char** _argumentVector ) -> int {
    do {
        {
            auto l_argumentVectorView =
                std::span( _argumentVector, _argumentCount ) |
                std::ranges::views::transform(
                    []( const char* _argument ) -> std::string_view {
                        return ( _argument );
                    } ) |
                std::ranges::to< std::vector< std::string_view > >();

            if ( !runtime::init( g_applicationState, l_argumentVectorView ) )
                [[unlikely]] {
                break;
            }
        }

        auto l_onExit = std::experimental::scope_exit( [ & ] {
            runtime::quit( g_applicationState );

#if defined( __SANITIZE_LEAK__ )

            __lsan_do_leak_check();

#endif
        } );

#if defined( HOT_RELOAD )

        signal( SIGSEGV, crashHandler );
        signal( SIGILL, crashHandler );
        signal( SIGBUS, crashHandler );

        if ( !hotReloadSo( g_rootSharedObjectPath.c_str() ) ) [[unlikely]] {
            break;
        }

        size_t l_iterationCount = 0;

#endif

        std::vector< runtime::event_t > l_events( 16 );

        // Main loop
        for ( ;; ) {
            vsync::begin();

            const auto l_handleEvents = [ & ] {
                // Poll events
                {
                    l_events.clear();

                    SDL_PumpEvents();

                    runtime::event_t l_event{};

                    while ( SDL_PollEvent( &l_event ) ) {
                        l_events.emplace_back( l_event );
                    }
                }

                return ( std::ranges::all_of(
                             l_events,
                             [ & ]( const runtime::event_t& _event ) {
                                 return ( runtime::event( g_applicationState,
                                                          _event ) );
                             } ) &&
                         (
                             // Empty means last event on current frame
                             runtime::event( g_applicationState, {} ) ) );
            };

            if ( !l_handleEvents() ) {
                break;
            }

            if ( !runtime::iterate( g_applicationState ) ) {
                break;
            }

            vsync::end();

            ( g_applicationState.renderContext.totalFramesRendered )++;

#if defined( HOT_RELOAD )

            l_iterationCount++;

            if ( ( l_iterationCount % g_hotReloadCheckDelayFrames ) == 0 ) {
                if ( !hotReloadSo( g_rootSharedObjectPath.c_str() ) )
                    [[unlikely]] {
                    break;
                }
            }
#endif
        }
    } while ( false );

    return ( ( g_applicationState.status ) ? ( EXIT_SUCCESS )
                                           : ( EXIT_FAILURE ) );
}
