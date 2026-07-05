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
}rbtNode;

typedef struct{
    rbtNode* nodes;
    int root;
    int count;
    int capacity;

    int* indexKey;
    int indexCapacity;

    int* recycleNodes;
    int recycleCount;
    int recycleCapacity;
}rbt;

rbtNode* getRootRbtNode(rbt* nodes);