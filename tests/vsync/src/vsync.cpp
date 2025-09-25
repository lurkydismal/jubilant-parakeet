#include "vsync.hpp"

#include "test.hpp"

class VsyncTest : public Test {
protected:
    void TearDown() override {
        // ensure global state cleaned between tests
        vsync::quit();
    }
};

// Test init/quit/re-init behaviour
TEST_F( VsyncTest, InitQuitReinit ) {
    using namespace vsync;

    const int l_fps = 60;
    const auto l_f16 = static_cast< float16_t >( l_fps );

    // first init should succeed
    ASSERT_DEATH( init( vsync_t::software, l_f16 ), ".*" );

    // second init without quit should fail (already initialized)
    ASSERT_DEATH( init( vsync_t::software, l_f16 ), ".*" );

    // quit should reset state and allow init again
    quit();
    ASSERT_DEATH( init( vsync_t::software, l_f16 ), ".*" );

    EXPECT_DEATH( init( vsync_t::software, 0 ), ".*" );
}

// Test that begin/end enforces at least one frame duration when vsync::off
TEST_F( VsyncTest, FrameEnforcesMinimumDuration ) {
    using namespace vsync;

    const int l_fps = 50; // target FPS for this test => 20 ms frame
    const auto l_f16 = static_cast< float16_t >( l_fps );

    ASSERT_DEATH( init( vsync_t::software, l_f16 ), ".*" );

    // expected frame duration in milliseconds (integer math mirrors your
    // implementation)
    const int l_expectedMs = 1000 / l_fps; // 20 ms

    // Measure real wall clock time between begin() and end()
    const auto l_t0 = std::chrono::steady_clock::now();
    begin();

    // simulate some small frame work (so end() must sleep the remainder)
    std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

    end();
    const auto l_t1 = std::chrono::steady_clock::now();

    const auto l_elapsedMs =
        std::chrono::duration_cast< std::chrono::milliseconds >( l_t1 - l_t0 )
            .count();

    // Allow a small margin for scheduling jitter; but ensure sleep happened.
    // For 20ms target, require at least ~15ms observed.
    const int l_marginMs = 5;
    EXPECT_GE( l_elapsedMs, l_expectedMs - l_marginMs )
        << "Elapsed " << l_elapsedMs << " ms, expected at least "
        << ( l_expectedMs - l_marginMs ) << " ms";
}

// If init() hasn't been called, begin()/end() should be effectively no-ops
TEST_F( VsyncTest, BeginEndWithoutInitIsNoop ) {
    using namespace vsync;

    const auto l_t0 = std::chrono::steady_clock::now();
    begin(); // should do nothing because vsync not initialized (g_vsyncType !=
             // off)
    std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
    end();
    const auto l_t1 = std::chrono::steady_clock::now();

    const auto l_elapsedMs =
        std::chrono::duration_cast< std::chrono::milliseconds >( l_t1 - l_t0 )
            .count();

    // This is a loose upper bound to catch blatant sleeps. Keep it generous to
    // avoid flakes.
    EXPECT_LT( l_elapsedMs, 50 )
        << "begin/end without init should not introduce a long sleep";
}
