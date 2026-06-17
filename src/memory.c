#include "common.h"
#include "memory.h"
Memory memory;
int growCapacity(int oldCap){
    if(oldCap==0)return 8;
    return (oldCap)*2;
}

void* reallocate(void* pointer , size_t old_Size ,size_t new_Size){
    if(new_Size==0){
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer , new_Size);
    if(result==NULL)exit(1);
    return result;
}

int* find(int* start, int value ,int length , bool isSort){
    int* end = start + (length);
    if(!isSort){
        for(int* it = start; it!=end ; it++)if(*it==value)return it;
        return NULL;
    }

    int low =0;
    int high = length-1;
    int mid;
    while(low<=high){
        mid = (low+high)/2;
        int* it = start+mid;
        if(value==*it)return it;
        if(value<*it)high = mid-1;
        else low = mid+1;
    }
    return NULL;    
}

void iniMemory(){
    memory.stackCount  = 0;
    memory.stackValueDataCount = 0;
    memory.stackValueDataCapacity = 0;
    memory.stackValueData = NULL;
    memory.stack = malloc(MEMORY_STACK_MAX);
}

// void addElementSize(int x){
//     if(memory.stackElementCount == memory.stackElementCapacity){
//         int oldCap = memory.stackElementCapacity;
//         memory.stackElementCapacity = growCapacity(oldCap);
//         memory.stackElementSize = growArray(int , memory.stackElementSize , oldCap , memory.stackElementCapacity);
//     }
//     if(memory.stackElementCount==0)memory.stackElementSize[memory.stackElementCount] = x;
//     else memory.stackElementSize[memory.stackElementCount] = x + memory.stackElementSize[memory.stackElementCount -1];
//     memory.stackElementCount++;

// }
void addValueData(int x ,  int offset){
    if(memory.stackValueDataCount==memory.stackValueDataCapacity){
        int oldCap = memory.stackValueDataCapacity;
        memory.stackValueDataCapacity = growCapacity(oldCap);
        memory.stackValueData = growArray(valueData , memory.stackValueData , oldCap , memory.stackValueDataCapacity);
    }
    (memory.stackValueData+memory.stackValueDataCount)->stackElementSize = x+offset;
    (memory.stackValueData+memory.stackValueDataCount)->value.size = x;
        
}

int topStackSize(){
    if(memory.stackElementCount == 0) return -1;
    if(memory.stackElementCount==1)return memory.stackElementSize[0];
    else return memory.stackElementSize[memory.stackElementCount-1 ] - memory.stackElementSize[memory.stackElementCount - 2];
}


void* allocateStackMemory(int size){
    addElementSize(size);
    memory.stackCount+=size;
    if(memory.stackCount>= MEMORY_STACK_MAX){
        printf("memory stack overFlow");
        exit(909);
    }
    return memory.stack+ memory.stackCount - size;
}

void freeStack(int index){
    int it;
    if(index==0)it =0;
    else it = memory.stackElementSize[index-1];
    memory.stackCount = it;
    memory.stackElementCount = index;
}

void* getStackValue(int index){
    int it;
    if(index==0)it = 0;
    else it = memory.stackElementSize[index-1];
    return memory.stack+it;
}

