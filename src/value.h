#pragma once
#include "common.h"

typedef enum{
    DATA_INT = 0,
    DATA_STRING= 1,
    DATA_BOOL = 2,
    DATA_CHAR = 3 ,
    DATA_FLOAT = 4,
    DATA_VECTOR  = 5,
}dataType;

typedef struct{
    dataType type;
    void* value;
}Value;

typedef struct{
    int capacity;
    int count;
    Value* values;
}valueArray;

void initilizeValueArray(valueArray* array);
void writeValueArray(valueArray* array , Value value);
void freeValueArray(valueArray* array);
void printValue(Value value);

void iniValue(Value* value , dataType type);
void writeValue(Value* value , void* val);
dataType getDataType(Token* token);