#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>

#define COLOR_BLACK \
    { .red = 0, .green = 0, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_WHITE \
    { .red = 0xFF, .green = 0xFF, .blue = 0xFF, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_RED \
    { .red = 0xFF, .green = 0, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_GREEN \
    { .red = 0, .green = 0xFF, .blue = 0, .alpha = SDL_ALPHA_OPAQUE }
#define COLOR_BLUE \
    { .red = 0, .green = 0, .blue = 0xFF, .alpha = SDL_ALPHA_OPAQUE }
#define DEFAULT_COLOR COLOR_WHITE

typedef struct {
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint8 alpha;
} color_t;

color_t color_t$fromString( const char* _string, Uint16 _base );
