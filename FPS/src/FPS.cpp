#include "FPS.hpp"

#include <chrono>
#include <functional>
#include <stop_token>
#include <thread>

#if !defined( TESTS )

#include "log.hpp"

#endif

namespace {

std::jthread g_loggerThread;

void logger( const std::stop_token& _stopToken,
             [[maybe_unused]] size_t& _frameCount ) {
    using clock_t = std::chrono::steady_clock;
    using namespace std::chrono_literals;

    auto l_timeLast = clock_t::now();

    while ( !_stopToken.stop_requested() ) [[likely]] {
        std::this_thread::sleep_for( 1s );

        const auto l_timeNow = clock_t::now();

#if !defined( TESTS )
        {
            const auto l_framesCount = _frameCount;
            // FIX: Make total and not per frame
            _frameCount = 0;

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

void init( size_t& _frameCount ) {
    g_loggerThread = std::jthread( logger, std::ref( _frameCount ) );
}

void quit() {
    g_loggerThread.request_stop();

    if ( g_loggerThread.joinable() ) [[likely]] {
        g_loggerThread.join();
    }
}

} // namespace FPS
