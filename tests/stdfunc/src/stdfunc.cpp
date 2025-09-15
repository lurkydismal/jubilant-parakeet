#include "stdfunc.hpp"

#include "test.hpp"

// TODO: Add bitsToBytes, isSpace,
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
    EXPECT_EQ( 'A'_b, std::byte{ 0xA } );
    EXPECT_EQ( 'a'_b, std::byte{ 0xA } );
    EXPECT_EQ( 'G'_b, std::byte{ 0 } );

    EXPECT_EQ( 0_b, std::byte{ 0 } );
    EXPECT_EQ( 10_b, std::byte{ 0xA } );
    EXPECT_EQ( 17_b, std::byte{ 0x11 } );
}

#if 0
TEST( _bytes, ( [] {
          std::array l_array = { std::byte{ 0xF }, std::byte{ 0xF },
                                 std::byte{ 0xF } };

          EXPECT_EQ( "FFF"_bytes, l_array );
      } ) );
#endif

#if 0
TEST( lengthOfNumber, ( [] {
          size_t l_actualLengthFailed = 0;
          size_t l_expectedLengthFailed = 0;

          for ( const auto _index :
                std::views::iota( size_t{}, 10'000'000uz ) ) {
              const size_t l_actualLength = lengthOfNumber( _index );
              const size_t l_expectedLength =
                  ( ( _index == 0 ) ? ( 1 ) : ( log10( _index ) + 1 ) );

              if ( l_actualLength != l_expectedLength ) {
                  l_actualLengthFailed = l_actualLength;
                  l_expectedLengthFailed = l_expectedLength;
              }
          }

          EXPECT_EQ( l_actualLengthFailed, l_expectedLengthFailed );
      } ) );
#endif

#if 0
TEST( random$number, ( [] {
          // Ensure random numbers are different across calls
          {
              const auto l_numberFirst = random::number< size_t >();
              const auto l_numberSecond = random::number< size_t >();

              assertNotEqual( l_numberFirst, l_numberSecond );
          }

          // Ensure multiple calls return nonzero values
          for ( const auto _ : std::views::iota( size_t{}, 10'000'000uz ) ) {
              assertNotEqual( random::number< size_t >(), size_t{} );
          }
      } ) );
#endif

#if 0
TEST( generateHash, ( [] {
          // Invalid inputs
          {
              // Valid buffer
              {
                  // Non NULL terminated string
                  {
                      std::array l_buffer = { '0'_b };

                      assertNotEqual( generateHash( l_buffer ), size_t{} );
                  }

                  // NULL terminated string
                  {
                      std::array l_buffer = ""_bytes;

                      assertNotEqual( generateHash( l_buffer ), size_t{} );
                  }
              }
          }

          // Valid buffer
          {
              // Ensure multiple calls return nonzero values
              {
                  for ( const auto _index :
                        std::views::iota( 1uz, 10'000uz ) ) {
                      const size_t l_bufferLength = _index;

                      std::vector< std::byte > l_buffer( l_bufferLength );

                      stdfunc::random::fill( l_buffer );

                      EXPECT_EQ( l_buffer.size(), l_bufferLength );

                      const size_t l_actualHash = generateHash( l_buffer );

                      assertTrue( l_actualHash );
                  }
              }
          }
      } ) );
#endif

#if 0
TEST( sanitizeString, ( [] {
          // Normal case with spaces and a comment
          EXPECT_EQ( sanitizeString( "  Hello  World   # Comment here" ),
                       "HelloWorld" );

          // Leading and trailing spaces
          EXPECT_EQ( sanitizeString( "   test   " ), "test" );

          // No spaces, no comments
          EXPECT_EQ( sanitizeString( "NoSpacesOrComments" ),
                       "NoSpacesOrComments" );

          // Only spaces
          EXPECT_EQ( sanitizeString( "       " ), "" );

          // Only comment
          EXPECT_EQ( sanitizeString( "   # This is a comment" ), "" );

          // Empty string
          EXPECT_EQ( sanitizeString( "" ), "" );

          // Comment in the middle
          EXPECT_EQ( sanitizeString( "KeepThis # RemoveThis" ), "KeepThis" );
      } ) );
#endif

#if 0
TEST( getApplicationDirectoryAbsolutePath, ( [] {
          auto l_path = filesystem::getApplicationDirectoryAbsolutePath();

          assertNotEqual( l_path, std::nullopt );
      } ) );
#endif

} // namespace stdfunc
