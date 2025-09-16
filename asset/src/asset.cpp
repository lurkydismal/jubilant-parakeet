#include "asset.hpp"

#include <thread>

#include "log.hpp"
#include "stdfunc.hpp"

namespace asset {

namespace {

std::jthread g_loggerThread;

#define MAX_REQUESTS 1000

using saveRequest_t = struct saveRequest {
    std::vector< std::byte > asset;
    std::string path;
    bool needAppend = false;
};

std::array< saveRequest_t, MAX_REQUESTS > g_saveQueue;
size_t g_saveQueueLength = 0;
pthread_mutex_t g_saveQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_saveQueueCondition = PTHREAD_COND_INITIALIZER;

/**
 * @brief Async save queue resolver
 */
void resolver( const std::stop_token& _stopToken ) {
    while ( !_stopToken.stop_requested() ) [[likely]] {
        saveRequest l_saveRequest;

        {
            pthread_mutex_lock( &g_saveQueueMutex );

            while ( !g_saveQueueLength && _stopToken.stop_requested() )
                [[unlikely]] {
                pthread_cond_wait( &g_saveQueueCondition, &g_saveQueueMutex );
            }

            if ( !_stopToken.stop_requested() && !g_saveQueueLength )
                [[unlikely]] {
                pthread_mutex_unlock( &g_saveQueueMutex );

                break;
            }

            // Copy request locally to process without holding lock
            l_saveRequest = g_saveQueue[ 0 ];

            // Move the rest of queue
            __builtin_memmove(
                std::bit_cast< void* >( g_saveQueue.data() ),
                &( g_saveQueue[ 1 ] ),
                ( ( g_saveQueueLength - 1 ) * sizeof( saveRequest ) ) );

            g_saveQueueLength--;

            pthread_mutex_unlock( &g_saveQueueMutex );
        }

        {
            bool l_result = sync::save( l_saveRequest.asset, l_saveRequest.path,
                                        l_saveRequest.needAppend );

            if ( !l_result ) [[unlikely]] {
                logg::error( "Saving asset of size {} to path: '{}'",
                             l_saveRequest.asset.size(), l_saveRequest.path );
            }

            // TODO: Pop request
        }
    }

    logg::info( "Asset save queue resolver stopped" );
}

} // namespace

namespace loader {

auto init() -> bool {
    g_loggerThread = std::jthread( resolver );

    return ( true );
}

void quit() {
    g_loggerThread.request_stop();

    if ( g_loggerThread.joinable() ) [[likely]] {
        g_loggerThread.join();
    }
}

} // namespace loader

namespace sync {

auto load( std::string_view _path ) -> asset_t {
    asset_t l_returnValue = std::nullopt;

    do {
#if 0
        int l_fileDescriptor = open( _path, O_RDONLY );

        if ( l_fileDescriptor == -1 ) {
            logg::error( "Opening asset: '{}'", _path );

            goto FILE_EXIT;
        }

        {
            // Get file size
            const off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );

            if ( !l_fileSize ) [[unlikely]] {
                logg::error( "Getting asset size" );

                goto FILE_EXIT;
            }

            lseek( l_fileDescriptor, 0, SEEK_SET );

            std::vector< std::byte > l_data( l_fileSize );

            const ssize_t l_readenCount =
                read( l_fileDescriptor, l_data.data(), l_data.size() );

            if ( !( l_readenCount == l_fileSize ) ) [[unlikely]] {
                logg::error( "Reading asset" );

                goto FILE_EXIT;
            }

            l_returnValue = l_data;
        }

    FILE_EXIT:
        if ( l_fileDescriptor == -1 ) [[unlikely]] {
            break;
        }

        close( l_fileDescriptor );

        if ( !l_returnValue ) [[unlikely]] {
            break;
        }
#endif
    } while ( false );

EXIT:
    return ( l_returnValue );
}

auto save( std::span< std::byte > _asset,
           std::string_view _path,
           bool _needAppend ) -> bool {
    bool l_returnValue = false;

    do {
        int l_fileDescriptor = -1;

        {
            // Open file descriptor
            {
#if 0
                size_t l_openFlags = ( O_WRONLY | O_CREAT );

                if ( !_needAppend ) {
                    l_openFlags |= O_TRUNC;
                }

                // 0 - No special bits
                // 6 - Read & Write for owner
                // 4 - Read for group members
                // 4 - Read for others
                l_fileDescriptor = open( _path, l_openFlags, 0644 );
#endif
            }

            if ( l_fileDescriptor == -1 ) {
                logg::error( "Opening file for saving: '{}'", _path );

                break;
            }

            const ssize_t l_writtenCount =
                write( l_fileDescriptor, _asset.data(), _asset.size() );

            if ( l_writtenCount != static_cast< ssize_t >( _asset.size() ) )
                [[unlikely]] {
                logg::error( "Writing to file" );

                goto EXIT_SAVE;
            }
        }

        l_returnValue = true;

    EXIT_SAVE:
        close( l_fileDescriptor );
    } while ( false );

    return ( l_returnValue );
}

} // namespace sync

namespace async {

auto save( std::span< std::byte > _asset,
           std::string_view _path,
           bool _needAppend ) -> bool {
    bool l_returnValue = false;

    do {
        pthread_mutex_lock( &g_saveQueueMutex );

        if ( g_saveQueueLength >= MAX_REQUESTS ) {
            stdfunc::trap();

            logg::error( "Save queue length is already full" );

            pthread_mutex_unlock( &g_saveQueueMutex );

            break;
        }

        saveRequest l_saveRequest{
            .asset = std::ranges::to< std::vector< std::byte > >( _asset ),
            .path = std::string( _path ),
            .needAppend = _needAppend };

        g_saveQueue.at( g_saveQueueLength ) = l_saveRequest;

        g_saveQueueLength++;

        pthread_cond_signal( &g_saveQueueCondition );
        pthread_mutex_unlock( &g_saveQueueMutex );

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

} // namespace async

} // namespace asset
