#include "FPS.h"

#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "log.h"
#include "stdfunc.h"

static pthread_t g_FPSCountThread;
static bool g_shouldFPSCountThreadWork = false;
static size_t g_currentFramesPerSecond = 0;
static size_t* g_totalFramesPassed = NULL;

static void* FPS$count( void* _data ) {
    ( void )( sizeof( _data ) );

    const struct timespec l_sleepTime = { .tv_sec = 1, .tv_nsec = 0 };

    size_t l_previousTotalFramesPerSecond = 0;

    while ( LIKELY( g_shouldFPSCountThreadWork ) ) {
        g_currentFramesPerSecond =
            ( *g_totalFramesPassed - l_previousTotalFramesPerSecond );

        log$transaction$query$format( ( logLevel_t )info, "FPS: %d\n",
                                      g_currentFramesPerSecond );

        l_previousTotalFramesPerSecond = *g_totalFramesPassed;

        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_sleepTime, NULL );
    }

    return ( NULL );
}

bool FPS$init( size_t* restrict _totalFramesPassed ) {
    bool l_returnValue = false;

    if ( UNLIKELY( g_totalFramesPassed ) ) {
        l_returnValue = false;

        goto EXIT;
    }

    {
        g_currentFramesPerSecond = 0;

        g_totalFramesPassed = _totalFramesPassed;

        g_shouldFPSCountThreadWork = true;

        if ( UNLIKELY( pthread_create( &g_FPSCountThread, NULL, FPS$count,
                                       NULL ) ) ) {
            log$transaction$query$format(
                ( logLevel_t )error,
                "%d: Insufficient resources to create another thread, or a "
                "system-imposed limit on the number of threads was "
                "encountered\n",
                EAGAIN );

            FPS$quit();

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool FPS$quit( void ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        l_returnValue = false;

        goto EXIT;
    }

    {
        g_shouldFPSCountThreadWork = false;

        pthread_join( g_FPSCountThread, NULL );

        g_currentFramesPerSecond = 0;

        g_totalFramesPassed = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

size_t FPS$get$current( void ) {
    size_t l_returnValue = 0;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        l_returnValue = SIZE_MAX;

        goto EXIT;
    }

    {
        l_returnValue = g_currentFramesPerSecond;
    }

EXIT:
    return ( l_returnValue );
}

size_t FPS$get$total( void ) {
    size_t l_returnValue = 0;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        l_returnValue = SIZE_MAX;

        goto EXIT;
    }

    {
        l_returnValue = *g_totalFramesPassed;
    }

EXIT:
    return ( l_returnValue );
}
