#pragma once
#include "value.h"
#include "token.h"
#include "chunk.h"

typedef struct calcNode{
    bool isleaf;
    struct calcNode* parent;
    Token* val;
    struct calcNode* left;
    struct calcNode* right;
}calcNode;

typedef struct{
    int count;
    int capacity;
    calcNode* node;
}Nodes;



void compile(tokenFunctions* tf ,funcByte* func);
void iniVarMaps(varMaps* maps);