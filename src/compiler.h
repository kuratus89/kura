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
}calcNodes;

typedef struct compNode{
    bool isLeaf;
    struct compNode* parent;

    //if leaf=>
    Token* comparator;
    Token* leftValue;
    Token* rightValue;

    //if not leaf=>
    struct compNode* left;
    struct compNode* right;
}compNode;





void compile(tokenFunctions* tf ,funcByte* func);
void iniVarMaps(varMaps* maps);