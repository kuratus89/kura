#include "common.h"
#include "memory.h"
#include "stdlib.h"

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
        else if(value<*it)high = mid-1;
        else low = mid+1;
    }
    return NULL;    
}