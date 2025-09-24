#pragma once

#include <sys/epoll.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <climits>
#include <functional>
#include <string_view>
#include <utility>

#include "stdfunc.hpp"

namespace watch {

#define EVENT_WRITE IN_CLOSE_WRITE
#define EVENT_DELETE IN_DELETE
#define EVENT_RENAME ( IN_MOVED_FROM | IN_MOVED_TO )

#define isEventWrite( _eventMask )                           \
    ( ( ( _eventMask ) & ( IN_MODIFY | IN_CLOSE_WRITE ) ) && \
      !( ( _eventMask ) &                                    \
         ( IN_DELETE | IN_DELETE_SELF | IN_MOVED_FROM | IN_MOVE_SELF ) ) )

#define isEventDelete( _eventMask ) \
    ( ( ( _eventMask ) &            \
        ( IN_DELETE | IN_DELETE_SELF | IN_MOVED_FROM | IN_MOVE_SELF ) ) )

#define isEventRename( _eventMask ) \
    ( ( ( _eventMask ) & ( IN_MOVED_FROM | IN_MOVED_TO | IN_MOVE_SELF ) ) )

using watchCallback_t = std::function<
    bool( std::string_view _fileName, size_t _eventMask, uint32_t _cookie ) >;

using watch_t = struct watch {
    watch() = delete;
    watch( const watch& ) = default;
    watch( watch&& ) = default;
    ~watch() {
        // TODO: Rename
        const auto l_x = [ & ]( int _descriptor ) -> auto {
            const bool l_result = ( close( _descriptor ) != -1 );

            stdfunc::assert( l_result );
        };

        l_x( _fileDescriptor );
        l_x( _epollDescriptor );
    };

    watch( std::string_view _path,
           watchCallback_t _callback,
           bool _isDirectory )
        : _fileDescriptor( inotify_init1( IN_NONBLOCK ) ),
          _epollDescriptor( epoll_create1( 0 ) ),
          _watchCallback( std::move( _callback ) ) {
        // epoll
        {
            struct epoll_event l_event = {
                .events = EPOLLIN,
                .data.fd = _fileDescriptor,
            };

            epoll_ctl( _epollDescriptor, EPOLL_CTL_ADD, _fileDescriptor,
                       &l_event );
        }
    }

    auto operator=( const watch& ) -> watch& = default;
    auto operator=( watch&& ) -> watch& = default;

    auto check( bool _isBlocking ) -> bool;

private:
    int _fileDescriptor;
    int _epollDescriptor;
    std::vector< int > _watchDescriptors;
    // TODO: Maybe multiple callbacks
    watchCallback_t _watchCallback;
};

} // namespace watch
