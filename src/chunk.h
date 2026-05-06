#pragma once
#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
}opcode;

typedef struct {
    int count;
    int capacity;
    int* lines;
    uint8_t* code;
    valueArray constants;
}Chunk;

void initilizeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk , uint8_t byte , int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk , Value value);