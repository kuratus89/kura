#pragma once
#include "common.h"

struct Token;

typedef enum{
    DATA_INT = 0,
    DATA_STRING= 1,
    DATA_BOOL = 2,
    DATA_CHAR = 3 ,
    DATA_FLOAT = 4,
    DATA_VECTOR  = 5,
    DATA_VOID = 6,
}dataType;

typedef struct{
    dataType type;
    void* value;
    size_t size;
}Value;

typedef struct{
    int capacity;
    int count;
    Value* values;
}valueArray;





void initilizeValueArray(valueArray* array);
int writeValueArray(valueArray* array , Value* value);
void writeValueArrayIndex(valueArray* array , Value* value , int index);
void freeValueArray(valueArray* array);

void iniValue(Value* value , dataType type);
void writeValue(Value* value , void* val);
dataType getDataType( struct Token* token);
void insertDataToValue(struct Token* token , Value* value);
 Value cloneValue(Value* value);