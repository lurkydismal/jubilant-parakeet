#include "watch_t.h"

#include <limits.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"

#define MAX_EVENT_AMOUNT ( 8 )
#define MAX_SINGLE_EVENT_SIZE ( sizeof( struct inotify_event ) + NAME_MAX + 1 )

watch_t watch_t$create( void ) {
    watch_t l_returnValue = DEFAULT_wATCH;

    {
        l_returnValue.fileDescriptor = inotify_init1( IN_NONBLOCK );

        // epoll
        {
            l_returnValue.epollDescriptor = epoll_create1( 0 );

            struct epoll_event l_event = {
                .events = EPOLLIN, .data.fd = l_returnValue.fileDescriptor };

            epoll_ctl( l_returnValue.epollDescriptor, EPOLL_CTL_ADD,
                       l_returnValue.fileDescriptor, &l_event );
        }

        l_returnValue.watchDescriptors = createArray( int );
        l_returnValue.watchCallbacks = createArray( watchCallback_t );
        l_returnValue.callbackContexts = createArray( void* );
    }

    return ( l_returnValue );
}

bool watch_t$destroy( watch_t* _watch ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_watch ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define TRY_CLOSE_OR_EXIT( _descriptor )                        \
    do {                                                        \
        l_returnValue = ( close( _watch->_descriptor ) != -1 ); \
        if ( UNLIKELY( !l_returnValue ) ) {                     \
            goto EXIT;                                          \
        }                                                       \
        _watch->_descriptor = -1;                               \
    } while ( 0 )

        TRY_CLOSE_OR_EXIT( fileDescriptor );
        TRY_CLOSE_OR_EXIT( epollDescriptor );

#undef TRY_CLOSE_OR_EXIT

        FREE_ARRAY( _watch->watchDescriptors );
        _watch->watchDescriptors = NULL;

        FREE_ARRAY( _watch->watchCallbacks );
        _watch->watchCallbacks = NULL;

        FREE_ARRAY( _watch->callbackContexts );
        _watch->callbackContexts = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool watch_t$add$toPath( watch_t* _watch,
                         const char* _path,
                         watchCallback_t _callback,
                         void* _context,
                         bool _isDirectory ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_watch ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_callback ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        uint32_t l_flags = IN_CLOSE_WRITE;

        // Set flags
        if ( _isDirectory ) {
            l_flags |= ( IN_MOVED_FROM | IN_MOVED_TO );
        }

        char* l_path = duplicateString( _path );

        concatBeforeAndAfterString(
            &l_path, asset_t$loader$assetsDirectory$get(), NULL );

        const int l_watchDescriptor =
            inotify_add_watch( _watch->fileDescriptor, l_path, l_flags );

        l_returnValue = ( l_watchDescriptor != -1 );

        free( l_path );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        insertIntoArray( &( _watch->watchDescriptors ), l_watchDescriptor );
        insertIntoArray( &( _watch->watchCallbacks ),
                         ( watchCallback_t )_callback );
        insertIntoArray( &( _watch->callbackContexts ), _context );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
bool watch_t$add$toGlob( watch_t* _watch,
                         const char* _glob,
                         watchCallback_t _callback,
                         void* _context,
                         bool _onlyDirectories ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_watch ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_callback ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        ( void )sizeof( _context );
        ( void )sizeof( _onlyDirectories );

        trap();

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool watch_t$remove( watch_t* _watch ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_watch ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            ( inotify_rm_watch(
                  _watch->fileDescriptor,
                  arrayLastElement( _watch->watchDescriptors ) ) != -1 );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        removeLastArray( &( _watch->watchDescriptors ) );
        removeLastArray( &( _watch->watchCallbacks ) );
        removeLastArray( &( _watch->callbackContexts ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool watch_t$check( watch_t* _watch, bool _isBlocking ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_watch ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        struct epoll_event l_events[ MAX_EVENT_AMOUNT ];
        const int l_timeout = ( ( _isBlocking ) ? ( -1 ) : ( 0 ) );

        int l_eventAmount = epoll_wait( _watch->epollDescriptor, l_events,
                                        MAX_EVENT_AMOUNT, l_timeout );

        l_returnValue = ( l_eventAmount != -1 );

        if ( UNLIKELY( !l_returnValue ) ) {
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

                    while ( l_eventPointer <
                            ( l_eventsBuffer + l_readAmount ) ) {
                        const struct inotify_event* l_event =
                            ( struct inotify_event* )l_eventPointer;

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

                        if ( UNLIKELY( !l_callback ) ) {
                            log$transaction$query( ( logLevel_t )error,
                                                   "Corrupted callback" );

                            trap();

                            break;
                        }

                        l_returnValue = l_callback( l_context, l_event->name,
                                                    l_event->cookie );

                        if ( UNLIKELY( !l_returnValue ) ) {
                            goto EXIT;
                        }

                        const size_t l_eventSize =
                            ( sizeof( struct inotify_event ) + l_event->len );

                        l_eventPointer += l_eventSize;
                    }
                }

                break;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
