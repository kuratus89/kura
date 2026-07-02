#include "common.h"
#include "datastruct.h"
#include "memory.h"

void iniRbt(rbt* nodes){
    nodes->nodes = NIL;
    nodes->count=0;
    nodes->capacity=0;
    nodes->root=NIL;

    nodes->hashKey = NULL;
    nodes->hashCapacity=0;

    nodes->recycleNodes = NULL;
    nodes->recycleCount=0;
    nodes->recycleCapacity=0;
}

void iniRbtNode(rbtNode* node){
    node->key=NIL;
    node->color=1;
    node->parent = NIL;
    node->isLeft=1;
    node->leftChild = NIL;
    node->rightChild = NIL;

    node->values = NULL;
    node->count=0;
    node->capacity=0;
}

void pushRbtValue(int value , rbtNode* node){
    if(node->count==node->capacity){
        int oldCap = node->capacity;
        node->capacity = growCapacity(oldCap);
        node->values = growArray(int , node->values , oldCap , node->capacity);
    }
    *(node->values +node->count) = value;
    node->count++;
}
void pushHash(int hash , int value , rbt* nodes){
    if(value>=nodes->hashCapacity){
        int oldCap = nodes->hashCapacity;
        nodes->hashCapacity = growCapacity(value);
        nodes->hashKey = growArray(int , nodes->hashKey , oldCap , nodes->hashCapacity);
    }
    *(nodes->hashKey + value) = hash;
}

void pushRecycleNode(rbt* nodes , int node){
    if(nodes->recycleCount==nodes->recycleCapacity){
        int oldCap = nodes->recycleCapacity;
        nodes->recycleCapacity = growCapacity(oldCap);
        nodes->recycleNodes = growArray(int , nodes->recycleNodes , oldCap , nodes->recycleCapacity);
    }
    *(nodes->recycleNodes + nodes->recycleCount) = node;
    nodes->recycleCount++;
}

void rotateLL(rbt* nodes , int node){
    rbtNode* child = nodes->nodes + node;
    rbtNode* parent = nodes->nodes + child->parent;
    rbtNode* grandParent = nodes->nodes+ parent->parent;
    
    if(grandParent->parent!=NIL){
        if(grandParent->isLeft)(nodes->nodes + grandParent->parent)->leftChild = child->parent;
        else(nodes->nodes + grandParent->parent)->rightChild = child->parent;
    }
    else nodes->root = child->parent;

    parent->parent = grandParent->parent;

    grandParent->parent = child->parent;
    parent->isLeft = grandParent->isLeft;
    grandParent->isLeft=0;
    if(parent->rightChild!=NIL){
        (nodes->nodes + parent->rightChild)->isLeft=1;
        (nodes->nodes + parent->rightChild)->parent = grandParent - nodes->nodes;
    }
    grandParent->leftChild = parent->rightChild;

    parent->rightChild = grandParent - nodes->nodes;
}
void rotateRR(rbt* nodes , int node){
    rbtNode* child = nodes->nodes + node;
    rbtNode* parent = nodes->nodes + child->parent;
    rbtNode* grandParent = nodes->nodes+ parent->parent;

    if(grandParent->parent!=NIL){
        if(grandParent->isLeft)(nodes->nodes+ grandParent->parent)->leftChild = child->parent;
        else (nodes->nodes + grandParent->parent)->rightChild = child->parent;
    }
    else nodes->root = child->parent;

    parent->parent = grandParent->parent;
    grandParent->parent = child->parent;
    parent->isLeft= grandParent->isLeft;

    if(parent->leftChild!=NIL){
        (nodes->nodes + parent->leftChild)->isLeft=0;
        (nodes->nodes + parent->leftChild)->parent = grandParent - nodes->nodes;
    }
    grandParent->rightChild = parent->leftChild;
    parent->leftChild = grandParent - nodes->nodes;
}
void rotateLR(rbt* nodes , int node){
    rbtNode* child = nodes->nodes+ node;
    rbtNode* parent = nodes->nodes+ child->parent;
    rbtNode* grandParent = nodes->nodes+ parent->parent;

    if(child->leftChild!=NIL){
        (nodes->nodes + child->leftChild)->parent = parent - nodes->nodes;
        (nodes->nodes + child->leftChild)->isLeft=0;
    }
    parent->rightChild = child->leftChild;
    grandParent->leftChild = child - nodes->nodes;
    child->isLeft=1;
    parent->parent = child - nodes->nodes;
    rotateLL(nodes , parent - nodes->nodes);
}
void rotateRL(rbt* nodes , int node){
    rbtNode* child = nodes->nodes + node;
    rbtNode* parent = nodes->nodes + child->parent;
    rbtNode* grandParent = nodes->nodes + parent->parent;

    if(child->rightChild!=NIL){
        (nodes->nodes + child->rightChild)->parent = parent - nodes->nodes;
        (nodes->nodes+ child->rightChild)->isLeft =1;
    }
    parent->leftChild = child->rightChild;
    grandParent->rightChild = child - nodes->nodes;
    child->isLeft = 0;
    parent->parent = child - nodes->nodes;
    rotateRR(nodes , child - nodes->nodes);
}

