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
    int stackElementSize;
    Value* value;
}valueData;

typedef struct {
    void* stack;
    int stackCount;
    valueData* stackValueData;
    int stackValueDataCount;
    int stackValueDataCapacity;
}Memory;

void iniMemory();
void* allocateStackMemory(int size);
void freeStack(int index);