#include "memory.h"
#include "value.h"
#include "common.h"
#include "token.h"
#include <string.h>

int dataSize[] = {4 , 0 , 1 , 1 , 4 ,0};

void initilizeValueArray(valueArray* array){
    array->count=0;
    array->capacity = 0;
    array->values = NULL;
}

int writeValueArray(valueArray* array , Value* value){
    if(array->capacity - array->count ==0){
        int oldCap = array->capacity;
        array->capacity = growCapacity(array->capacity);
        array->values = growArray(Value , array->values , oldCap , array->capacity);
    }
    array->values[array->count] = *value;
    array->count++;
    return array->count-1;
}

void writeValueArrayIndex(valueArray* array , Value* value , int index){
    if(index>=array->capacity)return;
    array->values[index] =*value;
}





inline void iniValue(Value* value , dataType type){
    value->type = type;
    value->size = dataSize[(int)type];

    if(value->size==0){
        value->value = NULL;
        return;
    }
    value->value = malloc(value->size);
    // value->value = allocateStackMemory(value->size);
}

inline Value* iniStackMemoryValue(int index , dataType type){
    int size = dataSize[(int)type];
    Value* value = allocateStackMemoryValueIndex(size , index);
    value->type = type;
    value->size = size;
    return value;
}

inline Value* pushRuntimeStackValue(dataType type){
    int size = dataSize[(int)type];
    Value* value = pushNewValueRuntimeStack(size);
    value->type = type;
    value->size = size;
    return value;
}
inline Value* pushRuntimeStackCloneValue(Value* value){
    Value* val = pushNewValueRuntimeStack(value->size);
    memcpy(val->value , value->value , value->size);
    val->size = value->size;
    val->type = value->type;
    return val;
}

void freeValue(Value* value){
    free(value->value);
}

void freeValueArray(valueArray* array){
    for(int i=0 ; i!=array->count ; i++)freeValue(&array->values[i]);
    freeArray(Value , array->values , array->capacity);
    initilizeValueArray(array);
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

void tokenToInt(Token* token , void* val){
    int i =0;
    for(char* it = token->start ; ((it!=token->end)&&(*it!='.')) ; it++)i = (i*10)+ (*it - '0');
    *(int*)val = i;
}

void tokenToString(Token* token , void* val){

}

void tokenToBool(Token* token , void* val){

}

void tokenToChar(Token* token , void* val){
    
}

void tokenToFloat(Token* token , void* val){

}

void tokenToVector(Token* token , void* val){

}
void (*tokenToValFn[])(Token* , void*) = {tokenToInt , tokenToString , tokenToBool , tokenToChar , tokenToFloat , tokenToVector};

void insertDataToValue(struct Token* token , Value* value){
    tokenToValFn[(int)value->type](token , value->value);
}

dataType getDataType(struct Token* token){
    char* typeChar = tokenGetSource(token);
    // will optimize later
    char* dataTypesChar[] = {"int" , "string" , "bool" , "char" , "float" , "vector" , "void"};
    for(int i=0 ; i<7 ; i++)if(strcmp(typeChar , dataTypesChar[i])==0)return((dataType)i);
}

inline void copyValue(Value* from , Value* to){
    memcpy(to->value , from->value , to->size);
}