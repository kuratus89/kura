#include "common.h"
#pragma once
#include <stdint.h>
#include "value.h"
int growCapacity(int oldCap);

#define growArray(type , pointer , oldCount , newCount)\
(type*)reallocate(pointer , sizeof(type)* (oldCount) ,\
sizeof(type) * (newCount))

#define freeArray(type , pointer , oldCount)reallocate(pointer , sizeof(type)* (oldCount) , 0)

void* reallocate(void* pointer , size_t oldSize , size_t newSize);
int* find(int* start , int value , int length , bool isSort);



typedef struct{
    int size;
    int start;
}heapIntervals;



typedef struct{
    void* memory;
    int maxInterval;

    heapIntervals* intervals;
    int* index;
    int intervalsCount;
    int intervalCapacity;
}heapBlock;


typedef struct {
    //stack memory
    void* stack;
    int stackCount;

    Value* values;
    int valuesCapacity;


    // runtime stack
    void* runtime;
    int runtimeCount;

    Value* runtimeValues;
    int runtimeValuesCount;
    int runtimeValuesCapacity;


    // heap memory
    heapBlock* heaps;
    int heapsCount;
    int heapsCapacity;
}Memory;

void iniMemory();
Value* allocateStackMemoryValueIndex(size_t x , int index);
Value* getStackMemoryValueIndex(int index);
void unloadStackMemoryValueIndex(int index);
void writeStackMemoryValueIndex(Value* value , int index);
Value* pushNewValueRuntimeStack(size_t size);
int getRuntimeStackSize();
Value* popRuntimeStack();
Value* getRuntimeStack(int offset);