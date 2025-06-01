#pragma once

#include "input_t.h"

#include <stdbool.h>

#define INPUT_BUFFER_SIZE (100)

#define DEFAULT_INPUT_BUFFER { .inputs = NULL, .frames = NULL, .currentBufferIndex = 0 }

typedef struct {
    input_t inputs[INPUT_BUFFER_SIZE];
    size_t frames[INPUT_BUFFER_SIZE];
    uint8_t currentBufferIndex;
} inputBuffer_t;

inputBuffer_t inputBuffer_t$create(void);
bool inputBuffer_t$destroy( inputBuffer_t* _inputBuffer );

bool inputBuffer_t$insert( inputBuffer_t* _inputBuffer, input_t _input, size_t _frame );
