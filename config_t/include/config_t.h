#pragma once

#include <stdbool.h>

#include "asset_t.h"
#include "background_t.h"

#define DEFAULT_CONFIG \
    {                  \
        .backgrounds = NULL, \
    }

typedef struct {
    background_t** backgrounds;
} config_t;

config_t config_t$create();
bool config_t$destroy( config_t* restrict _config );

bool config_t$load$fromString( config_t* restrict _config, const char* restrict _string );
bool config_t$load$fromAsset( config_t* restrict _config, asset_t* restrict _asset );
bool config_t$load$fromPath( config_t* restrict _config,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension );
bool config_t$unload( config_t* restrict _config );
