#include "FPS.hpp"

#include "test.hpp"

using namespace std::chrono_literals;

TEST( FPS, InitQuit_NoCrash_FastInterval ) {
    std::atomic< size_t > l_frameCount{ 0 };

    // use a short interval so tests run fast
    FPS::init( l_frameCount );

    // simulate some frames being produced
    for ( int l_i = 0; l_i < 200; ++l_i ) {
        l_frameCount.fetch_add( 1 );
        std::this_thread::sleep_for( 1ms );
    }

    // measure that quit returns reasonably quickly (should be ~interval)
    auto l_t0 = std::chrono::steady_clock::now();
    FPS::quit();
    auto l_dt = std::chrono::steady_clock::now() - l_t0;

    // quit should return within a small multiple of the interval
    ASSERT_LT( l_dt, 1s ) << "quit() took too long";
}

TEST( FPS, RepeatedInitQuit ) {
    std::atomic< size_t > l_frameCount{ 0 };

    for ( int l_cycle = 0; l_cycle < 5; ++l_cycle ) {
        FPS::init( l_frameCount );

        // produce a few frames
        for ( int l_i = 0; l_i < 50; ++l_i ) {
            l_frameCount.fetch_add( 1 );
            std::this_thread::sleep_for( 1ms );
        }

        FPS::quit();
        // small pause to ensure thread cleaned up (should be immediate)
        std::this_thread::sleep_for( 1s );
    }

    SUCCEED();
}
