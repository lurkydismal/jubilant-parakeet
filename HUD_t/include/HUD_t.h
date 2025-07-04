#pragma once

#include <stdbool.h>

#include "object_t.h"
#include "player_t.h"
#include "watch_t.h"

#define DEFAULT_HUD                      \
    { .guardBars = NULL,                 \
      .brokenGuardBars = NULL,           \
      .guardGauges = NULL,               \
      .hpBars = NULL,                    \
      .restorableHpBars = NULL,          \
      .hpGauges = NULL,                  \
      .logos = NULL,                     \
      .names = NULL,                     \
      .meterBars = NULL,                 \
      .meterGauges = NULL,               \
      .timer = DEFAULT_OBJECT,           \
      .timerBackground = DEFAULT_OBJECT, \
      .isGuardBroken = false,            \
      .name = NULL,                      \
      .folder = NULL,                    \
      .extension = NULL,                 \
      .players = NULL }

typedef struct {
    object_t** guardBars;
    object_t** brokenGuardBars;
    object_t** guardGauges;
    object_t** hpBars;
    object_t** restorableHpBars;
    object_t** hpGauges;
    object_t** logos;
    object_t** names;
    object_t** meterBars;
    object_t** meterGauges;
    object_t timerBackground;
    object_t timer;

    bool isGuardBroken;

    char* name;
    char* folder;
    char* extension;
    player_t** players;

#if defined( DEBUG )

    watch_t** watches;

#endif
} HUD_t;

HUD_t HUD_t$create( void );
bool HUD_t$destroy( HUD_t* restrict _HUD );

bool HUD_t$load( HUD_t* restrict _HUD, SDL_Renderer* _renderer );
bool HUD_t$unload( HUD_t* restrict _HUD );

bool HUD_t$step( HUD_t* restrict _HUD );
bool HUD_t$render( const HUD_t* restrict _HUD );
