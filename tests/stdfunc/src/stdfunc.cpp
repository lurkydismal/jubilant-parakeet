#include "stdfunc.hpp"

#include "test.hpp"

// TODO: Add isSpace,
// random$numberMinMax, random$value, random$view, random$fillMinMax,
// random$fill, makeVariantContainer, compress$text, compress$data,
// decompress$text, decompress$data, meta$iterateStructUnionTopMostFields
namespace stdfunc {

TEST( stdfunc, STRINGIFY ) {
    EXPECT_EQ( STRINGIFY( Tessst ), "Tessst" );
    EXPECT_EQ( STRINGIFY( stdfunc::color::g_green ),
               "stdfunc::color::g_green" );
}

TEST( stdfunc, _b ) {
    {
        constexpr std::byte l_constexprCheck = 0_b;

        static_assert( l_constexprCheck == std::byte{} );
    }

    {
        constexpr std::byte l_constexprCheck = '0'_b;

        static_assert( l_constexprCheck == std::byte{ 0x30 } );
    }

    EXPECT_EQ( 'A'_b, std::byte{ 0x41 } );
    EXPECT_EQ( 'a'_b, std::byte{ 0x61 } );
    EXPECT_EQ( 'G'_b, std::byte{ 0x47 } );

    EXPECT_EQ( 0_b, std::byte{ 0 } );
    EXPECT_EQ( 10_b, std::byte{ 0xA } );
    EXPECT_EQ( 17_b, std::byte{ 0x11 } );
}

TEST( stdfunc, _bytes ) {
    constexpr auto l_constexprCheck = "C"_bytes;

    static_assert( l_constexprCheck == std::array{ std::byte{ 0x43 } } );

    std::array l_array = { std::byte{ 0x46 }, std::byte{ 0x46 },
                           std::byte{ 0x46 } };

    EXPECT_EQ( "FFF"_bytes, l_array );
}

TEST( stdfunc, bitsToBytes ) {
    EXPECT_EQ( bitsToBytes( 0 ), 0 );

    EXPECT_EQ( bitsToBytes( 8 ), 1 );
    EXPECT_EQ( bitsToBytes( 16 ), 2 );
    EXPECT_EQ( bitsToBytes( 32 ), 4 );

    EXPECT_EQ( bitsToBytes( 1 ), 1 );
    EXPECT_EQ( bitsToBytes( 7 ), 1 );
    EXPECT_EQ( bitsToBytes( 9 ), 2 );
    EXPECT_EQ( bitsToBytes( 15 ), 2 );
    EXPECT_EQ( bitsToBytes( 17 ), 3 );

    // 63 bits = 7.875 bytes -> 8
    EXPECT_EQ( bitsToBytes( 63 ), 8 );
    EXPECT_EQ( bitsToBytes( 64 ), 8 );
    EXPECT_EQ( bitsToBytes( 65 ), 9 );

    // Test different arithmetic types
    EXPECT_EQ( bitsToBytes< int >( 7 ), 1 );
    EXPECT_EQ( bitsToBytes< long long >( 9 ), 2 );
    EXPECT_EQ( bitsToBytes< unsigned >( 32 ), 4 );
    EXPECT_EQ( bitsToBytes< uint64_t >( 65 ), 9 );

    // Constexpr evaluation
    constexpr auto l_constexprCheck =
        ( bitsToBytes( 17 ) == 3 && bitsToBytes( 64 ) == 8 &&
          bitsToBytes( 0 ) == 0 );

    static_assert( l_constexprCheck );

    EXPECT_TRUE( l_constexprCheck );
}

TEST( stdfunc, lengthOfNumber ) {
    constexpr size_t l_constexprCheck = lengthOfNumber( size_t{ 123 } );

    static_assert( l_constexprCheck == 3 );

    size_t l_actualLengthFailed = 0;
    size_t l_expectedLengthFailed = 0;

    for ( auto _index : std::views::iota( size_t{}, 10'000'000uz ) ) {
        const size_t l_actualLength = lengthOfNumber( _index );
        const size_t l_expectedLength =
            ( ( _index == 0 ) ? ( 1 ) : ( log10( _index ) + 1 ) );

        if ( l_actualLength != l_expectedLength ) {
            l_actualLengthFailed = l_actualLength;
            l_expectedLengthFailed = l_expectedLength;
        }
    }

    EXPECT_EQ( l_actualLengthFailed, l_expectedLengthFailed );
}

TEST( stdfunc, isSpace ) {
    // True cases (standard C whitespace chars)
    EXPECT_TRUE( isSpace( ' ' ) );
    EXPECT_TRUE( isSpace( '\f' ) );
    EXPECT_TRUE( isSpace( '\n' ) );
    EXPECT_TRUE( isSpace( '\r' ) );
    EXPECT_TRUE( isSpace( '\t' ) );
    EXPECT_TRUE( isSpace( '\v' ) );

    // False cases (non-whitespace chars)
    EXPECT_FALSE( isSpace( 'a' ) );
    EXPECT_FALSE( isSpace( 'Z' ) );
    EXPECT_FALSE( isSpace( '0' ) );
    EXPECT_FALSE( isSpace( '_' ) );
    EXPECT_FALSE( isSpace( '-' ) );
    EXPECT_FALSE( isSpace( '\0' ) );

    // Constexpr check
    constexpr bool l_constexprCheck = ( isSpace( ' ' ) && isSpace( '\t' ) &&
                                        !isSpace( 'X' ) && !isSpace( '1' ) );

    static_assert( l_constexprCheck );

    EXPECT_TRUE( l_constexprCheck );
}

TEST( stdfunc, sanitizeString ) {
    // Normal case with spaces and a comment
    EXPECT_EQ( sanitizeString( "  Hello  World   # Comment here" ) |
                   std::ranges::to< std::string >(),
               "Hello  World" );

    // Leading and trailing spaces
    EXPECT_EQ(
        sanitizeString( "   test   " ) | std::ranges::to< std::string >(),
        "test" );

    // No spaces, no comments
    EXPECT_EQ( sanitizeString( "NoSpacesOrComments" ) |
                   std::ranges::to< std::string >(),
               "NoSpacesOrComments" );

    // Only spaces
    EXPECT_EQ( sanitizeString( "       " ) | std::ranges::to< std::string >(),
               "" );

    // Only comment
    EXPECT_EQ( sanitizeString( "   # This is a comment" ) |
                   std::ranges::to< std::string >(),
               "" );

    // Empty string
    EXPECT_EQ( sanitizeString( "" ) | std::ranges::to< std::string >(), "" );

    // Comment in the middle
    EXPECT_EQ( sanitizeString( "KeepThis # RemoveThis" ) |
                   std::ranges::to< std::string >(),
               "KeepThis" );
}

TEST( stdfunc, random$number ) {
    {
        // Integral range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            int l_value = random::number( 1, 10 );
            EXPECT_GE( l_value, 1 );
            EXPECT_LE( l_value, 10 );
        }

        // Floating-point range
        for ( int l_i = 0; l_i < 1000; ++l_i ) {
            double l_value = random::number( 0.5, 2.5 );
            EXPECT_GE( l_value, 0.5 );
            EXPECT_LE( l_value, 2.5 );
        }

        // Different types
        auto l_intVal = random::number< int >( 5, 5 ); // degenerate range
        auto l_floatVal =
            random::number< float >( 1.0f, 1.0f ); // degenerate range
        EXPECT_EQ( l_intVal, 5 );
        EXPECT_FLOAT_EQ( l_floatVal, 1.0f );

        // Variability check (not all results should be identical)
        bool l_sawDifferent = false;
        int l_first = random::number( 1, 3 );
        for ( int l_i = 0; l_i < 50; ++l_i ) {
            if ( random::number( 1, 3 ) != l_first ) {
                l_sawDifferent = true;
                break;
            }
        }

        EXPECT_TRUE( l_sawDifferent ) << "RNG seems stuck or deterministic";
    }

