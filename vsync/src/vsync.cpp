#include "vsync.hpp"

#include <sys/time.h>

#include <bit>
#include <chrono>

namespace vsync {

namespace {

vsync_t g_vsyncType = vsync_t::off;
float g_desiredFPS = 0;
timespec g_sleepTime, g_startTime, g_endTime;

} // namespace

void init( vsync_t _vsyncType, float16_t _desiredFPS ) {
    stdfunc::assert( g_desiredFPS );

    g_desiredFPS = _desiredFPS;
    g_vsyncType = _vsyncType;

    if ( _vsyncType == vsync_t::software ) {
        g_sleepTime = {
            .tv_sec = 0,
            .tv_nsec =
                std::chrono::duration_cast< std::chrono::nanoseconds >(
                    std::chrono::duration< double >( 1.0 ) / _desiredFPS )
                    .count() };
    }
}

void quit() {
    g_desiredFPS = 0;

    __builtin_memset( std::bit_cast< void* >( &g_sleepTime ), 0,
                      sizeof( g_sleepTime ) );
    __builtin_memset( std::bit_cast< void* >( &g_startTime ), 0,
                      sizeof( g_startTime ) );
    __builtin_memset( std::bit_cast< void* >( &g_endTime ), 0,
                      sizeof( g_endTime ) );
}

void begin() {
    if ( g_vsyncType == vsync_t::software ) {
        clock_gettime( CLOCK_MONOTONIC, &g_startTime );
    }
}

void end() {
    if ( g_vsyncType == vsync_t::software ) {
        clock_gettime( CLOCK_MONOTONIC, &g_endTime );

        timespec l_adjustedSleepTime{};

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
