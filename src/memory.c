#include "common.h"
#include "memory.h"
#include "stdlib.h"

int growCapacity(int* oldCap){
    if(*oldCap==0)return 8;
    return (*oldCap)*2;
}



void* reallocate(void* pointer , size_t old_size ,size_t new_size){
    if(new_size==0){
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer , new_size);
    if(result==NULL)exit(1);
    return result;
}
