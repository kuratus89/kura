#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc , const char* argv[]){
    chunk cnk;
    initilizeChunk(&cnk);
    writeChunk(&cnk , OP_RETURN);
    disassembleChunk(&cnk , "First run");
    freeChunk(&cnk);
    return 0;
}