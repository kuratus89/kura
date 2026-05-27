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
    OP_STORE,
    OP_DECLARE,
    OP_LOAD_VAR,
    OP_EXIT,
}opcode;

typedef struct{
    bool isValue;
    int child[63];
    int value;
    dataType type;
}varMap;

typedef struct {
    int count;
    int capacity;
    varMap* maps;
}varMaps;

typedef struct{
    dataType type;
    int index;
}parameter;

typedef struct {
    int count;
    int capacity;
    int* lines;
    uint8_t* code;
    valueArray constants;
    int varCount;
    varMaps vars;
    dataType returnType;
    parameter* paras;
    int paraCount;
    int paraCapacity;
}Chunk;
typedef struct funcByte{
    int count;
    Chunk* func;
    Chunk global;
    int funcCount;
    int funcCapacity;
    
}funcByte;

void initilizeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk , uint8_t byte , int line);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk , void* value , dataType type);