
#include "chunk.h"
#include "memory.h"
#include "token.h"

void iniFuncByte(funcByte* func , int count){
    func->count = 0;
    func->func = growArray(Chunk , NULL , 0 , count);
    initilizeChunk(&func->global);
}



void compileGlobal(Tokens* global , funcByte*){
    for(int i=0 ; i<global->count ; i++){
        Token* it = global->token+i;
    }
}

void compile(tokenFunctions* tf , funcByte* func){
    iniFuncByte(func , tf->count);

}