#pragma once

#include <stdbool.h>

#include "asset_t.h"

#define DEFAULT_CONFIG \
    {                  \
    }

typedef struct {
    int x;
} config_t;

config_t config_t$create();
bool config_t$destroy( config_t* _config );

bool config_t$load$fromString( config_t* _config, const char* _string );
bool config_t$load$fromAsset( config_t* _config, asset_t* _asset );
bool config_t$load$fromPath( config_t* _config, const char* _path );
bool config_t$unload( config_t* _config );
