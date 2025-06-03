#include "controls_t.h"

#include "test.h"

TEST( controls_t$create ) {
    controls_t l_controls = controls_t$create();

    bool l_returnValue = controls_t$destroy( &l_controls );

    ASSERT_TRUE( l_returnValue );
}

TEST( controls_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = controls_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        controls_t l_controls = controls_t$create();

        bool l_returnValue = controls_t$destroy( &l_controls );

        ASSERT_TRUE( l_returnValue );
    }
}

// TODO: Implement
TEST( controls_t$control_t$convert$fromScancode ) {}

// TODO: Implement
TEST( controls_t$control_t$convert$fromInput ) {}
