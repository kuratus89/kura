#pragma once

typedef struct{
    char* start;
    char* end;
    int length;
    int line;
}scanPtr;

typedef struct{
    int capacity;
    int count;
    scanPtr* pointers;
}scar;

scar scanToPtr(char* source);
