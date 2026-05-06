#include "memory.h"
#include "value.h"
#include <stdio.h>

void initilizeValueArray(valueArray* array){
    array->count=0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(valueArray* array , Value value){
    if(array->capacity - array->count ==0){
        int oldCap = array->capacity;
        array->capacity = growCapacity(array->capacity);
        array->values = growArray(Value , array->values , oldCap , array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(valueArray* array){
    freeArray(Value , array->values , array->capacity);
    initilizeValueArray(array);
}

void printValue(Value value){
    printf("%g" , value);
}