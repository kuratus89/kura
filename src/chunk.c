
#include "chunk.h"
#include "memory.h"
void initilizeChunk(chunk* cnk){
    cnk->count = 0;
    cnk-> capacity = 0;
    cnk-> code = NULL;
}

void writeChunk(chunk* cnk , uint8_t byte){
    if(cnk->capacity - cnk->count == 0){
            int oldCap = cnk->capacity;
            cnk-> capacity = growCapacity(oldCap);
            cnk->code = growArray(uint8_t , cnk->code , oldCap , cnk->capacity);
    }
    cnk->code[cnk->count]= byte;
    cnk->count++;
}

void freeChunk(chunk* cnk){
    freeArray(uint8_t , cnk , cnk->capacity);
    initilizeChunk(cnk);
}
