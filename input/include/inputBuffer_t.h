#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "input_t.h"

#define INPUT_BUFFER_LENGTH ( 100 )

#define MAX_DELAY_BETWEEN_INPUTS ( 8 )

#define DEFAULT_INPUT_BUFFER { .currentBufferIndex = 0 }

typedef struct {
    input_t inputs[ INPUT_BUFFER_LENGTH ];
    size_t frames[ INPUT_BUFFER_LENGTH ];
    size_t currentBufferIndex;
} inputBuffer_t;

inputBuffer_t inputBuffer_t$create( void );
bool inputBuffer_t$destroy( inputBuffer_t* _inputBuffer );

bool inputBuffer_t$insert( inputBuffer_t* _inputBuffer,
                           input_t _input,
                           size_t _frame );

input_t** inputBuffer_t$inputsSequence$get( inputBuffer_t* _inputBuffer );
input_t** inputBuffer_t$inputsSequence$get$withLimit(
    inputBuffer_t* _inputBuffer,
    const size_t _limitAmount );
