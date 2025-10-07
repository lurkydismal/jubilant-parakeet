#include "forward_circular_buffer.hpp"

#include "test.hpp"

using namespace fcb;

// small helper to convert iterator range to vector
template < typename It >
static auto toVector( It _b, const It& _e ) {
    std::vector< std::decay_t< decltype( *_b ) > > l_out;
    for ( ; _b != _e; ++_b ) {
        l_out.push_back( *_b );
    }
    return ( l_out );
}

TEST( ForwardCircularBuffer, DefaultConstructEmpty ) {
    forwardCircularBuffer< int, 4 > l_buf;
    EXPECT_EQ( l_buf.size(), 0u );
    EXPECT_EQ( l_buf.max_size(), 4u );
    EXPECT_TRUE( l_buf.empty() );
    EXPECT_FALSE( l_buf.full() );
}

TEST( ForwardCircularBuffer, PushBackAndAccess ) {
    forwardCircularBuffer< int, 4 > l_buf;

    l_buf.push_back( 10 );
    l_buf.push_back( 20 );
    l_buf.push_back( 30 );

    EXPECT_EQ( l_buf.size(), 3u );
    EXPECT_FALSE( l_buf.empty() );
    EXPECT_FALSE( l_buf.full() );

    EXPECT_EQ( l_buf.front(), 10 );
    EXPECT_EQ( l_buf.back(), 30 );

    EXPECT_EQ( l_buf[ 0 ], 10 );
    EXPECT_EQ( l_buf[ 1 ], 20 );
    EXPECT_EQ( l_buf[ 2 ], 30 );

    EXPECT_EQ( l_buf.at( 0 ), 10 );
    EXPECT_EQ( l_buf.at( 1 ), 20 );
    EXPECT_EQ( l_buf.at( 2 ), 30 );

    // iterate and compare
    auto l_v = toVector( l_buf.begin(), l_buf.end() );
    EXPECT_EQ( l_v, ( std::vector< int >{ 10, 20, 30 } ) );
}

TEST( ForwardCircularBuffer, EmplaceBackAndBackReference ) {
    forwardCircularBuffer< std::pair< int, int >, 3 > l_buf;
    auto& l_r =
        l_buf.emplace_back( 1, 2 ); // should construct and return back()
    EXPECT_EQ( l_r.first, 1 );
    EXPECT_EQ( l_r.second, 2 );

    // Ensure back() returns same object and value
    EXPECT_EQ( l_buf.back().first, 1 );
    EXPECT_EQ( l_buf.back().second, 2 );
    EXPECT_EQ( l_buf.size(), 1u );
}

TEST( ForwardCircularBuffer, FillAndFull ) {
    forwardCircularBuffer< int, 5 > l_buf;
    l_buf.fill( 7 );
    EXPECT_EQ( l_buf.size(), l_buf.max_size() );
    EXPECT_TRUE( l_buf.full() );
    EXPECT_FALSE( l_buf.empty() );

    // All elements should be 7
    for ( auto l_x : l_buf ) {
        EXPECT_EQ( l_x, 7 );
    }

    // rbegin should iterate in reverse order (still all equal)
    auto l_rv = toVector( l_buf.rbegin(), l_buf.rend() );
    EXPECT_EQ( l_rv.size(), l_buf.max_size() );
    for ( auto l_x : l_rv ) {
        EXPECT_EQ( l_x, 7 );
    }
}

TEST( ForwardCircularBuffer, IterationOrderAndReverse ) {
    forwardCircularBuffer< int, 6 > l_buf;
    l_buf.push_back( 1 );
    l_buf.push_back( 2 );
    l_buf.push_back( 3 );
    l_buf.push_back( 4 );

    // Forward iteration
    EXPECT_EQ( toVector( l_buf.begin(), l_buf.end() ),
               ( std::vector< int >{ 1, 2, 3, 4 } ) );

    // Reverse iteration
    EXPECT_EQ( toVector( l_buf.rbegin(), l_buf.rend() ),
               ( std::vector< int >{ 4, 3, 2, 1 } ) );
}

