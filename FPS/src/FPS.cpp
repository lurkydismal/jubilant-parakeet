#include "FPS.hpp"

#include <thread>

#include "log.hpp"

namespace {

std::jthread g_loggerThread;

void logger( const std::stop_token& _stopToken,
             std::atomic< size_t >& _frameCount ) {
    using clock = std::chrono::steady_clock;
    using namespace std::chrono_literals;

    auto l_timeLast = clock::now();

    while ( !_stopToken.stop_requested() ) [[likely]] {
        std::this_thread::sleep_for( 1s );

        const auto l_timeNow = clock::now();

#if !defined( TESTS )
        {
            const auto l_framesCount = _frameCount.exchange( 0 );

            const auto l_frameDuration = ( l_timeNow - l_timeLast );
            const std::chrono::duration< double > l_frameDurationInSeconds =
                l_frameDuration;

            double l_FPS = 0;

            if ( l_frameDurationInSeconds > 0s ) [[likely]] {
                l_FPS = ( l_framesCount / l_frameDurationInSeconds.count() );
            }

            logg::info( "FPS: {:.2f}", l_FPS );
        }
#endif

        l_timeLast = l_timeNow;
    }

#if !defined( TESTS )

    logg::info( "FPS logger stopped." );

#endif
}

} // namespace

namespace FPS {

void init( std::atomic< size_t >& _frameCount ) {
    g_loggerThread = std::jthread( logger, std::ref( _frameCount ) );
}

void quit() {
    g_loggerThread.request_stop();

    if ( g_loggerThread.joinable() ) [[likely]] {
        g_loggerThread.join();
    }
}

} // namespace FPS