    // Ensure random numbers are different across calls
    {
        const auto l_numberFirst = random::number< size_t >();
        const auto l_numberSecond = random::number< size_t >();

        EXPECT_NE( l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number< size_t >(), size_t{} );
    }

    for ( auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
        EXPECT_NE( random::number< size_t >(), size_t{} );
    }
}

TEST( stdfunc, random$value ) {
    // Test with vector<int>
    std::vector< int > l_vec{ 1, 2, 3, 4, 5 };
    for ( int l_i = 0; l_i < 100; ++l_i ) {
        int& l_ref = random::value( l_vec );
        EXPECT_TRUE( l_ref >= 1 && l_ref <= 5 );
    }

    // Test with std::array
    std::array< char, 3 > l_arr{ 'a', 'b', 'c' };
    for ( int l_i = 0; l_i < 100; ++l_i ) {
        char& l_ref = random::value( l_arr );
        EXPECT_TRUE( l_ref == 'a' || l_ref == 'b' || l_ref == 'c' );
    }

    // Reference semantics: modify element through returned ref
    std::vector< int > l_modVec{ 10, 20, 30 };
    int& l_picked = random::value( l_modVec );
    int l_old = l_picked;
    l_picked = 99; // modify through reference
    EXPECT_TRUE( std::ranges::find( l_modVec, 99 ) != l_modVec.end() );
    // Put it back so test is stable
    *std::ranges::find( l_modVec, 99 ) = l_old;

    // Degenerate case: container with 1 element
    std::vector< int > l_single{ 42 };
    for ( int l_i = 0; l_i < 10; ++l_i ) {
        EXPECT_EQ( random::value( l_single ), 42 );
    }

    // Empty container should assert (death test)
    std::vector< int > l_empty;
    EXPECT_DEATH( { random::value( l_empty ); }, "" );
}

TEST( stdfunc, generateHash ) {
    // Invalid inputs
    {
        // Valid buffer
        {
            // Non NULL terminated string
            {
                std::array l_buffer = { '0'_b };

                EXPECT_NE( generateHash( l_buffer ), size_t{} );
            }

            // NULL terminated string
            {
                std::array l_buffer = ""_bytes;

                EXPECT_NE( generateHash( l_buffer ), size_t{} );
            }
        }
    }

    // Valid buffer
    {
        // Ensure multiple calls return nonzero values
        {
            for ( const auto _index : std::views::iota( 1uz, 10'000uz ) ) {
                const size_t l_bufferLength = _index;

                std::vector< std::byte > l_buffer( l_bufferLength );

                stdfunc::random::fill( l_buffer );

                EXPECT_EQ( l_buffer.size(), l_bufferLength );

                const size_t l_actualHash = generateHash( l_buffer );

                EXPECT_TRUE( l_actualHash );
            }
        }
    }
}

TEST( stdfunc, filesystem$getApplicationDirectoryAbsolutePath ) {
    auto l_path = filesystem::getApplicationDirectoryAbsolutePath();

    EXPECT_NE( l_path, std::nullopt );
}

} // namespace stdfunc
