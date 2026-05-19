
#include "chunk.h"
#include "memory.h"
#include "token.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"

void compileError(Token* token , char* msg){
    printf("Error whiile compiling\nline: %d\ntoken type: %s\ntoken: %s" , token->line ,disassembleTokenType(token->type) , tokenGetSource(token));
    exit(87);
}

void iniFuncByte(funcByte* func , int count){
    func->count = 0;
    func->func = growArray(Chunk , NULL , 0 , count);
    initilizeChunk(&func->global);
}

void iniNodes(Nodes* nodes){
    nodes->capacity=0;
    nodes->count = 0;
    nodes->node = NULL;
}

calcNode* writeNodes(Nodes* nodes , calcNode* node){
    if(nodes->capacity==nodes->count){
        int oldCap = nodes->capacity;
        nodes->capacity= growCapacity(oldCap);
        nodes->node = growArray(calcNode , nodes->node , oldCap , nodes->capacity);
    }
    nodes->node[nodes->count] = *node;
    nodes->count++;
    return (&nodes->node[nodes->count -1]);
}

int isOperator(tokenType type){
    switch(type){
        case TOKEN_SLASH : return 4;
        case TOKEN_STAR :return 3;
        case TOKEN_PLUS :return 2;
        case TOKEN_MINUS :return 1;
        default : return 0;
    }
}

bool isValue(tokenType type){
    switch(type){
        case TOKEN_INT:  return 1;
        case TOKEN_FLOAT : return 1;
        case TOKEN_STRING : return 1;
        default : return 0;
    }
}

bool objValue(Token** tok , Value* val){
    Token* token = *tok;

    *tok = token;
    return (token->type == TOKEN_IDENTIFIER);
}

calcNode* buildBinTree(Token** tok , dataType type ,Nodes* nodes ){
    Token* token = *tok;
    bool braclo=0;
    if(token->type==TOKEN_LEFT_PAREN){
        braclo=1;
        token++;
    }
    calcNode temp;
    temp.parent = NULL;
    temp.left = NULL;
    temp.right = NULL;
    temp.val = NULL;
    temp.oper = NULL;
    
    calcNode* current = writeNodes(nodes , &temp);
    Value tempVal;

    if(isOperator(token->type)){
        current->isleaf=0;
        current->oper = token->type;
        calcNode tempop;
        tempop.isleaf=1;
        tempop.val = NULL;
        current->left= writeNodes(nodes , &tempop);
        token++;
        if(token->type==TOKEN_LEFT_PAREN){
            current->right =  buildBinTree(&token , type , nodes);        
        }
        else if(obj(token , &tempVal)){
            if(!isValue(token->type))compileError(token , "expected a valid value");
        }
        
        if(!isValue(token->type))compileError(token , "expected a valid value");
        tempop.val = token;
        current->right = writeNodes(nodes , &tempop);
        token++;
    }
    else if(isValue(token->type)) {
        current->isleaf =1;
        current->val = token;
        token++;
    }
    else if(objValue(&token , &tempVal)){
        compileError(token , "identifier not yet supported");
    }


    while((token->type !=TOKEN_EOL)&&(token->type != TOKEN_SEMICOLON)&&(token->type != TOKEN_RIGHT_PAREN)){
        if(!isOperator(token->type))compileError(token , "expected a valid operator");
        int cur = isOperator(token->type);
        tokenType ty = token->type;
        token++;
        while((current->parent != NULL)){
            int par = isoperator(current->parent->oper);
            if(cur<=par){
                current = current->parent;
            }
            else break;
        }
        if(objValue(&token , &tempVal)){
            compileError(token , "objects not yet supported");
            continue;
        }
        
        if(!isValue(token->type))compileError(token , "expected a valid value");

        calcNode rightNode;
        rightNode.isleaf=1;
        rightNode.val = token;
        if(current->parent==NULL){
            calcNode newParent;
            newParent.isleaf=0;
            newParent.parent= NULL;
            newParent.oper = ty;
            newParent.left = current;
            newParent.right = writeNode(nodes , &rightNode);
            current->parent = writeNodes(nodes , &newParent);
            current->parent->right->parent = current->parent;
            current = current->parent->right;
        }
        else {
            calcNode newNode;
            newNode.isleaf=0;
            newNode.left = current;
            newNode.parent = current->parent;
            newNode.oper = ty;
            newNode.right = writeNodes(nodes , &rightNode);
            current->parent->right = writeNodes(nodes , &newNode);
        }

    }
    if(braclo){
        if(token->type!=TOKEN_RIGHT_PAREN)compileError(token , "right paren not found");
        token++;
    }
    while(current->parent!=NULL)current = current->parent;
    *tok = token;
    return current;
}



void Globalcollector(Token** tokenPtr , dataType type ){
    
}

void compileGlobal(Tokens* global , funcByte* func){
    for(int i=0 ; i<global->count ; i++){
        Token* it = global->token+i;
    }
    global->
}

void compile(tokenFunctions* tf , funcByte* func){
    iniFuncByte(func , tf->count);
    compileGlobal(&tf->mainFunc , func);
}
