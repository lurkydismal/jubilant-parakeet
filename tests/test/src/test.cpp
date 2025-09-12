#include "test.hpp"

namespace {

auto g_array1 =
    stdfunc::makeVariantContainer< std::array >( true,
                                                 std::string_view( " true " ),
                                                 123,
                                                 "",
                                                 'a' );
auto g_array2 =
    stdfunc::makeVariantContainer< std::array >( false,
                                                 std::string_view( " false " ),
                                                 321,
                                                 " ",
                                                 'b' );

} // namespace

namespace test {

TEST( assertTrue, ( [] {
          assertTrue( true );
          assertTrue( 1 );
      } ) );

TEST( assertFalse, ( [] {
          assertFalse( false );
          assertFalse( 0 );
      } ) );

TEST( assertEqual, ( [] {
          for ( const auto& _element : g_array1 ) {
              std::visit(
                  []( auto&& _value ) { assertEqual( _value, _value ); },
                  _element );
          }
      } ) );

TEST( assertNotEqual, ( [] {
          for ( const auto& [ _element1, _element2 ] :
                std::views::zip( g_array1, g_array2 ) ) {
              std::visit(
                  []( auto&& _value1, auto&& _value2 ) {
                      assertNotEqual( _value1, _value2 );
                  },
                  _element1, _element2 );
          }
      } ) );

} // namespace test
