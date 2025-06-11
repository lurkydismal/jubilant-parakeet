#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_ASSET { .data = NULL, .size = 0 }

typedef struct {
    uint8_t* data;
    size_t size;
} asset_t;

bool asset_t$loader$init( const char* restrict _assetsDirectory );
bool asset_t$loader$quit( void );

asset_t asset_t$create( void );
bool asset_t$destroy( asset_t* restrict _asset );

bool asset_t$load( asset_t* restrict _asset, const char* restrict _path );
bool asset_t$load$compressed( asset_t* restrict _asset,
                              const char* restrict _path );
bool asset_t$unload( asset_t* restrict _asset );

bool asset_t$compress( asset_t* restrict _asset );
bool asset_t$uncompress( asset_t* restrict _asset );

bool asset_t$save$sync$toPath( asset_t* restrict _asset,
                               const char* restrict _path,
                               const bool _needTruncate );

// TODO: Implement
bool asset_t$save$async$toPath$submit( asset_t* restrict _asset,
                                       const char* restrict _path,
                                       const bool _needTruncate );

const char* asset_t$loader$assetsDirectory$get( void );
