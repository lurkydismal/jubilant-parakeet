#include "stdfunc.hpp"

#include <ctll/fixed_string.hpp>

#include <ranges>

#include "test.hpp"

namespace stdfunc {

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

#if 0
TEST( sanitizeString, ( [] {
          // Normal case with spaces and a comment
          assertEqual( sanitizeString( ctll::fixed_string(
                           "  Hello  World   # Comment here" ) ),
                       "HelloWorld" );

          // Leading and trailing spaces
          assertEqual( sanitizeString( ctll::fixed_string( "   test   " ) ),
                       "test" );

          // No spaces, no comments
          assertEqual(
              sanitizeString( ctll::fixed_string( "NoSpacesOrComments" ) ),
              "NoSpacesOrComments" );

          // Only spaces
          assertEqual( sanitizeString( ctll::fixed_string( "       " ) ), "" );

          // Only comment
          assertEqual(
              sanitizeString( ctll::fixed_string( "   # This is a comment" ) ),
              "" );

          // Empty string
          assertEqual( sanitizeString( "" ), "" );

          // Comment in the middle
          assertEqual(
              sanitizeString( ctll::fixed_string( "KeepThis # RemoveThis" ) ),
              "KeepThis" );
      } ) );
#endif

TEST( getApplicationDirectoryAbsolutePath, ( [] {
          auto l_path = filesystem::getApplicationDirectoryAbsolutePath();

          assertNotEqual( l_path, std::nullopt );
      } ) );

} // namespace stdfunc
