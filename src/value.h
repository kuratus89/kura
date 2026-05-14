#pragma once
#include "common.h"

typedef enum{
    DATA_VOID ,
    DATA_INT,
    DATA_STRING,
    DATA_BOOL
}dataType;
typedef double Value;

typedef struct{
    int capacity;
    int count;
    Value* values;
}valueArray;

void initilizeValueArray(valueArray* array);
void writeValueArray(valueArray* array , Value value);
void freeValueArray(valueArray* array);
void printValue(Value value);