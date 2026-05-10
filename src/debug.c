#include "common.h"
#include "chunk.h"
#include "value.h"
#include "scanner.h"
#include <stdio.h>

int printInstructer(const char* instruct , int offset){
    printf("%s\n" , instruct);
    return offset+1;
}


int constantInstruction(const char* name , Chunk* chunk , int offset){

    uint8_t constant = chunk->code[offset +1];
    printf("%-16s %4d '" , name , constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset+2;
}

int disassembleInstruction(Chunk* chunk , int offset){
    printf("%04d %d-> " , offset , chunk->lines[offset]);

    uint8_t inst = chunk->code[offset];
    switch(inst){
        case OP_RETURN: return printInstructer("OP_RETURN" , offset);
        case OP_CONSTANT: return constantInstruction("OP_CONSTANT" , chunk , offset);
        case OP_NEGATE : return printInstructer("OP_NEGATE" , offset);
        default : printf("Unknow opCode %d\n",inst);
    }
    return offset+1;

}

void disassembleChunk(Chunk* chunk , const char* name){
    printf("<=== %s ===>\nCount -> %d\nCapacity -> %d\n\n" , name , chunk->count , chunk->capacity);
    for(int offset=0 ; offset< chunk->count;)offset = disassembleInstruction(chunk , offset);
}

void printScan(scanPtr* scan){
    printf("line:%d " , (*scan).line);
    for(char* it =(*scan).start ;it!=(*scan).end ; it++)printf("%c" , (*it));
    printf("\n");
}

void disassembleScan(scar* fs ){
    int cnt = (*fs).count;
    printf("%d scans found!\n" , cnt);
    for(int i=0 ; i<cnt ; i++){
        printScan(((*fs).pointers+i));
    }
}