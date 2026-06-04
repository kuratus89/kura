
#include "chunk.h"
#include "memory.h"
#include "compiler.h"
#include "value.h"
void initilizeChunk(Chunk* chunk){
    chunk->count = 0;
    chunk-> capacity = 0;
    chunk->varCount = 0;
    chunk-> code = NULL;
    chunk-> lines = NULL;
    chunk->paraCapacity =0;
    chunk->paraCount =0;
    chunk->paras = NULL;
    chunk->returnType = DATA_VOID;
    chunk->flags.flagCapacity = 0;
    chunk->flags.flagCount = 0;
    chunk->flags.flag = NULL;
    initilizeValueArray(&chunk->constants);
    iniVarMaps(&chunk->vars);
    
}

void writeChunk(Chunk* chunk , uint8_t byte , int line){
    if(chunk->capacity - chunk->count == 0){
            int oldCap = chunk->capacity;
            chunk-> capacity = growCapacity(oldCap);
            chunk->code = growArray(uint8_t , chunk->code , oldCap , chunk->capacity);
            chunk->lines = growArray(int , chunk->lines , oldCap ,chunk->capacity);
    }
    chunk->code[chunk->count]= byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void freeChunk(Chunk* chunk){
    freeArray(uint8_t , chunk->code , chunk->capacity);
    freeValueArray(&chunk->constants);
    freeArray(int , chunk->lines , chunk-> capacity);
    initilizeChunk(chunk);
}

void freeWasteChunkCode(Chunk* chunk){
    
}



int addConstant(Chunk* chunk , void* value , dataType type){
    Value val;
    iniValue(&val , type);
    writeValue(&val , value);
    return writeValueArray(&chunk->constants , &val);
}