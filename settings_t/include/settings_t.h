#pragma once

#include <stdbool.h>

#include "asset_t.h"
#include "controls_t.h"
#include "window_t.h"

#define DEFAULT_SETTINGS_VERSION "0.1"

#define DEFAULT_SETTINGS            \
    { .window = DEFAULT_WINDOW,     \
      .controls = DEFAULT_CONTROLS, \
      .version = NULL,              \
      .identifier = NULL }

// All available customization
typedef struct {
    window_t window;
    controls_t controls;
    char* version;
    char* identifier;
} settings_t;

settings_t settings_t$create( void );
bool settings_t$destroy( settings_t* restrict _settings );

bool settings_t$load$fromAsset( settings_t* restrict _settings,
                                const asset_t* restrict _asset );
bool settings_t$load$fromPath( settings_t* restrict _settings,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension );
bool settings_t$unload( settings_t* restrict _settings );
