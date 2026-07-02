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



inline iniHeapBlock(heapBlock* heap ,size_t size){

}

inline int addNewHeapBlock(size_t size){
    if(memory.heapsCount==memory.heapsCapacity){
        int oldCap = memory.heapsCapacity;
        memory.heapsCapacity =growCapacity(oldCap);
        memory.heaps = growArray(heapBlock , memory.heaps , oldCap , memory.heapsCapacity);
    }
    iniHeapBlock(memory.heaps+ memory.heapsCount ,size);
    memory.heapsCount++;
    return memory.heapsCount-1;
}


inline int findHeapBlock(size_t size){
    int low =0;
    int high = memory.heapsCount;
    int mid;
    while(low<high){
        mid = (low+high)/2;
        if((memory.heaps+mid)->maxInterval<size)low = mid+1;
        else high = mid;
    }
    if((memory.heaps+mid)->maxInterval<size)return addNewHeapBlock(size);
    return mid;
}

void pushInterval(heapIntervals* interval){
    
}

inline allocateHeapMemory(size_t size){
    int it = findHeapBlock(size);
    heapBlock* heap =memory.heaps+it;
    

}