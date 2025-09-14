#include "test.hpp"

#if defined( TESTS )

auto main( int _argumentCount, char** _argumentsVector ) -> int {
    ::testing::InitGoogleTest( &_argumentCount, _argumentsVector );

    return ( RUN_ALL_TESTS() );
}

#endif
