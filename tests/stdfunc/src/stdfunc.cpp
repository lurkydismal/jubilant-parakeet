#include "stdfunc.hpp"

#include "test.hpp"

// TODO: Add bitsToBytes, isSpace,
// random$numberMinMax, random$value, random$view, random$fillMinMax,
// random$fill, makeVariantContainer, compress$text, compress$data,
// decompress$text, decompress$data, meta$iterateStructUnionTopMostFields
namespace stdfunc {

#if 0
TEST( STRINGIFY, ( [] {
          assertEqual( STRINGIFY( Tessst ), "Tessst" );
          assertEqual( STRINGIFY( stdfunc::color::g_green ),
                       "stdfunc::color::g_green" );
      } ) );
#endif

#if 0
TEST( MACRO_TO_STRING, ( [] {
#define y 123
#define x y
#define z() y
          assertEqual( MACRO_TO_STRING( x ), "123" );
          assertEqual( MACRO_TO_STRING( y ), "123" );
          assertEqual( MACRO_TO_STRING( z ), "123" );

#undef z
#undef x
#undef y
      } ) );
#endif

#if 0
TEST( _b, ( [] {
          assertEqual( 'A'_b, 0xA );
          assertEqual( 'a'_b, 0xA );
          assertEqual( 'G'_b, 0 );

          assertEqual( 0_b, 0 );
          assertEqual( 10_b, 0xA );
          assertEqual( 17_b, 0x11 );
      } ) );
#endif

#if 0
TEST( _bytes, ( [] {
          std::array l_array = { std::byte{ 0xF }, std::byte{ 0xF },
                                 std::byte{ 0xF } };

          assertEqual( "FFF"_bytes, l_array );
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

          assertEqual( l_actualLengthFailed, l_expectedLengthFailed );
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

                      assertEqual( l_buffer.size(), l_bufferLength );

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
          assertEqual( sanitizeString( "  Hello  World   # Comment here" ),
                       "HelloWorld" );

          // Leading and trailing spaces
          assertEqual( sanitizeString( "   test   " ), "test" );

          // No spaces, no comments
          assertEqual( sanitizeString( "NoSpacesOrComments" ),
                       "NoSpacesOrComments" );

          // Only spaces
          assertEqual( sanitizeString( "       " ), "" );

          // Only comment
          assertEqual( sanitizeString( "   # This is a comment" ), "" );

          // Empty string
          assertEqual( sanitizeString( "" ), "" );

          // Comment in the middle
          assertEqual( sanitizeString( "KeepThis # RemoveThis" ), "KeepThis" );
      } ) );
#endif

#if 0
TEST( getApplicationDirectoryAbsolutePath, ( [] {
          auto l_path = filesystem::getApplicationDirectoryAbsolutePath();

          assertNotEqual( l_path, std::nullopt );
      } ) );
#endif

} // namespace stdfunc
