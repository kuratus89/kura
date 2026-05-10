#pragma once
#include "chunk.h"
typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[256];
    Value* stackTop;
}VM;


typedef enum{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
}interpretResult;

void initilizeVM();
void freeVM();
interpretResult interpret(char* source);