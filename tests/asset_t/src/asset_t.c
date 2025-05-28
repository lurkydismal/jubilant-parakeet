#include "asset_t.h"

#include "test.h"

TEST( asset_t$loader$init ) {
    // Valid string
    {
        bool l_returnValue = asset_t$loader$init( "TEST" );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = asset_t$loader$quit();

        ASSERT_FALSE( l_returnValue );
    }

    // Duplicate init
    {
        bool l_returnValue = asset_t$loader$init( "TEST" );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = asset_t$loader$init( "TEST2" );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = asset_t$loader$quit();

        ASSERT_FALSE( l_returnValue );
    }

    // NULL input - should fail
    {
        bool l_returnValue = asset_t$loader$init( NULL );

        ASSERT_FALSE( l_returnValue );
    }
}
