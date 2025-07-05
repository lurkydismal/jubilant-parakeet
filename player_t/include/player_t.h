#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "character_t.h"
#include "inputBuffer_t.h"

#define DEFAULT_PLAYER                       \
    {                                        \
        .character = DEFAULT_CHARACTER,      \
        .inputBuffer = DEFAULT_INPUT_BUFFER, \
        .isGuardBroken = false,              \
        .healthPointsMax = 10000,            \
        .restorableHealthPointsMax = 10000,  \
        .guardPointsMax = 1000,              \
        .meterPointsMax = 100,               \
        .healthPoints = 4321,                \
        .restorableHealthPoints = 7654,      \
        .guardPoints = 123,                  \
        .meterPoints = 23,                   \
        .name = NULL,                        \
    }

typedef struct {
    character_t character;
    inputBuffer_t inputBuffer;

    bool isGuardBroken;

    size_t healthPointsMax;
    size_t restorableHealthPointsMax;
    size_t guardPointsMax;
    float meterPointsMax;

    size_t healthPoints;
    size_t restorableHealthPoints;
    size_t guardPoints;
    float meterPoints;

    // TODO: Implement
    char* name;
} player_t;

player_t player_t$create( void );
bool player_t$destroy( player_t* restrict _player );

#if 0
// Boxes
// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// Animation
// fileName_WidthxHeight_StartIndex-EndIndex.extension
bool player_t$state$add$fromPaths( player_t* restrict _player,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxesPath,
                                   char* const* restrict _animationPaths,
                                   bool _isActionable,
                                   bool _canLoop );
bool player_t$state$add$fromGlob( player_t* restrict _player,
                                  SDL_Renderer* _renderer,
                                  const char* restrict _boxesGlob,
                                  const char* restrict _animationGlob,
                                  bool _isActionable,
                                  bool _canLoop );
bool player_t$states$remove( player_t* restrict _player );
#endif

bool player_t$load( player_t* restrict _player, SDL_Renderer* _renderer );
bool player_t$unload( player_t* restrict _player );
bool player_t$step( player_t* restrict _player );
bool player_t$render( const player_t* restrict _player,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes );

bool player_t$input$add( player_t* restrict _player,
                         const input_t* _input,
                         const size_t _frame );
input_t** player_t$inputsSequences$get$withLimit( player_t* restrict _player,
                                                  const size_t _currentFrame,
                                                  const size_t _limitAmount );
