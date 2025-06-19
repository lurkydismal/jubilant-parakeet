#pragma once

#include <stdbool.h>

#include "object_t.h"

#define DEFAULT_HUD                      \
    { .logos = NULL,                     \
      .hpGauges = NULL,                  \
      .hpBars = NULL,                    \
      .names = NULL,                     \
      .meterGauges = NULL,               \
      .meterBars = NULL,                 \
      .timer = DEFAULT_OBJECT,           \
      .timerBackground = DEFAULT_OBJECT, \
      .name = NULL,                      \
      .folder = NULL,                    \
      .extension = NULL,                 \
      .playerAmount = 0 }

typedef struct {
    object_t** logos;
    object_t** hpGauges;
    object_t** hpBars;
    object_t** names;
    object_t** meterGauges;
    object_t** meterBars;
    object_t timer;
    object_t timerBackground;
    char* name;
    char* folder;
    char* extension;
    size_t playerAmount;
} HUD_t;

HUD_t HUD_t$create( void );
bool HUD_t$destroy( HUD_t* restrict _HUD );

bool HUD_t$load( HUD_t* restrict _HUD, SDL_Renderer* _renderer );
bool HUD_t$unload( HUD_t* restrict _HUD );

bool HUD_t$step( HUD_t* restrict _HUD );
bool HUD_t$render( const HUD_t* restrict _HUD );
