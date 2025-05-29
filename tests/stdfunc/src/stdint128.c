#include "stdint128.h"

#include <stdlib.h>

#include "test.h"

// TODO: Refactor

TEST( uint128_to_str ) {
    char* s;

    // Zero
    {
        s = uint128_to_str( 0 );

        ASSERT_STRING_EQ( s, "0" );

        free( s );
    }

    // Small number
    {
        s = uint128_to_str( 12345 );

        ASSERT_STRING_EQ( s, "12345" );

        free( s );
    }

    // 64-bit max value
    {
        s = uint128_to_str( ( uint128_t )UINT64_MAX );

        ASSERT_STRING_EQ( s, "18446744073709551615" );

        free( s );
    }

    // 128-bit max value
    {
        s = uint128_to_str( UINT128_MAX );

        ASSERT_STRING_EQ( s, "340282366920938463463374607431768211455" );

        free( s );
    }
}

TEST( int128_to_str ) {
    char* s;

    // Zero
    {
        s = int128_to_str( 0 );

        ASSERT_STRING_EQ( s, "0" );

        free( s );
    }

    // Small number
    {
        s = int128_to_str( 12345 );

        ASSERT_STRING_EQ( s, "12345" );

        free( s );
    }

    // 64-bit max value
    {
        s = int128_to_str( ( int128_t )INT64_MAX );

        ASSERT_STRING_EQ( s, "9223372036854775807" );

        free( s );
    }

    // 64-bit min value
    {
        s = int128_to_str( ( int128_t )INT64_MIN );

        ASSERT_STRING_EQ( s, "-9223372036854775808" );

        free( s );
    }

    // 128-bit max value
    {
        s = int128_to_str( INT128_MAX );

        ASSERT_STRING_EQ( s, "170141183460469231731687303715884105727" );

        free( s );
    }

    // 128-bit min value
    {
        s = int128_to_str( INT128_MIN );

        ASSERT_STRING_EQ( s, "-170141183460469231731687303715884105728" );

        free( s );
    }
}
