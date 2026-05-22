
#include "chunk.h"
#include "memory.h"
#include "token.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"

void compileError(Token* token , char* msg){
    printf("Error whiile compiling\n%s\nline: %d\ntoken type: %s\ntoken: %s" , msg , token->line ,disassembleTokenType(token->type) , tokenGetSource(token));
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
void iniNode(calcNode* node){
    node->isleaf = NULL;
    node->left = NULL;
    node->right = NULL;
    node->left = NULL;
    node->parent = NULL;
    node->val = NULL;
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

calcNode* buildBinTree(Token** tok , dataType type ,Nodes* nodes , bool fr){
    Token* token = *tok;    
    calcNode* current;
    Value tempVal;
    bool bra=0;
    if((token->type==TOKEN_LEFT_PAREN)&&(!fr)){
        token++;
        bra=1;
    }

    if((token->type==TOKEN_LEFT_PAREN)){
        current = buildBinTree(&token , type , nodes , 0);
    }
    else if(isOperator(token->type)){
        calcNode temp;
        iniNode(&temp);
        current = writeNodes(nodes , &temp);
        current->isleaf=0;
        current->val = token;
        
        current->left= writeNodes(nodes , &temp);
        token++;
        if(token->type==TOKEN_LEFT_PAREN){
            current->right =  buildBinTree(&token , type , nodes , 0); 
            current->right->parent = current;  
            // current = current->right;  
        }
        else if(isValue(token->type)){
            temp.val = token;
            temp.isleaf = 1;
            current->right = writeNodes(nodes , &temp);
            current->right->parent = current;
            // current = current->right;
            token++;
        }
        else if(objValue(&token , &tempVal)){
            compileError(token , "identifier not yet supported");
            token++;
        }
        else compileError(token , "Expected a valid value");
        
    }
    else if(isValue(token->type)) {
        calcNode temp;
        iniNode(&temp);
        temp.isleaf = 1;
        temp.val = token;
        current = writeNodes(nodes , &temp);
        token++;
    }
    else if(objValue(&token , &tempVal)){
        compileError(token , "identifier not yet supported");
    }
    else {
        compileError(token , "Invalid syntax");
    }


    while((token->type !=TOKEN_EOL)&&(token->type != TOKEN_SEMICOLON)&&((fr)||(token->type!=TOKEN_RIGHT_PAREN))){
        if(!isOperator(token->type))compileError(token , "expected a valid operator");
        int cur = isOperator(token->type);
        Token* ty = token;
        token++;
        while((current->parent != NULL)){
            int par = isOperator(current->parent->val->type);
            if(cur<=par){
                current = current->parent;
            }
            else break;
        }
        calcNode rightNode;
        iniNode(&rightNode);
        if(token->type==TOKEN_LEFT_PAREN){
            calcNode* ntr = buildBinTree(&token , type , nodes , 0);
            rightNode = *ntr;
        }
        else if(objValue(&token , &tempVal)){
            compileError(token , "objects not yet supported");
            continue;
        }
        else if(isValue(token->type)){
            rightNode.isleaf=1;
            rightNode.val = token;
            token++;
        }
        else compileError(token , "expected a valid value");


        if(current->parent==NULL){
            calcNode newParent;
            newParent.isleaf=0;
            newParent.parent= NULL;
            newParent.val = ty;
            newParent.left = current;
            newParent.right = writeNodes(nodes , &rightNode);
            current->parent = writeNodes(nodes , &newParent);
            current->parent->right->parent = current->parent;
            current = current->parent->right;
        }
        else {
            
            calcNode newNode;
            iniNode(&newNode);
            newNode.isleaf = 0;
            newNode.val = ty;
            newNode.parent = current->parent;
            newNode.left = current;
            newNode.right = writeNodes(nodes , &rightNode);

            current->parent = writeNodes(nodes , &newNode);
            current = current->parent;
            current->parent->right = current;
            current->right->parent = current;
            current = current->right;
        }

    }
    if(bra){
        if(token->type!=TOKEN_RIGHT_PAREN)compileError(token , "right paren not found");
        token++;
    }
    if(fr) while(!((token->type==TOKEN_SEMICOLON)||(token->type==TOKEN_EOL)))token++;
    while(current->parent!=NULL)current = current->parent;
    *tok = token;
    return current;
}

int pushValue(Token* token ,  Chunk* chunk , dataType type){
    Value value;
    iniValue(&value , type);
    insertDataToValue(token , &value);
    writeValueArray(&chunk->constants , &value);
    return chunk->constants.count -1;
}

void executeBinTree(calcNode* current , Chunk* chunk , dataType type){

    if(current->isleaf){
        writeChunk(chunk , OP_LOAD_CONSTANT , current->val->line);
        int it = pushValue(current->val , chunk , type);
        writeChunk(chunk , it , current->val->line);
        return;
    }
    if(current->left->val==NULL){
        if((current->val->type!=TOKEN_MINUS)&&(current->val->type!=TOKEN_PLUS))compileError(current->val , "Ivalid syntax");
        executeBinTree(current->right , chunk , type);
        writeChunk(chunk , OP_NEGATE , current->val->line);
        return;
    }
    
    executeBinTree(current->left , chunk , type);
    executeBinTree(current->right , chunk , type);
    opcode op;
    switch(current->val->type){
        case TOKEN_PLUS : op=OP_ADD;break;
        case TOKEN_MINUS :op= OP_SUB;break;
        case TOKEN_STAR : op = OP_MUL;break;
        case TOKEN_SLASH : op = OP_DIV;break;
    }
    writeChunk(chunk , op , current->val->line);
}

dataType tokenToDataType(Token* token){
    char* dataName = tokenGetSource(token);
    char* dataChar[] = {"int" , "string" , "char" , "bool" , "float" , "vector"};
    dataType dt[] = {DATA_INT , DATA_STRING , DATA_CHAR , DATA_BOOL , DATA_FLOAT , DATA_VECTOR};
    
    for(int i=0 ; i<7 ; i++)if(tokenEqual(dataChar[i] , token->start , token->end))return dt[i];
    compileError(token , "Invalid data type");
}

void datastructures(Token** tok , Chunk* chunk){
    Token* token = *tok;
    dataType type = tokenToDataType(token);
    // for now i am not storing the value as variable , and let it remain in vm's stack
    token++;
    if(token->type!=TOKEN_IDENTIFIER)compileError(token , "expected a identifier");
    token++;
    if(token->type!=TOKEN_EQUAL)compileError(token , "invalid syntax");
    token++;
    Nodes nodes;
    iniNodes(&nodes);
    int tokenCount = 0;
    for(Token* t = token; (t->type != TOKEN_SEMICOLON) && (t->type != TOKEN_EOL); t++)tokenCount++;
    nodes.capacity = tokenCount * 3 + 8;
    nodes.node = growArray(calcNode, NULL, 0, nodes.capacity);
    calcNode* current=buildBinTree(&token , type , &nodes , 1);
    executeBinTree(current ,chunk , type);
    *tok = token;
}



void compileGlobal(Tokens* global , funcByte* func){
    
    Token* it=global->token;
    while(it->type!=TOKEN_EOL){
        switch(it->type){
            case TOKEN_DATA : datastructures(&it , &func->global);
        }
        while((it->type!=TOKEN_SEMICOLON)&&(it->type != TOKEN_EOL))it++;
        if(it->type !=TOKEN_EOL)it++;
    }    
}

void compile(tokenFunctions* tf , funcByte* func){
    iniFuncByte(func , tf->count);
    compileGlobal(&tf->mainFunc , func);
}
