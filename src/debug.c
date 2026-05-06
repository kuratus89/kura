#include "common.h"
#include "chunk.h"
#include <stdio.h>

void printInstructer(const char* instruct){
    printf("%s\n" , instruct);
}

int disassembleInstruction(chunk* cnk , int offset){
    printf("%04d-> " , offset);

    uint8_t inst = cnk->code[offset];
    switch(inst){
        case OP_RETURN:
            printInstructer("OP_RETURN");
            break;
        default : printf("Unknow opCode %d\n",inst);
    }
    return offset+1;

}

void disassembleChunk(chunk* cnk , const char* name){
    printf("<=== %s ===>\nCount -> %d\nCapacity -> %d\n\n" , name , cnk->count , cnk->capacity);
    for(int offset=0 ; offset< cnk->count;)offset = disassembleInstruction(cnk , offset);
}