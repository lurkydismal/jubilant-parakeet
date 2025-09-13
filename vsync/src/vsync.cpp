#include "vsync.hpp"

#include <sys/time.h>

#include <chrono>

#include "log.hpp"

namespace vsync {

namespace {

vsync_t g_vsyncType = vsync_t::unknownVsync;
float g_desiredFPS = 0;
struct timespec g_sleepTime, g_startTime, g_endTime;

} // namespace

auto init( const vsync_t _vsyncType, const float16_t _desiredFPS ) -> bool {
    log::variable( _desiredFPS );

    bool l_returnValue = false;

    do {
        if ( g_desiredFPS ) {
            log::error( "Already initialized" );

            break;
        }

        {
            g_desiredFPS = _desiredFPS;
            g_vsyncType = _vsyncType;

            if ( _vsyncType == vsync_t::off ) {
                g_sleepTime = {
                    .tv_sec = 0,
                    .tv_nsec =
                        std::chrono::duration_cast< std::chrono::nanoseconds >(
                            std::chrono::duration< double >( 1.0 ) /
                            _desiredFPS )
                            .count() };
            }

            log::info( "Setting vsync to {} FPS", _desiredFPS );

            log::debug( "Vsync sleep time set to {} nanoseconds",
                        g_sleepTime.tv_nsec );
        }

        l_returnValue = true;
    } while ( false );

    return ( l_returnValue );
}

void quit() {
    g_desiredFPS = 0;

    if ( g_vsyncType == vsync_t::off ) {
        g_sleepTime.tv_nsec = 0;
    }
}

void begin() {
    if ( g_vsyncType == vsync_t::off ) {
        clock_gettime( CLOCK_MONOTONIC, &g_startTime );
    }
}

void end() {
    if ( g_vsyncType == vsync_t::off ) {
        clock_gettime( CLOCK_MONOTONIC, &g_endTime );

        struct timespec l_adjustedSleepTime{};

        {
            const size_t l_iterationTimeNano =
                ( g_endTime.tv_nsec - g_startTime.tv_nsec );

            long long l_adjustedSleepNano =
                ( g_sleepTime.tv_nsec - l_iterationTimeNano );

            l_adjustedSleepNano &= -( l_adjustedSleepNano > 0 );

            l_adjustedSleepTime.tv_sec = 0;
            l_adjustedSleepTime.tv_nsec = l_adjustedSleepNano;
        }

        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_adjustedSleepTime, nullptr );
    }
}

} // namespace vsync
