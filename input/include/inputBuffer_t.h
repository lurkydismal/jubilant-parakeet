#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "input_t.h"

#define INPUT_BUFFER_LENGTH ( 100 )

#define MAX_DELAY_BETWEEN_INPUTS ( 8 )

#define DEFAULT_INPUT_BUFFER   \
    { .inputs = { 0 },         \
      .frames = { 0 },         \
      .currentBufferIndex = 0, \
      .previousBufferIndex = 0 }

typedef struct {
    input_t inputs[ INPUT_BUFFER_LENGTH ];
    size_t frames[ INPUT_BUFFER_LENGTH ];
    size_t currentBufferIndex;
    size_t previousBufferIndex;
} inputBuffer_t;

inputBuffer_t inputBuffer_t$create( void );
bool inputBuffer_t$destroy( inputBuffer_t* _inputBuffer );

input_t inputBuffer_t$inputsSequence$getInput$last(
    inputBuffer_t* _inputBuffer );
size_t inputBuffer_t$inputsSequence$getFrame$last(
    inputBuffer_t* _inputBuffer );

bool inputBuffer_t$insert( inputBuffer_t* _inputBuffer,
                           input_t _input,
                           size_t _frame );

input_t** inputBuffer_t$inputsSequence$get( inputBuffer_t* _inputBuffer,
                                            const size_t _currentFrame );
input_t** inputBuffer_t$inputsSequence$get$withLimit(
    inputBuffer_t* _inputBuffer,
    const size_t _currentFrame,
    const size_t _limitAmount );
