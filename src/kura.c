#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "scanner.h"
#include "token.h"
#include "vm.h"
#include "compiler.h"
#include <string.h>

interpretResult interpret(char* source){
    scar scan; 
    scanToPtr(source , &scan);
    Tokens tokens;
    tokenize(&scan , &tokens);
    freeScan(&scan);
    tokenFunctions tf;
    functinize(&tokens , &tf);
    funcByte func;
    compile(&tf ,&func );
    iniVM(&func.global);
    return (run());
}

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
        printf("Fail to read file \"%s\" \n" , path);
        exit(999);
    }
    return buffer;
}

static void runFile(const char* path){
    char* source = readFile(path);
    interpretResult result = interpret(source);
    free(source);
    
    if(result==INTERPRET_COMPILE_ERROR)exit(1);
    if(result==INTERPRET_RUNTIME_ERROR)exit(11);
}

int main(int argc , const char* argv[]){
    if(argc==1){
        printf("file path not found");
        exit(87);
    }
    runFile(argv[1]);
    printf("success!");
    return 0;
}