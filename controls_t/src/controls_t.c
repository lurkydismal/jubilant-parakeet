#include "controls_t.h"

#include "input.h"
#include "stdfunc.h"

controls_t controls_t$create( void ) {
    controls_t l_returnValue = DEFAULT_CONTROLS;

    {
        // Create
        {
            // Directions
            {
                l_returnValue.up = control_t$create();
                l_returnValue.down = control_t$create();
                l_returnValue.left = control_t$create();
                l_returnValue.right = control_t$create();
            }

            // Buttons
            {
                l_returnValue.A = control_t$create();
                l_returnValue.B = control_t$create();
                l_returnValue.C = control_t$create();
                l_returnValue.D = control_t$create();
            }
        }

        // Directions
        {
            l_returnValue.up.input = UP;
            l_returnValue.down.input = DOWN;
            l_returnValue.left.input = LEFT;
            l_returnValue.right.input = RIGHT;
        }

        // Buttons
        {
            l_returnValue.A.input = A;
            l_returnValue.B.input = B;
            l_returnValue.C.input = C;
            l_returnValue.D.input = D;
        }
    }

    return ( l_returnValue );
}

bool controls_t$destroy( controls_t* _controls ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_controls ) ) {
        goto EXIT;
    }

    {
#define DESTROY_DONTROL( _field ) \
    ( { ( control_t$destroy( &( _controls->_field ) ) ); } )

        l_returnValue = DESTROY_DONTROL( up );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( down );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( left );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( right );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( A );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( B );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( C );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = DESTROY_DONTROL( D );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

#undef DESTROY_DONTROL

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
