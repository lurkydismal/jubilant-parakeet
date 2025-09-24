#include "watch.hpp"

namespace watch {

namespace {

constexpr size_t g_maxEventAmount = 8;
constexpr size_t g_maxSingleEventSize =
    ( sizeof( struct inotify_event ) + NAME_MAX + 1 );

} // namespace

#if 0
watch_t watch_t$create( void ) {
    watch_t l_returnValue = DEFAULT_wATCH;

    {
        l_returnValue._fileDescriptor = inotify_init1( IN_NONBLOCK );

        // epoll
        {
            l_returnValue._epollDescriptor = epoll_create1( 0 );

            struct epoll_event l_event = {
                .events = EPOLLIN,
                .data.fd = l_returnValue._fileDescriptor,
            };

            epoll_ctl( l_returnValue._epollDescriptor, EPOLL_CTL_ADD,
                       l_returnValue._fileDescriptor, &l_event );
        }

        l_returnValue.watchDescriptors = createArray( int );
        l_returnValue.watchCallbacks = createArray( watchCallback_t );
        l_returnValue.callbackContexts = createArray( void* );
    }

    return ( l_returnValue );
}

bool watch_t$add$toPath( watch_t* _watch,
                         const char* _path,
                         watchCallback_t _callback,
                         void* _context,
                         bool _isDirectory ) {
    uint32_t l_flags = IN_CLOSE_WRITE;

    // Set flags
    if ( _isDirectory ) {
        l_flags |= ( IN_MOVED_FROM | IN_MOVED_TO );
        l_flags |= IN_DELETE;

    } else {
        l_flags |= IN_DELETE_SELF;
    }

    int l_watchDescriptor = -1;

    {
        char* l_path = duplicateString( _path );

        concatBeforeAndAfterString(
            &l_path, asset_t$loader$assetsDirectory$get(), NULL );

        l_watchDescriptor =
            inotify_add_watch( _watch->fileDescriptor, l_path, l_flags );

        l_returnValue = ( l_watchDescriptor != -1 );

        free( l_path );
    }

    if ( UNLIKELY( !l_returnValue ) ) {
        log$transaction$query$format( ( logLevel_t )error,
                                      "Adding watch to path: '%s'", _path );

        goto EXIT;
    }

    insertIntoArray( &( _watch->watchDescriptors ), l_watchDescriptor );
    insertIntoArray( &( _watch->watchCallbacks ),
                     ( watchCallback_t )_callback );
    insertIntoArray( &( _watch->callbackContexts ), _context );
}

bool watch_t$add$toGlob( watch_t* _watch,
                         const char* _glob,
                         watchCallback_t _callback,
                         void* _context,
                         bool _needDirectories ) {
    char** l_paths = getPathsByGlob( _glob, NULL, false );

    FOR_ARRAY( char* const*, l_paths ) {
        const bool l_isDirectory =
            ( ( _needDirectories ) ? ( isPathDirectory( *_element ) )
                                   : ( false ) );

        l_returnValue = watch_t$add$toPath( _watch, *_element, _callback,
                                            _context, l_isDirectory );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Adding watch to path" );

            break;
        }
    }

    FREE_ARRAY_ELEMENTS( l_paths );
    FREE_ARRAY( l_paths );

    if ( UNLIKELY( !l_returnValue ) ) {
        goto EXIT;
    }
}

bool watch_t$remove( watch_t* _watch ) {
    l_returnValue =
        ( inotify_rm_watch( _watch->fileDescriptor,
                            arrayLastElement( _watch->watchDescriptors ) ) !=
          -1 );

    if ( UNLIKELY( !l_returnValue ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "Removing watch from path" );

        goto EXIT;
    }

    removeLastElementArray( &( _watch->watchDescriptors ) );
    removeLastElementArray( &( _watch->watchCallbacks ) );
    removeLastElementArray( &( _watch->callbackContexts ) );
}

bool watch_t$check( watch_t* _watch, bool _isBlocking ) {
    struct epoll_event l_events[ MAX_EVENT_AMOUNT ];
    const int l_timeout = ( ( _isBlocking ) ? ( -1 ) : ( 0 ) );

    int l_eventAmount = epoll_wait( _watch->epollDescriptor, l_events,
                                    MAX_EVENT_AMOUNT, l_timeout );

    l_returnValue = ( l_eventAmount != -1 );

    if ( UNLIKELY( !l_returnValue ) ) {
        log$transaction$query( ( logLevel_t )error, "Pulling epoll" );

        goto EXIT;
    }

    while ( l_eventAmount ) {
        l_eventAmount--;

        if ( l_events[ l_eventAmount ].data.fd == _watch->fileDescriptor ) {
            char l_eventsBuffer[ MAX_SINGLE_EVENT_SIZE * MAX_EVENT_AMOUNT ]
                __attribute__( (
                    aligned( __alignof__( struct inotify_event ) ) ) );

            const ssize_t l_readAmount =
                read( _watch->fileDescriptor, l_eventsBuffer,
                      sizeof( l_eventsBuffer ) );

            if ( !l_readAmount ) {
                continue;
            }

            {
                char* l_eventPointer = l_eventsBuffer;

                while ( l_eventPointer < ( l_eventsBuffer + l_readAmount ) ) {
                    const struct inotify_event* l_event =
                        ( struct inotify_event* )l_eventPointer;

                    if ( !isEventWrite( l_event->mask ) &&
                         !isEventDelete( l_event->mask ) &&
                         !isEventRename( l_event->mask ) ) {
                        goto LOOP_CONTINUE;
                    }

                    {
                        watchCallback_t l_callback = NULL;
                        void* l_context = NULL;

                        FOR_RANGE( arrayLength_t, 0,
                                   arrayLength( _watch->watchDescriptors ) ) {
                            const int l_watchDescriptor =
                                _watch->watchDescriptors[ _index ];

                            if ( l_watchDescriptor == l_event->wd ) {
                                l_callback = _watch->watchCallbacks[ _index ];
                                l_context = _watch->callbackContexts[ _index ];

                                break;
                            }
                        }

                        l_returnValue = !!( l_callback );

                        if ( UNLIKELY( !l_returnValue ) ) {
                            log$transaction$query( ( logLevel_t )error,
                                                   "Corrupted callback" );

                            trap();

                            goto EXIT;
                        }

                        l_returnValue =
                            l_callback( l_context, l_event->name, l_event->mask,
                                        l_event->cookie );

                        if ( UNLIKELY( !l_returnValue ) ) {
                            log$transaction$query( ( logLevel_t )error,
                                                   "Watch callback" );

                            goto EXIT;
                        }
                    }

                LOOP_CONTINUE:
                    const size_t l_eventSize =
                        ( sizeof( struct inotify_event ) + l_event->len );

                    l_eventPointer += l_eventSize;
                }
            }

            break;
        }
    }
}
#endif

} // namespace watch
