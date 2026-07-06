#pragma once

typedef struct{
    
    int key;
    bool color;
    int parent;
    bool isLeft;
    int leftChild;
    int rightChild;

    int* values;
    int count;
    int capacity;
    int size;

    int* recycleValues;
    int recycleValuesCount;
    int recycleValuesCapacity;
}rbtNode;

typedef struct{
    int nodeIndex;
    int valueIndex;
}indexRbtKeys;

typedef struct{
    rbtNode* nodes;
    int root;
    int count;
    int capacity;

    indexRbtKeys* indexKey;
    int indexCapacity;

    int* recycleNodes;
    int recycleCount;
    int recycleCapacity;
}rbt;

rbtNode* getRootRbtNode(rbt* nodes);
void iniRbt(rbt* nodes);
void pushRbtNode(int value , int key , rbt* nodes);
rbtNode* lowerBoundRbt(rbt* nodes , int key);
// int popTopNodeValue(rbt* nodes , rbtNode* node);
int getTopNodeValue(rbtNode* node);
void deleteRbtNodeValue(rbt* nodes , int value);