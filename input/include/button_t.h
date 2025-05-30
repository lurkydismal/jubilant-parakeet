#pragma once

typedef enum {
    A = ( 1 << 4 ),
    B = ( 1 << 5 ),
    C = ( 1 << 6 ),
    D = ( 1 << 7 ),
    AB = ( A | B ),
    ABC = ( A | B | C )
} button_t;
