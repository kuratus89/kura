#pragma once
#include "chunk.h"

# define FEED_MAX 256
# define STACK_MAX 256
# define VAR_STACK_MAX 256

// typedef struct {
//     Chunk* chunk;
//     uint8_t* ip;
//     Value stack[256];
//     Value* stackTop;
// }VM;


typedef struct{
    Chunk* chunk;
    uint8_t* ip;
    int varsCount;
}Feeder;

typedef struct{
    Feeder feeds[FEED_MAX];
    // Value stack[STACK_MAX];
    Feeder* currentFunc;
    // Value* stackTop;
    int runCnt;
    int varFuncCount;
    Chunk* Functions;
}VM;




typedef enum{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
}interpretResult;

void iniVM(Chunk* chunk , Chunk* func);
interpretResult run();