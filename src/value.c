#include "memory.h"
#include "value.h"
#include "common.h"

int dataSize[] = {4 , 0 , 1 , 4 ,0};

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

void iniValue(Value* value , dataType type){
    value->type = type;
    int size = dataSize[(int)type];
    if(size==0){
        value->value = NULL;
        return;
    }
    value->value = malloc(size);
}


void freeValueArray(valueArray* array){
    freeArray(Value , array->values , array->capacity);
    initilizeValueArray(array);
}

void printValue(Value value){
    printf("%g" , value);
}

void writeValueInt(Value* value , void* val){
    *((int*)value->value) = *((int*)val);
}

void writeValueChar(Value* value , void* val){
    *((char*)value->value) = *((char*)val);
}

void writeValueBool(Value* value , void* val){
    *((bool*)value->value) = *((bool*)val);
}

void writeValueString(Value* value , void* val){

}

void writeValueFloat(Value* value , void* val){
    *((float*)value->value) = *((float*)val);
}
void writeValueVector(Value* value , void* val){

}

void (*dataWriteFunc[])(Value* , void*) = {writeValueInt , writeValueString , writeValueBool , writeValueChar , writeValueFloat , writeValueVector};

void writeValue(Value* value , void* val){
    dataWriteFunc[(int)value->type](value , val);
}