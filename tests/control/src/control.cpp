#include "control.hpp"

#include "stdfunc.hpp"
#include "test.hpp"

// FIX: Make reflectable
TEST( controls, controls_t$reflection ) {
    static_assert( stdfunc::meta::is_reflectable< control::control_t > );

    stdfunc::meta::hasMemberWithName< control::control_t >( "scancode" );
}