void colorRed(rbt* nodes , int node ){
    if(nodes->root == node)return;
    rbtNode* child = nodes->nodes + node;
    rbtNode* parent = nodes->nodes + child->parent;
    rbtNode* grandParent =  nodes->nodes + parent->parent;

    child->color =1;

    if(!parent->color)return;
    

    bool isUncleRed;
    int uncle;
    if(parent->isLeft){
        if(grandParent->rightChild==NIL){
            isUncleRed=0;
            uncle = NIL;
        }
        else if((nodes->nodes +grandParent->rightChild)->color){
            isUncleRed =1;
            uncle =grandParent->rightChild;
        }
        else{
            isUncleRed =0;
            uncle =  grandParent->rightChild;
        }
        
    }
    else {
        if(grandParent->leftChild==NIL){
            isUncleRed =0;
            uncle = NIL;
        }
        else if((nodes->nodes + grandParent->leftChild)->color){
            isUncleRed =1;
            uncle = grandParent->leftChild;
        }
        else {
            isUncleRed =0;
            uncle = grandParent->leftChild;
        }
    }

    if(isUncleRed){
        (nodes->nodes + uncle)->color =0;
        parent->color = 0;
        colorRed(nodes , grandParent - nodes->nodes);
        return;
    }
    if(child->isLeft && parent->isLeft){
        rotateLL(nodes , child - nodes->nodes);
        grandParent->color =1;
        parent->color =0;
    }
    else if((!child->isLeft)&&(!parent->isLeft)){
        rotateRR(nodes , child - nodes->nodes);
        grandParent->color =1;
        parent->color =0;
    }
    else if((!child->isLeft)&&(parent->isLeft)){
        rotateLR(nodes , child- nodes->nodes);
        grandParent->color =1;
        child->color =0;
    }
    else{
        rotateRL(nodes , child - nodes->nodes);
        grandParent->color =1;
        child->color =0;
    }
    
}

void pushNode(int value , int key , rbt* nodes){
    if(nodes->count==nodes->capacity){
        int oldCap = nodes->capacity;
        nodes->capacity = growCapacity(oldCap);
        nodes->nodes = growArray(rbtNode , nodes->nodes , oldCap , nodes->capacity);
    }
    if(value >= nodes->hashCapacity){
        int oldCap = nodes->hashCapacity;
        nodes->hashCapacity = growCapacity(oldCap);
        nodes->hashKey = growArray(int , nodes->hashKey , oldCap , nodes->hashCapacity);
    }

    
    if(nodes->root == NIL){
        int it;
        if(nodes->recycleCount>0){
            it = *(nodes->recycleNodes + nodes->recycleCount-1);
            nodes->recycleCount--;
        }
        else {
            it = nodes->count;
            nodes->count++;
            iniRbtNode(nodes->nodes + it);
        }
        (nodes->nodes+it)->key = key;
        pushRbtValue(value , nodes->nodes+ it);
        pushHash(it , value , nodes);
        (nodes->nodes +it)->leftChild = NIL;
        (nodes->nodes +it)-> rightChild = NIL;
        nodes->root = it;
        (nodes->nodes + it)->color = 0;
        (nodes->nodes + it)->parent = NIL;
        return;
    }

    rbtNode* current = nodes->nodes +nodes->root;
    bool multi=0;
    bool left;
    while(true){
        if(key>current->key){
            if(current->rightChild == NIL){
                left =0;
                break;
            }
            current = nodes->nodes + current->rightChild;
            continue;
        }
        if(key<current->key){
            if(current->leftChild == NIL){
                left =1;
                break;
            }
            current = nodes->nodes + current->leftChild;
            continue;
        }
        if(key==current->key){
            multi=1;
            break;
        }
    }

    if(multi){
        pushRbtValue(value , current);
        pushHash(current - nodes->nodes , value , nodes);
        return;
    }

    int it;
    if(nodes->recycleCount>0){
        it = *(nodes->recycleNodes+ nodes->recycleCount-1);
        nodes->recycleCount--;
    }
    else {
        it = nodes->count;
        nodes->count++;
        iniRbtNode(nodes->nodes + it);
    }
    (nodes->nodes+it)->key = key;
    pushRbtValue(value , nodes->nodes+ it);
    (nodes->nodes +it)->leftChild = NIL;
    (nodes->nodes +it)-> rightChild = NIL;
    (nodes->nodes +it)->parent = current - nodes->nodes;
    (nodes->nodes+it)->color = 0;

    if(left){
        current->leftChild =it;
        (nodes->nodes+it)->isLeft =1;
    }
    else {
        current->rightChild = it;
        (nodes->nodes +it)->isLeft = 0;
    }

    colorRed(nodes , it);
}

