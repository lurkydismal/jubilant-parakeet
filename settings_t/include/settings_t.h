#pragma once

#include <stdbool.h>

#include "asset_t.h"
#include "window_t.h"

#define DEFAULT_SETTINGS \
    { .window = DEFAULT_WINDOW, .limitedLoopDesiredFPS = 60 }

// All available customization
typedef struct {
    window_t window;
    size_t limitedLoopDesiredFPS;
} settings_t;

settings_t settings_t$create( void );
bool settings_t$destroy( settings_t* restrict _settings );

bool settings_t$load$fromAsset( settings_t* restrict _settings,
                                const asset_t* restrict _asset );
bool settings_t$load$fromPath( settings_t* restrict _settings,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension );
bool settings_t$unload( settings_t* restrict _settings );
