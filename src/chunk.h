#pragma once
#include "common.h"

typedef enum {
    OP_RETURN,
}opcode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
}chunk;