void swap(rbt*nodes , int node1 , int node2){// do not swap color
    rbtNode* x = nodes->nodes + node1;
    rbtNode* y = nodes->nodes + node2;

    if(nodes->root!=node1){
        if(x->isLeft)(nodes->nodes + x->parent)->leftChild = node2;
        else (nodes->nodes + x->parent)->rightChild = node2;
    }
    
    if(nodes->root!=node2){
        if(y->isLeft)(nodes->nodes + y->parent)->leftChild = node1;
        else (nodes->nodes + y->parent)->rightChild = node1;
    }

    if(x->leftChild != NIL)(nodes->nodes + x->leftChild)->parent = node2;
    if(x->rightChild != NIL)(nodes->nodes + x->rightChild)->parent = node2;

    if(y->leftChild !=NIL)(nodes->nodes + y->leftChild)->parent = node1;
    if(y->rightChild !=NIL)(nodes->nodes + y->rightChild)->parent = node2;

    int parent = x->parent;
    int leftChild = x->leftChild;
    int rightChild = x->rightChild;
    x->parent = y->parent;
    x->leftChild = y->leftChild;
    x->rightChild = y->rightChild;

    if(nodes->root == node1)nodes->root = node2;
    else if(nodes->root == node2)nodes->root = node1;
}

void forceDelete(rbt* nodes , int node){
    rbtNode* current = (nodes->nodes + node);
    if((current->leftChild != NIL)|| (current->rightChild != NIL))return;
    if(nodes->root == node)nodes->root = NIL;
    else {
        if(current->isLeft)(nodes->nodes + current->parent)->leftChild = NIL;
        else (nodes->nodes + current->parent)->rightChild = NIL;
    }
    iniRbtNode(nodes->nodes + node);
    pushRecycleNode(nodes , node);
}

void moveLeft(rbt* nodes , int node){
    rbtNode* parent = nodes->nodes + node;
    rbtNode* child = nodes->nodes + parent->rightChild;

    if(nodes->root == node)nodes->root = parent->rightChild;
    else {
        if(parent->isLeft)(nodes->nodes + parent->parent)->leftChild = parent->rightChild;
        else (nodes->nodes + parent->parent)->rightChild = parent->rightChild; 
    }

    if(child->leftChild !=NIL){
        (nodes->nodes + child->leftChild)->isLeft = 0;
        (nodes->nodes + child->leftChild)->parent = node;
    }
    parent->rightChild = child->leftChild;
    child->isLeft = parent->isLeft;
    parent->parent = child - nodes->nodes;
    child->leftChild = node;
}

void moveRight(rbt* nodes , int node){
    rbtNode* parent = nodes->nodes + node;
    rbtNode* child = nodes->nodes + parent->leftChild;

    if(nodes->root = node)nodes->root = parent->leftChild;
    else {
        if(parent->isLeft)(nodes->nodes + parent->parent)->leftChild = parent->leftChild;
        else (nodes->nodes + parent->parent)->rightChild = parent->leftChild;
    }

    if(child->rightChild !=NIL){
        (nodes->nodes + child->rightChild)->isLeft =1;
        (nodes->nodes + child->rightChild)->parent = node;
    }

    parent->leftChild = child->rightChild;
    child->isLeft = parent->isLeft;
    parent->parent = child - nodes->nodes;
    child->leftChild = node;
}

