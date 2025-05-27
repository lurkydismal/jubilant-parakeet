#include "object_t.h"
#include "test.h"

TEST( object_t$create ) {
    object_t l_object = object_t$create();

    bool l_returnValue = object_t$destroy( &l_object );

    ASSERT_TRUE( l_returnValue );
}

TEST( object_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( object_t$destroy( NULL ) );

    {
        object_t l_object = object_t$create();

        ASSERT_EQ( "%p", l_object.renderer, NULL );

        bool l_returnValue = object_t$destroy( &l_object );

        ASSERT_TRUE( l_returnValue );
    }
}
