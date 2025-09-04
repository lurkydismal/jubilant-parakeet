#pragma once

#include "texture_t.h"
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>
#include <stdbool.h>

#include "asset_t.h"

#define ASCII_START ( 32 )
#define ASCII_END ( 126 )
#define CHARACTERS_COUNT( _start, _end ) ( ( _end ) - ( ( _start ) - 1 ) )

#define DEFAULT_FONT \
    { .atlas = DEFAULT_TEXTURE,  \
      .height = 0.0, \
      .ascent = 0,   \
      .descent = 0,  \
      .lineGap = 0,  \
        .info = {0}, \
        .glyphs = {0}, \
       }

typedef stbtt_fontinfo fontInfo_t;
typedef stbtt_packedchar glyph_t;

typedef struct {
    texture_t atlas;
    float height;
    size_t ascent;
    size_t descent;
    size_t lineGap;
    fontInfo_t info;
    glyph_t glyphs[ CHARACTERS_COUNT( ASCII_START, ASCII_END ) ];
} font_t;

font_t font_t$create( void );
bool font_t$destroy( font_t* restrict _font );

bool font_t$load$fromAsset( font_t* restrict _font, const asset_t* restrict _asset );
bool font_t$load$fromPath( font_t* restrict _font, const char* restrict _path );
bool font_t$load$fromGlob( font_t* restrict _font, const char* restrict _glob );
bool font_t$unload( font_t* restrict _font );

// TODO: Render on screen
// TODO: Get glyph as texture
