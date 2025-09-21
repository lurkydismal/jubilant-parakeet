#include "boxes.hpp"

#include "test.hpp"

using namespace boxes;

// Helper to quickly make a span<vector<box_t>>
static auto spanOfVec( const std::vector< box_t >& _v )
    -> std::span< const box_t > {
    return { _v.data(), _v.size() };
}

// -------------------------
// Death-tests (constructor)
// -------------------------

TEST( BoxesDeathTests, ConstructorDiesOnEmptyFrames_ASSERT_DEATH ) {
    std::vector< std::span< const box_t > > l_frames; // empty
    // Constructor should assert when given empty
    // input.
    ASSERT_DEATH( { boxes_t l_b{ std::span( l_frames ) }; }, ".*" );
}

TEST( BoxesDeathTests, ConstructorDiesOnFrameBeingEmpty_EXPECT_DEATH ) {
    // Create a frames-list that contains one empty frame span.
    std::vector< std::span< const box_t > > l_frames{
        std::span< const box_t >() };

    // The constructor checks that no contained frame is empty -> should die.
    EXPECT_DEATH( { boxes_t l_b{ std::span( l_frames ) }; }, ".*" );
}

// -------------------------
// Non-death logical tests
// -------------------------

TEST( BoxesLogic, ConstructorAcceptsNonEmptyFrames_CurrentKeyFrameContents ) {
    // frame 0
    std::vector< box_t > l_f0{ box_t( 1.0f, 2.0f, 3.0f, 4.0f ) };
    // frame 1
    std::vector< box_t > l_f1{ box_t( 5.0f, 6.0f, 7.0f, 8.0f ),
                               box_t( 9.0f, 10.0f, 11.0f, 12.0f ) };

    std::vector< std::span< const box_t > > l_frames{ spanOfVec( l_f0 ),
                                                      spanOfVec( l_f1 ) };

    // Construct (should not die)
    boxes_t l_b{ std::span( l_frames ) };

    // currentKeyFrame() initially returns frame 0
    auto l_k = l_b.currentKeyFrame();
    ASSERT_EQ( l_k.size(), 1u );
    EXPECT_FLOAT_EQ( l_k[ 0 ].x, 1.0f );
    EXPECT_FLOAT_EQ( l_k[ 0 ].y, 2.0f );
    EXPECT_FLOAT_EQ( l_k[ 0 ].width, 3.0f );
    EXPECT_FLOAT_EQ( l_k[ 0 ].height, 4.0f );
}

TEST( BoxesLogic, StepAdvancesAndStopsWhenNoLoop ) {
    std::vector< box_t > l_f0{ box_t( 0, 0, 1, 1 ) };
    std::vector< box_t > l_f1{ box_t( 1, 1, 2, 2 ) };
    std::vector< box_t > l_f2{ box_t( 2, 2, 3, 3 ) };

    std::vector< std::span< const box_t > > l_frames{
        spanOfVec( l_f0 ), spanOfVec( l_f1 ), spanOfVec( l_f2 ) };

    boxes_t l_b{ std::span( l_frames ) };

    // Advance to frame 1
    l_b.step( false );
    auto l_k1 = l_b.currentKeyFrame();
    EXPECT_EQ( l_k1.size(), 1u );
    EXPECT_FLOAT_EQ( l_k1[ 0 ].x, 1.0f );

    // Advance to frame 2
    l_b.step( false );
    auto l_k2 = l_b.currentKeyFrame();
    EXPECT_FLOAT_EQ( l_k2[ 0 ].x, 2.0f );

    // At last frame; step(false) should not wrap or overflow - should stay on
    // last frame
    l_b.step( false );
    auto l_kLast = l_b.currentKeyFrame();
    EXPECT_FLOAT_EQ( l_kLast[ 0 ].x, 2.0f );
}

TEST( BoxesLogic, StepWrapsWhenLoopTrue ) {
    std::vector< box_t > l_f0{ box_t( 0, 0, 1, 1 ) };
    std::vector< box_t > l_f1{ box_t( 1, 1, 2, 2 ) };

    std::vector< std::span< const box_t > > l_frames{ spanOfVec( l_f0 ),
                                                      spanOfVec( l_f1 ) };

    boxes_t l_b{ std::span( l_frames ) };

    // Move to last frame
    l_b.step( false );
    auto l_kLast = l_b.currentKeyFrame();
    EXPECT_FLOAT_EQ( l_kLast[ 0 ].x, 1.0f );

    // Now step with looping enabled -> should wrap to frame 0
    l_b.step( true );
    auto l_k0 = l_b.currentKeyFrame();
    EXPECT_FLOAT_EQ( l_k0[ 0 ].x, 0.0f );
}

TEST( BoxesLogic, SingleFrameStepNoChangeRegardlessOfLoopFlag ) {
    std::vector< box_t > l_f0{ box_t( 7, 8, 9, 10 ) };
    std::vector< std::span< const box_t > > l_frames{ spanOfVec( l_f0 ) };

    boxes_t l_b{ std::span( l_frames ) };

    l_b.step( false );
    EXPECT_FLOAT_EQ( l_b.currentKeyFrame()[ 0 ].x, 7.0f );

    l_b.step( true );
    EXPECT_FLOAT_EQ( l_b.currentKeyFrame()[ 0 ].x, 7.0f );
}
