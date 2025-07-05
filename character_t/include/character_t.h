#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "object_t.h"
#include "watch_t.h"

#define DEFAULT_CHARACTER              \
    {                                  \
        .movesObject = DEFAULT_OBJECT, \
        .moveNames = NULL,             \
        .moves = NULL,                 \
        .name = NULL,                  \
        .folder = NULL,                \
        .extension = NULL,             \
    }

typedef struct {
    int x;
} move_t;

typedef struct {
    object_t movesObject;

    char** moveNames;
    move_t** moves;

    char* name;
    char* folder;
    char* extension;

#if defined( DEBUG )

    watch_t** watches;

#endif
} character_t;

character_t character_t$create( void );
bool character_t$destroy( character_t* restrict _character );

bool character_t$load( character_t* restrict _character,
                       SDL_Renderer* _renderer );
bool character_t$unload( character_t* restrict _character );

bool character_t$step( character_t* restrict _character );
bool character_t$render( const character_t* restrict _character,
                         const SDL_FRect* restrict _cameraRectangle,
                         bool _doDrawBoxes );
