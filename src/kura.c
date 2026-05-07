#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* readFile(const char* path){
    FILE* file = fopen(path , "rb");
    if(file==NULL){
        printf("Fail to open file \"%s\" \n" , path);
        fclose(file);
        exit(9);
    }
    fseek(file , 0L , SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if(buffer==NULL){
        printf("Fail to allocate memory to read file \"%s\" \n" , path);
        fclose(file);
        exit(99);
    }
    size_t bytesRead = fread(buffer , sizeof(char) , fileSize , file);
    buffer[bytesRead] = '\0';
    fclose(file);

    if(bytesRead<fileSize){
        fprint("Fail to read file \"%s\" \n" , path);
        exit(999);
    }
    return buffer;
}


int main(int argc , const char* argv[]){
    initilizeVM();
    Chunk chunk;
    initilizeChunk(&chunk);
    int constant = addConstant(&chunk , 1.2);
    writeChunk(&chunk , OP_CONSTANT , 123);
    writeChunk(&chunk , constant , 123);
    writeChunk(&chunk, OP_RETURN , 123);

    interpret(&chunk);
    freeChunk(&chunk);
    freeVM();
    return 0;
}