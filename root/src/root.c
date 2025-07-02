#include <stdio.h>

__attribute__( ( constructor ) ) static void hotReload$load( void ) {
    printf( ">>> Loaded\n" );
}

__attribute__( ( destructor ) ) static void hotReload$unload( void ) {
    printf( "<<< Unloaded\n" );
}