TEST( ForwardCircularBuffer, ClearAndDataPointer ) {
    forwardCircularBuffer< int, 4 > l_buf;
    l_buf.push_back( 11 );
    l_buf.push_back( 22 );
    EXPECT_EQ( l_buf.size(), 2u );

    int* l_p = l_buf.data();
    static_assert( std::is_same_v< decltype( l_p ), int* > );
    // p may point to internal array; check first two values are what we expect
    EXPECT_EQ( l_p[ 0 ], l_buf[ 0 ] );

    l_buf.clear();
    EXPECT_EQ( l_buf.size(), 0u );
    EXPECT_TRUE( l_buf.empty() );
}

TEST( ForwardCircularBuffer, SwapBuffers ) {
    forwardCircularBuffer< int, 4 > l_a, l_b;
    l_a.push_back( 1 );
    l_a.push_back( 2 );

    l_b.push_back( 10 );
    l_b.push_back( 20 );
    l_b.push_back( 30 );

    // swap contents
    l_a.swap( l_b );

    EXPECT_EQ( l_a.size(), 3u );
    EXPECT_EQ( l_b.size(), 2u );
    EXPECT_EQ( toVector( l_a.begin(), l_a.end() ),
               ( std::vector< int >{ 10, 20, 30 } ) );
    EXPECT_EQ( toVector( l_b.begin(), l_b.end() ),
               ( std::vector< int >{ 1, 2 } ) );
}

TEST( ForwardCircularBuffer, PopBackRemovesAndReturnsValue ) {
    forwardCircularBuffer< int, 4 > l_buf;
    l_buf.push_back( 5 );
    l_buf.push_back( 6 );
    EXPECT_EQ( l_buf.size(), 2u );

    int l_val = l_buf.pop_back();
    EXPECT_EQ( l_val, 6 );
    EXPECT_EQ( l_buf.size(), 1u );
    EXPECT_EQ( l_buf.back(), 5 );

    l_val = l_buf.pop_back();
    EXPECT_EQ( l_val, 5 );
    EXPECT_EQ( l_buf.size(), 0u );
    EXPECT_TRUE( l_buf.empty() );
}

static auto toVector( auto _b, auto _e ) -> std::vector< int > {
    std::vector< std::decay_t< decltype( *_b ) > > l_out;
    for ( ; _b != _e; ++_b )
        l_out.push_back( *_b );
    return l_out;
}

// ---------- Basic wrap-around and iterator arithmetic ----------
TEST( FcbIntrusive, WrapAroundAfterPopAndPush ) {
    forwardCircularBuffer< int, 4 > l_buf;
    // push 0..3 -> buffer full
    l_buf.push_back( 0 );
    l_buf.push_back( 1 );
    l_buf.push_back( 2 );
    l_buf.push_back( 3 );
    EXPECT_EQ( l_buf.size(), 4u );
    // pop last element
    {
        int l_removed = l_buf.pop_back();
        EXPECT_EQ( l_removed, 3 );
        EXPECT_EQ( l_buf.size(), 3u );
    }

    // push a new value that should reuse the freed slot -> logically
    // contents should be 0,1,2,99
    l_buf.push_back( 99 );

    auto l_v = toVector( l_buf.begin(), l_buf.end() );
    EXPECT_EQ( l_v.size(), 4u );
    EXPECT_EQ( l_v, ( std::vector< int >{ 0, 1, 2, 99 } ) );
}

// ---------- Iterator random-access operations ----------
TEST( FcbIntrusive, IteratorRandomAccessAndComparison ) {
    forwardCircularBuffer< int, 6 > l_buf;
    for ( int l_i = 1; l_i <= 4; ++l_i ) {
        l_buf.push_back( l_i ); // 1,2,3,4
    }

    auto l_it = l_buf.begin();
    EXPECT_EQ( *l_it, 1 );
    EXPECT_EQ( *( l_it + 1 ), 2 );
    EXPECT_EQ( l_it[ 2 ], 3 );

    auto l_it2 = l_it + 4; // points to end
    EXPECT_EQ( l_it2, l_buf.end() );
    EXPECT_LT( l_it, l_it2 );
    EXPECT_GT( l_it2, l_it );

    // distance
    ptrdiff_t l_dist = l_it2 - l_it;
    EXPECT_EQ( l_dist, static_cast< ptrdiff_t >( l_buf.size() ) );
}

