#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "object_t.h"
#include "watch_t.h"

#define DEFAULT_BACKGROUND        \
    {                             \
        .object = DEFAULT_OBJECT, \
        .name = NULL,             \
        .folder = NULL,           \
        .extension = NULL,        \
    }

typedef struct {
    object_t object;

    char* name;
    char* folder;
    char* extension;

#if defined( DEBUG )

    watch_t** watches;

#endif
} background_t;

background_t background_t$create( void );
bool background_t$destroy( background_t* restrict _background );

bool background_t$load( background_t* restrict _background,
                        SDL_Renderer* _renderer );
bool background_t$unload( background_t* restrict _background );

bool background_t$step( background_t* restrict _background );
bool background_t$render( const background_t* restrict _background,
                          const SDL_FRect* restrict _cameraRectangle,
                          bool _doDrawBoxes );
