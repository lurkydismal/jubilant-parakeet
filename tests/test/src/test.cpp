#include "test.hpp"

namespace test {

TEST( stringToBool, [] {
#if 0
    const std::array l_x = { true, false, 1, "", " true ", "hello" };

    // Assert true
    for ( const auto& _x : l_x ) {
        assertTrue( _x );
    }
#endif
    assertTrue( true );
    assertTrue( false );
    assertTrue( true );
    assertTrue( true );
    assertTrue( true );
    assertTrue( true );
} );

} // namespace test
