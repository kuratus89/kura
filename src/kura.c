#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc , const char* argv[]){
    Chunk chunk;
    initilizeChunk(&chunk);
    int constant = addConstant(&chunk , 1.2);
    writeChunk(&chunk , constant , 123);
    writeChunk(&chunk , OP_CONSTANT , 123);
    
    writeChunk(&chunk, OP_RETURN , 123);
    disassembleChunk(&chunk , "First run");
    freeChunk(&chunk);
    return 0;
}