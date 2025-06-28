#include <stdio.h>

__attribute__( ( constructor ) ) static void on_load( void ) {
    printf( ">>> Loaded!\n" );
}

__attribute__( ( destructor ) ) static void on_unload( void ) {
    printf( "<<< Unloaded!\n" );
}
