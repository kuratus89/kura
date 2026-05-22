#pragma once
#include "value.h"
#include "token.h"

typedef struct calcNode{
    bool isleaf;
    struct calcNode* parent;

    // if its leaf , then we will access  its value
    Token* val;

    // if its not root(operator) , then we will use pointers to next nodes 
    struct calcNode* left;
    struct calcNode* right;
}calcNode;

typedef struct{
    int count;
    int capacity;
    calcNode* node;
}Nodes;

void compile(tokenFunctions* tf , funcByte* func);