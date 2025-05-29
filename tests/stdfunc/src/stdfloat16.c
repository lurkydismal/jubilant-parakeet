#include "stdfloat16.h"

#include <math.h>
#include <stdint.h>

#include "test.h"

// TODO: Refactor

TEST( float16_bits ) {
    // Zero
    {
        float16_t f = 0.0;

        ASSERT_EQ( "%u", float16_bits( f ), 0x0000 );
    }

    // Negative zero
    {
        float16_t f = -0.0;

        ASSERT_EQ( "%u", float16_bits( f ), 0x8000 );
    }

    // One
    {
        float16_t f = 1.0;

        ASSERT_EQ( "%u", float16_bits( f ), 0x3C00 );
    }

    // Negative two
    {
        float16_t f = -2.0;

        ASSERT_EQ( "%u", float16_bits( f ), 0xC000 );
    }

    // Positive infinity
    {
        float16_t f = INFINITY;

        ASSERT_EQ( "%u", float16_bits( f ), 0x7C00 );
    }

    // NaN ( not a number ) — only check the exponent bits match, and fraction
    // is non-zero
    {
        float16_t f = NAN;

        uint16_t nan_bits = float16_bits( f );

        // Exponent all 1s
        ASSERT_EQ( "%u", ( nan_bits & 0x7C00 ), 0x7C00 );

        // Mantissa non-zero
        ASSERT_NOT_EQ( "%u", ( nan_bits & 0x03FF ), 0x0000 );
    }
}

TEST( utoa ) {
    char buffer[ 32 ];

    {
        utoa( 0, buffer );

        ASSERT_STRING_EQ( buffer, "0" );
    }

    {
        utoa( 1, buffer );

        ASSERT_STRING_EQ( buffer, "1" );
    }

    {
        utoa( 42, buffer );

        ASSERT_STRING_EQ( buffer, "42" );
    }

    {
        utoa( 1234567890, buffer );

        ASSERT_STRING_EQ( buffer, "1234567890" );
    }

    {
        utoa( UINT32_MAX, buffer );

        ASSERT_STRING_EQ( buffer, "4294967295" );
    }
}

TEST( float16_to_decimal_str ) {
    char* s;

    // Zero
    {
        s = float16_to_decimal_str( ( _Float16 )0.0 );

        ASSERT_STRING_EQ( s, "0.000000" );

        free( s );
    }

    // One
    {
        s = float16_to_decimal_str( ( _Float16 )1.0 );

        ASSERT_STRING_EQ( s, "1.000000" );

        free( s );
    }

    // Negative One
    {
        s = float16_to_decimal_str( ( _Float16 )-1.0 );

        ASSERT_STRING_EQ( s, "-1.000000" );

        free( s );
    }

    // Two and a half
    {
        s = float16_to_decimal_str( ( _Float16 )2.5 );

        ASSERT_STRING_EQ( s, "2.500000" );

        free( s );
    }

    // Infinity
    {
        s = float16_to_decimal_str( ( _Float16 )INFINITY );

        ASSERT_STRING_EQ( s, "inf" );

        free( s );
    }

    // Negative infinity
    {
        s = float16_to_decimal_str( ( _Float16 )-INFINITY );

        ASSERT_STRING_EQ( s, "-inf" );

        free( s );
    }

    // NaN ( can vary, just check prefix )
    {
        s = float16_to_decimal_str( ( _Float16 )NAN );

        ASSERT_TRUE( strncmp( s, "nan", 3 ) == 0 );

        free( s );
    }
}