void swapColor(rbtNode* x , rbtNode* y){
    if(x->color == y->color)return;
    else {
        x->color = !x->color;
        y->color = !y->color;
    }
    // bool color = x->color;
    // x->color = y->color;
    // y->color = color;
}
void colorBlack(rbt* nodes , int node){

    if((nodes->nodes + node)->color){
        (nodes->nodes + node)->color =0;
        return;
    }

    if(nodes->root = node)return;

    rbtNode* current = nodes->nodes + node;
    rbtNode* parent = nodes->nodes + current->parent;
    rbtNode* sibling;


    if(current->isLeft)sibling = nodes->nodes + parent->rightChild;
    else sibling = nodes->nodes + parent->leftChild;

    if(sibling->color){
        // if(!parent->color){
        //     sibling->color=0;
        //     parent->color = 1;
        // }
        swapColor(parent , sibling);
        if(current->isLeft)moveLeft(nodes , parent - nodes->nodes);
        else moveRight(nodes , parent - nodes->nodes);
        //it will still have siblings

        if(current->isLeft)sibling = nodes->nodes + parent->rightChild;
        else sibling = nodes->nodes + parent->leftChild;
    }
    int leftChild = sibling->leftChild;
    int rightChild = sibling->rightChild;
    bool isLeftBlack , isRightBlack;

    if(leftChild ==NIL)isLeftBlack =1;
    else {
        if((nodes->nodes + leftChild)->color)isLeftBlack=0;
        else isLeftBlack =1;
    }
    if(rightChild == NIL)isRightBlack=1;
    else {
        if((nodes->nodes+ rightChild)->color)isRightBlack=0;
        else isRightBlack=1;
    }

    if(isRightBlack && isLeftBlack){
        sibling->color =1;
        colorBlack(nodes , parent - nodes->nodes);
        return;
    }

    if(current->isLeft){
        if(isLeftBlack){
            // if(!parent->color){
            //     sibling->color =0;
            //     parent->color =1;
            // }
            swapColor(parent , sibling);
            (nodes->nodes + rightChild)->color =0;
            moveLeft(nodes , parent - nodes->nodes);
        }
        else {

        }
    }

    // bool farChild, nearChild;

    // if(current->isLeft){
    //     farChild = isRightBlack;
    //     nearChild = isLeftBlack;
    // }
    // else {
    //     farChild = isLeftBlack;
    //     nearChild = isRightBlack;
    // }

    // if(nearChild){

    // }
    // if(farChild){

    // }


}


int nextNode(rbt* nodes , int node){
    while((nodes->nodes + node)->rightChild != NIL)node = (nodes->nodes + node)->rightChild;
    return node;
}


void deleteNode(rbt* nodes , int nodeIt){
    rbtNode* node = nodes->nodes + nodeIt;
    if((node->leftChild != NIL)&&(node->rightChild != NIL)){
         swap(nodes , node , nextNode(nodes , nodeIt));
    }
    
    if(node->leftChild != NIL){
        if(node->parent !=NIL){
            if(node->isLeft)(nodes->nodes + node->parent)->leftChild = node->leftChild;
            else (nodes->nodes + node->parent)->rightChild = node->leftChild;
        }
        else nodes->root = node->leftChild;
        (nodes->nodes + node->leftChild)->parent = node->parent;
        (nodes->nodes + node->leftChild)->isLeft = node->isLeft;
        if(!node->color)(nodes->nodes + node->leftChild)->color =0;
        forceDelete(nodes , nodeIt);
        return;
    }

    if(node->rightChild != NIL){
        if(node->parent !=NIL){
            if(node->isLeft)(nodes->nodes + node->parent)->leftChild = node->rightChild;
            else (nodes->nodes + node->parent)->rightChild = node->rightChild;
        }
        else nodes->root = node->rightChild;
        (nodes->nodes + node->rightChild)->parent = node->parent;
        (nodes->nodes + node->rightChild)->isLeft = node->isLeft;
        if(!node->color)(nodes->nodes + node->rightChild)->color =0;
        forceDelete(nodes , nodeIt);
        return;
    }

    

    if(node->color){
        forceDelete(nodes , nodeIt);
        return;
    }

    doubleBlack(nodes , nodeIt);
    forceDelete(nodes , nodeIt);    
}