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

void initilizeChunk(chunk* cnk);
void writeChunk(chunk* cnk , uint8_t byte);
void freeChunk(chunk* cnk);