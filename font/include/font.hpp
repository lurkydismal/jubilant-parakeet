#pragma once

#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#include <array>
#include <span>

#include "slickdl.hpp"

namespace font {

namespace {

#define ASCII_START ( 32 )
#define ASCII_END ( 126 )
#define CHARACTERS_COUNT( _start, _end ) ( ( _end ) - ( ( _start ) - 1 ) )

} // namespace

using fontInfo_t = stbtt_fontinfo;
using glyph_t = stbtt_packedchar;

using font_t = struct font {
    font() = delete;
    font( const font& ) = default;
    font( font&& ) = default;
    ~font() = default;
    auto operator=( const font& ) -> font& = default;
    auto operator=( font&& ) -> font& = default;

    auto load( std::span< const std::byte > _asset ) -> bool;
    auto fontFromFromRomPath( std::string_view _path ) -> bool;

    slickdl::texture_t atlas;
    float height;
    size_t ascent;
    size_t descent;
    size_t lineGap;
    fontInfo_t info;
    std::array< glyph_t, CHARACTERS_COUNT( ASCII_START, ASCII_END ) > glyphs;
};

// TODO: Render on screen
// TODO: Get glyph as texture

} // namespace font
