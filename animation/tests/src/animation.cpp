#include "animation.hpp"

#include <SDL3/SDL_render.h>

#include <gsl/pointers>

#include "test.hpp"

using namespace animation;

TEST( AnimationBasic, CurrentKeyFrameAndIndexing ) {
    // create two "fake" SDL_Texture pointers (must be non-null for
    // gsl::not_null)
    auto* l_t1 = reinterpret_cast< SDL_Texture* >( 0x1 );
    auto* l_t2 = reinterpret_cast< SDL_Texture* >( 0x2 );

    std::vector< slickdl::texture_t > l_keyFrames = { l_t1, l_t2 };

    // frames mapping: index into keyFrames
    // three-frame animation: frames 0->1->0
    std::vector< size_t > l_frames = { 0, 1, 0 };

    // target boxes: one frame, one box per frame (boxes.hpp test stub)
    boxes::box_t l_b{ 10.0f, 20.0f, 32.0f, 32.0f };
    boxes::boxes_t l_targetBoxes( { { l_b }, { l_b }, { l_b } } );

    animation_t l_anim( std::span< const slickdl::texture_t >( l_keyFrames ),
                        std::span< size_t >( l_frames ),
                        std::move( l_targetBoxes ) );

    // initial frame should map to keyFrames[ frames[0] ] == t1
    EXPECT_EQ( l_anim.currentKeyFrame(), l_t1 );

    // step once (non-loop): move to frames[1] -> t2
    l_anim.step( false );
    EXPECT_EQ( l_anim.currentKeyFrame(), l_t2 );

    // step second time: frames[2] -> t1
    l_anim.step( false );
    EXPECT_EQ( l_anim.currentKeyFrame(), l_t1 );

    // step again with no loop: should stay at last frame (index 2) because code
    // only resets if canLoop==true After three frames _currentFrame should be
    // at index 2 (0-based) call step(false) now: since _currentFrame == 2 and
    // frames.size() == 3, it will hit else branch and not reset
    l_anim.step( false );
    EXPECT_EQ( l_anim.currentKeyFrame(), l_t1 );

    // now construct again and test looping behavior
    boxes::boxes_t l_tb2( { { l_b }, { l_b }, { l_b } } );
    animation_t l_anim2( std::span< const slickdl::texture_t >( l_keyFrames ),
                         std::span< size_t >( l_frames ), std::move( l_tb2 ) );

    // advance to last frame
    l_anim2.step( false ); // -> index 1
    l_anim2.step( false ); // -> index 2
    EXPECT_EQ( l_anim2.currentKeyFrame(), l_t1 );

    // Now step with looping: should reset to 0 (first frame)
    l_anim2.step( true );
    EXPECT_EQ( l_anim2.currentKeyFrame(),
               l_t1 ); // frames[0] is t1 (because frames[0] == 0)
    // And stepping again should go to index 1 (t2)
    l_anim2.step( true );
    EXPECT_EQ( l_anim2.currentKeyFrame(), l_t2 );
}

TEST( AnimationTargetBox, CurrentTargetBoxAndBoxesStep ) {
    auto* l_t1 = reinterpret_cast< SDL_Texture* >( 0x10 );
    std::vector< slickdl::texture_t > l_keyFrames = { l_t1 };
    std::vector< size_t > l_frames = { 0, 0, 0 }; // indexes irrelevant here

    boxes::box_t l_b1{ 1, 2, 3, 4 };
    boxes::box_t l_b2{ 5, 6, 7, 8 };
    boxes::box_t l_b3{ 9, 10, 11, 12 };

    boxes::boxes_t l_targetBoxes( { { l_b1 }, { l_b2 }, { l_b3 } } );

    animation_t l_anim( std::span< const slickdl::texture_t >( l_keyFrames ),
                        std::span< size_t >( l_frames ),
                        std::move( l_targetBoxes ) );

    // initially, currentTargetBox should be the box from frame 0
    const boxes::box_t& l_current = l_anim.currentTargetBox();
    EXPECT_FLOAT_EQ( l_current.x, 1.0f );
    EXPECT_FLOAT_EQ( l_current.y, 2.0f );

    // step and verify targetBoxes moved forward
    l_anim.step( false );
    const boxes::box_t& l_after1 = l_anim.currentTargetBox();
    EXPECT_FLOAT_EQ( l_after1.x, 5.0f );
    EXPECT_FLOAT_EQ( l_after1.y, 6.0f );

    l_anim.step( false );
    const boxes::box_t& l_after2 = l_anim.currentTargetBox();
    EXPECT_FLOAT_EQ( l_after2.x, 9.0f );
    EXPECT_FLOAT_EQ( l_after2.y, 10.0f );
}
