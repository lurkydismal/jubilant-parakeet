#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>

#include "input_t.h"
#include "object_t.h"
#include "watch_t.h"

#define DEFAULT_CHARACTER              \
    {                                  \
        .movesObject = DEFAULT_OBJECT, \
        .moveNames = NULL,             \
        .moves = NULL,                 \
        .displayName = NULL,           \
        .isGuardBroken = false,              \
        .healthPointsMax = 0,            \
        .restorableHealthPointsMax = 0,  \
        .guardPointsMax = 0,              \
        .meterPointsMax = 0,               \
        .healthPoints = 0,                \
        .restorableHealthPoints = 0,      \
        .guardPoints = 0,                  \
        .meterPoints = 0,                   \
        .walkSpeed = 0,                \
        .jumpHeight = 0,               \
        .name = NULL,                  \
        .folder = NULL,                \
        .extension = NULL,             \
    }

typedef struct {
    char* type;
    input_t input;
    float* velocity;
    size_t** cancelWindows;
    size_t startup;
    size_t active;
    size_t recovery;
    size_t* cancelInto;
} move_t;

typedef struct {
    object_t movesObject;

    char** moveNames;
    move_t** moves;

    char* displayName;

    bool isGuardBroken;

    size_t healthPointsMax;
    size_t restorableHealthPointsMax;
    size_t guardPointsMax;
    float meterPointsMax;

    size_t healthPoints;
    size_t restorableHealthPoints;
    size_t guardPoints;
    float meterPoints;
    float walkSpeed;
    float jumpHeight;

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
