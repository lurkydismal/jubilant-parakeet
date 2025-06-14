#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "inputBuffer_t.h"
#include "object_t.h"

#define DEFAULT_PLAYER \
    { .object = DEFAULT_OBJECT, .inputBuffer = DEFAULT_INPUT_BUFFER }

typedef struct {
    object_t object;
    inputBuffer_t inputBuffer;
} player_t;

player_t player_t$create( void );
bool player_t$destroy( player_t* restrict _player );

// fileName_ColorAsHex.extension
// X Y Width Height StartIndex-EndIndex
// After - animation
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

bool player_t$step( player_t* restrict _player,
                    float _velocityX,
                    float _velocityY );
bool player_t$render( const player_t* restrict _player,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes );

bool player_t$input$add( player_t* restrict _player,
                         const input_t _input,
                         const size_t _frame );
input_t** player_t$inputs$get$withLimit( player_t* restrict _player,
                                         const size_t _currentFrame,
                                         const size_t _limitAmount );
