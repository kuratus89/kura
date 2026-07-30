#include "common.h"
#include "memory.h"
#include "datastruct.h"
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
    memory.stack = malloc(MEMORY_STACK_MAX);
    memory.values=NULL;
    memory.valuesCapacity =0;

    memory.runtime = malloc(RUNTIME_STACK_MEMORY_MAX);
    memory.runtimeCount = 0;
    memory.runtimeValues = NULL;
    memory.runtimeValuesCount = 0;
    memory.runtimeValuesCapacity=0;

    memory.heaps = NULL;
    memory.heapsCapacity=0;
}

static inline void* allocateStackMemory(size_t x ){
    if(memory.stackCount+x >= MEMORY_STACK_MAX){
        printf("memory overflow");
        exit(19);
    }
    memory.stackCount+=x;
    return(memory.stack+memory.stackCount-x);
}
static inline void* allocateRuntimeMemory(size_t size){
    if(memory.runtimeCount+size>= RUNTIME_STACK_MEMORY_MAX){
        printf("Runtime stack overflow");
        exit(29);
    }
    memory.runtimeCount+=size;
    return(memory.runtime + memory.runtimeCount - size);
}



inline Value* allocateStackMemoryValueIndex(size_t x , int index){
    if(index>= memory.valuesCapacity){
        int oldCap = memory.valuesCapacity;
        memory.valuesCapacity = growCapacity(index);
        memory.values = growArray(Value , memory.values , oldCap , memory.valuesCapacity);
    }
    memory.values[index].value = allocateStackMemory(x);
    return memory.values+index;
}

inline Value* getStackMemoryValueIndex(int index){
    return memory.values+index;
}

inline void writeStackMemoryValueIndex(Value* value ,  int index){
    if(index>=memory.valuesCapacity)return;
    Value* x = getStackMemoryValueIndex(index);
    copyValue(value , x);
}

inline void unloadStackMemoryValueIndex(int index){
    memory.stackCount = (memory.values+index)->value - memory.stack;
}

inline Value* pushNewValueRuntimeStack(size_t size){
    if(memory.runtimeValuesCount>= memory.runtimeValuesCapacity){
        int oldCap = memory.runtimeValuesCapacity;
        memory.runtimeValuesCapacity = growCapacity(oldCap);
        memory.runtimeValues = growArray(Value , memory.runtimeValues , oldCap , memory.runtimeValuesCapacity);
    }

    memory.runtimeValues[memory.runtimeValuesCount].value = allocateRuntimeMemory(size);
    memory.runtimeValuesCount++;
    return memory.runtimeValues+ memory.runtimeValuesCount-1;
}

inline int getRuntimeStackSize(){
    return memory.runtimeValuesCount;
}

inline Value* popRuntimeStack(){
    memory.runtimeCount = memory.runtimeValues[memory.runtimeValuesCount-1].value - memory.runtime;
    memory.runtimeValuesCount--;
    return memory.runtimeValues + memory.runtimeValuesCount;
}
inline Value* getRuntimeStack(int offset){
    return memory.runtimeValues + memory.runtimeValuesCount -1 - offset;
}

heapInterval* pushInterval(heapBlock* heap , int size , int start){
    if(heap->intervalsCount==heap->intervalsCapacity){
        int oldCap = heap->intervalsCapacity;
        heap->intervalsCapacity = growCapacity(oldCap * 2);
        heap->intervals = growArray(heapInterval , heap->intervals , oldCap , heap->intervalsCapacity);
    }
    int ip;
    if(heap->recycleIntervalCount){
        heap->recycleIntervalCount--;
        ip = *(heap->recycleInterval + heap->recycleIntervalCount);
    }
    else {
        ip = heap->intervalsCount;
        heap->intervalsCount++;
    }
    pushRbtNode(ip ,size , &heap->intervalRbt);
    (heap->intervals + ip)->size = size;
    (heap->intervals + ip)->start = start;
    return heap->intervals + ip;
}

inline void iniHeapBlock(heapBlock* heap ,size_t size){

}

inline int addNewHeapBlock(size_t size){
    if(memory.heapsCount==memory.heapsCapacity){
        int oldCap = memory.heapsCapacity;
        memory.heapsCapacity =growCapacity(oldCap);
        memory.heaps = growArray(heapBlock , memory.heaps , oldCap , memory.heapsCapacity);
    }
    iniHeapBlock(memory.heaps+ memory.heapsCount ,size);
    pushRbtNode(memory.heapsCount , size , &memory.heapRbt);
    memory.heapsCount++;
    return memory.heapsCount-1;
}


inline int findHeapBlock(size_t size){
    rbtNode* node = lowerBoundRbt(&memory.heapRbt , size);
    if(node == NULL)return addNewHeapBlock(size);
    else return getTopNodeValue(node);
}

void deleteInterval(heapBlock* heap , int interval){
    if(heap->recycleIntervalCount==heap->recycleIntervalCapacity){
        int oldCap = heap->recycleIntervalCapacity;
        heap->recycleIntervalCapacity = growCapacity(oldCap);
        heap->recycleInterval = growArray(int , heap->recycleInterval , oldCap , heap->recycleIntervalCapacity);
    }
    *(heap->recycleInterval + heap->recycleIntervalCount) = interval;
    heap->recycleIntervalCount++;
    deleteRbtNodeValue(&heap->intervalRbt , interval);
}


void* allocateHeapMemory(size_t size){
    int totalSize = size + headerSize + footerSize;
    heapBlock* heap =memory.heaps+findHeapBlock(totalSize);
    rbtNode* node = lowerBoundRbt(&heap->intervalRbt , totalSize);
    int value = getTopNodeValue(node);
    heapInterval* interval = heap->intervals + value;
    int intervalStart = interval->start;
    int intervalSize = interval->size;
    deleteInterval(heap , interval - heap->intervals);

    // if(intervalSize>totalSize)pushInterval(heap , intervalSize - totalSize , intervalStart + totalSize+1);

    *(int*)((uint8_t*)heap->memory + intervalStart) = -1; // link to previous free memory
    *(int*)(int*)((uint8_t*)heap->memory + intervalStart + sizeof(int)) = heap - memory.heaps; //heap block address
    *(int*)((uint8_t*)heap->memory + intervalStart + 2*sizeof(int)) = intervalStart; // memory address
    *(int*)((uint8_t*)heap->memory + intervalStart + 3*sizeof(int)) = totalSize; // memory size
    
    // *((uint8_t*)heap->memory + intervalStart + headerSize + size +1) = (int)-1; //link to next free memory

    if(intervalStart - sizeof(int) >=0)*((uint8_t*)heap->memory + intervalStart - sizeof(int)) = (int)-1; // removing link from previous memory , as its not a free memory anymore
    if(intervalStart + totalSize + sizeof(int) < heap->size)*((uint8_t*)heap->memory + intervalStart + totalSize + 1) = (int)-1;//removing link from next memory , as its not a free memory anymore

    if(intervalSize>totalSize){
        heapInterval* nextMemory = pushInterval(heap , intervalSize - totalSize , intervalStart + totalSize +1);
        *(int*)((uint8_t*)heap->memory + intervalStart + headerSize +size +1 )= nextMemory - heap->intervals;
    }
    else *(int*)((uint8_t*)heap->memory + intervalStart + headerSize + size +1) = -1;

    return (uint8_t*)heap->memory + intervalStart + headerSize;
}

void freeHeapMemory(void* mem){

}