// ---------- Out of range access (contract) ----------
TEST( FcbIntrusive, OutOfRangeAccessTriggersAssertOrThrow ) {
    forwardCircularBuffer< int, 3 > l_buf;
    l_buf.push_back( 10 );
    l_buf.push_back( 20 );

#if defined( GTEST_HAS_DEATH_TEST )
    // If your stdfunc::assert aborts the process we can use death test
    ASSERT_DEATH( ( void )l_buf.at( 5 ),
                  "" ); // exact message can be left empty
#endif
}

// ---------- Push when full (contract failure) ----------
TEST( FcbIntrusive, PushWhenFullTriggersAssertOrDeath ) {
    forwardCircularBuffer< int, 2 > l_buf;
    l_buf.push_back( 1 );
    l_buf.push_back( 2 );
#if defined( GTEST_HAS_DEATH_TEST )
    EXPECT_DEATH( l_buf.push_back( 3 ), ".*" );
#else
    GTEST_SKIP() << "Push-when-full death test skipped";
#endif
}

// ---------- Move-only type lifecycle and destructor checks ----------
struct lifeTracker {
    lifeTracker() = default;
    static inline int g_constructions = 0;
    static inline int g_destructions = 0;
    int v;
    auto operator=( lifeTracker&& ) -> lifeTracker& = delete;
    lifeTracker( int _vv ) : v( _vv ) { ++g_constructions; }
    lifeTracker( lifeTracker&& _o ) : v( _o.v ) { ++g_constructions; }
    lifeTracker( const lifeTracker& ) = delete;
    auto operator=( const lifeTracker& ) -> lifeTracker& = delete;
    ~lifeTracker() { ++g_destructions; }
};

TEST( FcbIntrusive, EmplaceBackMoveOnlyAndPopDestroys ) {
    lifeTracker::g_constructions = 0;
    lifeTracker::g_destructions = 0;

    forwardCircularBuffer< lifeTracker, 4 > l_buf;
    l_buf.emplace_back( 10 );
    l_buf.emplace_back( 20 );
    EXPECT_EQ( l_buf.size(), 2u );
    EXPECT_EQ( lifeTracker::g_constructions, 2 );
    EXPECT_EQ( lifeTracker::g_destructions, 0 );

    // pop should move out and destroy the removed objects storage
    {
        lifeTracker l_removed = l_buf.pop_back();
        EXPECT_EQ( l_removed.v, 20 );
    }
    // after pop_back, one object was destroyed
    EXPECT_EQ( lifeTracker::g_destructions, 1 );
    EXPECT_EQ( l_buf.size(), 1u );

    // clear() behavior: ideally should destroy remaining element(s)
    l_buf.clear();
    // clear() in current implementation does NOT destroy placement-new
    // elements. If you have fixed clear to destroy elements, expect
    // destructions == 2. Otherwise, this test fails or must be updated. We mark
    // as informative:
    EXPECT_EQ( lifeTracker::g_destructions, 2 );
    if ( lifeTracker::g_destructions != 2 ) {
        GTEST_FAIL() << "clear() did not destroy elements; implement "
                        "destructor calls in clear()";
    }
}

// ---------- Iterator validity after wrap (read/write through iterator)
// ----------
TEST( FcbIntrusive, IteratorDereferenceAssignAndObserve ) {
    forwardCircularBuffer< int, 5 > l_buf;
    for ( int l_i = 0; l_i < 3; l_i++ ) {
        l_buf.push_back( l_i + 1 ); // 1,2,3
    }

    auto l_it = l_buf.begin();
    ++l_it; // points to 2
    *l_it = 42;
    EXPECT_EQ( l_buf[ 1 ], 42 );

    // Check reverse iterator modifies the underlying sequence
    auto l_rit = l_buf.rbegin();
    *l_rit = 100; // modifies last element (3 -> 100)
    EXPECT_EQ( l_buf.back(), 100 );
}

// ---------- Iterator difference across different buffers is unspecified
// ----------
TEST( FcbIntrusive, IteratorDifferentBuffersComparisonIsUnspecified ) {
    forwardCircularBuffer< int, 4 > l_a, l_b;
    l_a.push_back( 1 );
    l_b.push_back( 2 );

    auto l_ia = l_a.begin();
    auto l_ib = l_b.begin();
    // The implementation documents unspecified behavior if iterators from
    // different buffers are compared; ensure such comparisons do not crash but
    // may produce any result. We just run them to make sure no UB/crash in
    // typical builds.
    ( void )( l_ia == l_ib );
    ( void )( l_ia != l_ib );
    SUCCEED();
}
