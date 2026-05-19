#pragma once
#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_LOAD_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_FLAG,
    OP_GOTO,
}opcode;

typedef struct {
    int count;
    int capacity;
    int* lines;
    uint8_t* code;
    valueArray constants;
}Chunk;
typedef struct{
    int count;
    Chunk* func;
    Chunk global;
}funcByte;

void initilizeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk , uint8_t byte , int line);
void freeChunk(Chunk* chunk);
void addConstant(Chunk* chunk , void* value , dataType type);