
#include "chunk.h"
#include "memory.h"
#include "token.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"
void compileCallFunction(Token** tok , Chunk* chunk , funcByte* func);
void compileNewBranch(Token** tok , Chunk* chunk , funcByte* func);
void compileSingleLine(Token** tok , Chunk* chunk , funcByte* func);
void compileError(Token* token , char* msg){
    printf("Error whiile compiling\n%s\nline: %d\ntoken type: %s\ntoken: %s" , msg , token->line ,disassembleTokenType(token->type) , tokenGetSource(token));
    exit(87);
}

void iniFuncByte(funcByte* func , int count){
    func->count = 0;
    func->func = NULL;
    initilizeChunk(&func->global);
    func->funcCount = 0;
    func->funcCapacity= 0;
    iniVarMaps(&func->vars);
}

void iniNodes(calcNodes* nodes){
    nodes->capacity=0;
    nodes->count = 0;
    nodes->node = NULL;
}
void iniNode(calcNode* node){
    node->isleaf = NULL;
    node->left = -1;
    node->right = -1;
    node->parent = -1;
    node->val = NULL;
}

void iniVarMap(varMap* map){
    map->isValue = 0;
    map->value = -1;
    for(int i =0 ; i<63 ; i++)map->child[i] = -1;
}


int getMapInt(char c){
    if((c>='a')&&(c<='z'))return (c - 'a');
    if((c>='A')&&(c<='Z'))return (c - 'A' + 26);
    if((c>='0')&&(c<='9'))return (c - '0' + 52);
    if(c=='_')return (62);
    return -1;
}

int writeVarMaps(varMaps* maps , varMap* map){
    if(maps->count == maps->capacity){
        int oldCap = maps->capacity;
        maps->capacity = growCapacity(oldCap);
        maps->maps = growArray(varMap , maps->maps ,oldCap , maps->capacity);
    }
    maps->maps[maps->count] = *map;
    maps->count++;
    return maps->count-1;
}

void writeKey(varMaps* maps , char* name , int x){
    if(x>=maps->keyCapacity){
        int oldCap = maps->keyCapacity;
        maps->keyCapacity = growCapacity(x);
        maps->keys = growArray(string , maps->keys , oldCap , maps->keyCapacity);
        for(int i = oldCap ; i<maps->capacity ; i++){
            (maps->keys+i)->value = NULL;
            (maps->keys+i)->capacity = 0;
        }
    }
    int cnt =0;
    for(char* i= name ; *i!='\0' ; i++)cnt++;
    cnt++;

    if((maps->keys+x)->capacity!=cnt){
        (maps->keys+x)->value = growArray(char , (maps->keys+x)->value , (maps->keys+x)->capacity , cnt);
        (maps->keys+x)->capacity = cnt;
    }
    for(int i=0; i!=cnt; i++)(maps->keys+x)->value[i] = name[i];
}

char* getKey(varMaps* maps , int x){
    return ((maps->keys+x)->value);
}



void iniVarMaps(varMaps* maps){
    maps->capacity=0;
    maps->count=0;
    maps->keyCapacity=0;
    maps->maps = NULL;
    maps->keys = NULL;
    varMap root;
    iniVarMap(&root);
    writeVarMaps(maps , &root);

}


void declareKeyValue(varMaps* maps , char* key , int value , Token* errorToken , dataType type){
    int ma = 0;
    for(char* it = key ; *it!='\0' ; it++){
        int i = getMapInt(*it);
        if(i<0)compileError(errorToken , "invalid character");
        if(maps->maps[ma].child[i]<0){
            varMap mao;
            iniVarMap(&mao);
            int newId = writeVarMaps(maps , &mao);
            maps->maps[ma].child[i] = newId;
        }
        ma = maps->maps[ma].child[i];
    }
    if(maps->maps[ma].isValue)compileError(errorToken , "identifier already declared");
    maps->maps[ma].isValue = 1;
    maps->maps[ma].value = value;
    maps->maps[ma].type = type;
}
void deleteKeyValue(varMaps* maps , char* key , Token* errorToken){
    int ma = 0;
    for(char* it =key ; *it!='\0' ; it++){
        int i = getMapInt(*it);
        if(i<0)compileError(errorToken , "invalid character");
        if(maps->maps[ma].child[i]<0)compileError(errorToken , "identifier is not defined");
        ma = maps->maps[ma].child[i];
    }
    maps->maps[ma].isValue =0;
}


int getKeyValue(varMaps* maps , char* key , Token* errorToken){
    int ma = 0;
    for(char* it = key ; *it!='\0' ; it++){
        int i = getMapInt(*it);
        if(i<0)compileError(errorToken , "invalid character");
        if(maps->maps[ma].child[i]<0)compileError(errorToken , "identifier is not defined");
        ma = maps->maps[ma].child[i];
    }
    if(! maps->maps[ma].isValue)compileError(errorToken , "identifier is not defined");
    return maps->maps[ma].value;
}

bool isKeyValue(varMaps* maps , char* key , Token* errorToken){
    int ma = 0;
    for(char* it = key ; *it!='\0' ; it++){
        int i = getMapInt(*it);
        if(i<0)compileError(errorToken , "invalid character");
        if(maps->maps[ma].child[i]<0)return 0;
        ma = maps->maps[ma].child[i];
    }
    if(maps->maps[ma].isValue)return 1;
    else return 0;
}

dataType getDataTypeKeyValue(varMaps* maps , char* key , Token* errorToken){
    int ma = 0 ;
    for(char* it = key ; *it!='\0' ; it++){
        int i = getMapInt(*it);
        if(i<0)compileError(errorToken , "invalid character");
        if(maps->maps[ma].child[i]<0)compileError(errorToken , "identifier is not defined");
        ma = maps->maps[ma].child[i];
    }
    if(!maps->maps[ma].isValue)compileError(errorToken , "identifier is not defined");
    return maps->maps[ma].type;
}

void unload(Chunk* chunk , int x , Token* errorToken){
    for(int i=x ; i!=chunk->varCount ; i++)deleteKeyValue(&chunk->vars , getKey(&chunk->vars , i) , errorToken);
    chunk->varCount = x;
}


int writeNodes(calcNodes* nodes , calcNode* node){
    if(nodes->capacity==nodes->count){
        int oldCap = nodes->capacity;
        nodes->capacity = growCapacity(oldCap);
        nodes->node = growArray(calcNode, nodes->node , oldCap, nodes->capacity);        
    }
    nodes->node[nodes->count] = *node;
    nodes->count++;
    return (nodes->count -1);
}


int isOperator(tokenType type){
    switch(type){
        // logical operators
        case TOKEN_OR_OR: return 1;
        case TOKEN_AND_AND : return 2;

        // comparison operators
        case TOKEN_EQUAL_EQUAL :
        case TOKEN_BANG_EQUAL:
        case TOKEN_LESSER:
        case TOKEN_GREATER:
        case TOKEN_LESSER_EQUAL:
        case TOKEN_GREATER_EQUAL : return 3;

        // arithmetic operators
        case TOKEN_PLUS :
        case TOKEN_MINUS : return 4;

        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:return 5;

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


// bool objValue(Token** tok , Value* val){
//     Token* token = *tok;

//     *tok = token;
//     return (token->type == TOKEN_IDENTIFIER);
// }



// int buildBinTree(Token** tok,calcNodes* nodes , bool fr, bool comma ){
//     Token* token = *tok;
//     int current;
//     Value tempVal;
//     bool bra=0;
//     if((token->type==TOKEN_LEFT_PAREN)&&(!fr)){
//         token++;
//         bra=1;
//     }

//     if((token->type==TOKEN_LEFT_PAREN)){
//         current = buildBinTree(&token , nodes , 0 ,0);
//     }
//     else if(isOperator(token->type)){
//         calcNode temp;
//         iniNode(&temp);
//         current = writeNodes(nodes , &temp);
//         nodes->node[current].isleaf=0;
//         nodes->node[current].val = token;
        
//         nodes->node[current].left= writeNodes(nodes , &temp);
//         token++;
//         if(token->type==TOKEN_LEFT_PAREN){
//             nodes->node[current].right =  buildBinTree(&token, nodes , 0,0);
//             nodes->node[nodes->node[current].right].parent = current;
//         }
//         else if(isValue(token->type)){
//             temp.val = token;
//             temp.isleaf = 1;
//             nodes->node[current].right = writeNodes(nodes , &temp);
//             nodes->node[nodes->node[current].right].parent = current;
//             token++;
//         }
//         else if(token->type==TOKEN_IDENTIFIER){
//             temp.val = token;
//             temp.isleaf = 1;
//             nodes->node[current].right = writeNodes(nodes , &temp);
//             nodes->node[nodes->node[current].right].parent = current;
//             token++;
//             bool balance = 0;
//             if(token->type==TOKEN_LEFT_PAREN){
//                 token++;
//                 balance =1;
//             }
//             while(balance){
//                 if(token->type==TOKEN_LEFT_PAREN)balance++;
//                 else if(token->type==TOKEN_RIGHT_PAREN)balance--;
//                 token++;
//             }
//         }
//         else compileError(token , "Expected a valid value");
        
//     }
//     else if(isValue(token->type)) {
//         calcNode temp;
//         iniNode(&temp);
//         temp.isleaf = 1;
//         temp.val = token;
//         current = writeNodes(nodes , &temp);
//         token++;
//     }
//     else if(token->type==TOKEN_IDENTIFIER){
//         calcNode temp;
//         iniNode(&temp);
//         temp.isleaf = 1;
//         temp.val = token;
//         current = writeNodes(nodes , &temp);
//         token++;
//         int balance = 0;
//         if(token->type==TOKEN_LEFT_PAREN){
//             balance =1;
//             token++;
//         }
//         while(balance){
//             if(token->type==TOKEN_LEFT_PAREN)balance++;
//             else if(token->type==TOKEN_RIGHT_PAREN)balance--;
//             token++;
//         }
//     }
//     else {
//         compileError(token , "Invalid syntax");
//     }


//     while((token->type !=TOKEN_EOL)&&(token->type != TOKEN_SEMICOLON)&&((fr&& (!comma))||(token->type!=TOKEN_RIGHT_PAREN))&&((!comma)||(token->type !=TOKEN_COMMA))){
//         if(!isOperator(token->type))compileError(token , "expected a valid operator");
//         int cur = isOperator(token->type);
//         Token* ty = token;
//         token++;
//         // while((current->parent != NULL)){
//         while(nodes->node[current].parent != -1){
//             // int par = isOperator(current->parent->val->type);
//             int par = isOperator(nodes->node[nodes->node[current].parent].val->type);
//             if(cur<=par){
//                 // current = current->parent;
//                 current = nodes->node[current].parent;
//             }
//             else break;
//         }
//         int rightIndex = -1;
//         calcNode rightNode;
//         iniNode(&rightNode);
//         if(token->type==TOKEN_LEFT_PAREN){
//             rightIndex= buildBinTree(&token , nodes , 0 , 0);
//         }
        
//         else if(isValue(token->type)){
//             rightNode.isleaf=1;
//             rightNode.val = token;
//             rightIndex = writeNodes(nodes , &rightNode);
//             token++;
//         }
//         else if(token->type==TOKEN_IDENTIFIER){
//             rightNode.isleaf = 1;
//             rightNode.val = token;

//             token++;
//             int balance =0;
//             if(token->type==TOKEN_LEFT_PAREN){
//                 balance=1;
//                 token++;
//             }
//             while(balance){
//                 if(token->type==TOKEN_LEFT_PAREN)balance++;
//                 else if(token->type==TOKEN_RIGHT_PAREN)balance--;
//                 token++;
//             }
//             rightIndex = writeNodes(nodes , &rightNode);
//         }
//         else compileError(token , "expected a valid value");


//         // if(current->parent==NULL){.
//         if(nodes->node[current].parent == -1){
//             calcNode newParent;
//             newParent.isleaf=0;
//             newParent.parent= -1;
//             newParent.val = ty;
//             newParent.left = current;
//             newParent.right = rightIndex;
//             // current->parent = writeNodes(nodes , &newParent);
//             int newParentIndex = writeNodes(nodes , &newParent);
//             nodes->node[current].parent = newParentIndex;
//             // current->parent->right->parent = current->parent;
//             // current = current->parent->right;
//             // nodes->node[nodes->node[nodes->node[current].parent].right].parent = nodes->node[current].parent;
//             // current = nodes->node[nodes->node[current].parent].right;
//             nodes->node[current].parent = newParentIndex;
//             nodes->node[rightIndex].parent = newParentIndex;
//             current = rightIndex;
//         }
//         else {
            
//             calcNode newNode;
//             iniNode(&newNode);
//             newNode.isleaf = 0;
//             newNode.val = ty;
//             // newNode.parent = current->parent;
//             newNode.parent = nodes->node[current].parent;

//             newNode.left = current;
//             newNode.right = rightIndex;

//             // current->parent = writeNodes(nodes , &newNode);
//             // current = current->parent;
//             // current->parent->right = current;
//             // current->right->parent = current;
//             // current = current->right;
//             int newNodeIndex = writeNodes(nodes , &newNode);
//             nodes->node[current].parent =newNodeIndex;
//             nodes->node[nodes->node[current].parent].right = newNodeIndex;
//             nodes->node[current].parent = newNodeIndex;
//             nodes->node[rightIndex].parent = newNodeIndex;
//             current = rightIndex;

//         }

//     }
//     if(bra){
//         if(token->type!=TOKEN_RIGHT_PAREN)compileError(token , "right paren not found");
//         token++;
//     }
//     if(fr&& (!comma)) while(!((token->type==TOKEN_SEMICOLON)||(token->type==TOKEN_EOL)))token++;
//     // while(current->parent!=NULL)current = current->parent;
//     while(nodes->node[current].parent != -1)current = nodes->node[current].parent;
//     *tok = token;
//     return current;
// }
int buildBinTree(Token** tok,calcNodes* nodes , bool fr, bool comma ){
    Token* token = *tok;
    int current;
    Value tempVal;
    bool bra=0;
    if((token->type==TOKEN_LEFT_PAREN)&&(!fr)){
        token++;
        bra=1;
    }

    if((token->type==TOKEN_LEFT_PAREN)){
        current = buildBinTree(&token , nodes , 0 ,0);
    }
    else if(isOperator(token->type)){
        calcNode temp;
        iniNode(&temp);
        current = writeNodes(nodes , &temp);
        nodes->node[current].isleaf=0;
        nodes->node[current].val = token;
        
        nodes->node[current].left= writeNodes(nodes , &temp);
        token++;
        if(token->type==TOKEN_LEFT_PAREN){
            nodes->node[current].right =  buildBinTree(&token, nodes , 0,0);
            nodes->node[nodes->node[current].right].parent = current;
        }
        else if(isValue(token->type)){
            temp.val = token;
            temp.isleaf = 1;
            nodes->node[current].right = writeNodes(nodes , &temp);
            nodes->node[nodes->node[current].right].parent = current;
            token++;
        }
        else if(token->type==TOKEN_IDENTIFIER){
            temp.val = token;
            temp.isleaf = 1;
            nodes->node[current].right = writeNodes(nodes , &temp);
            nodes->node[nodes->node[current].right].parent = current;
            token++;
            bool balance = 0;
            if(token->type==TOKEN_LEFT_PAREN){
                token++;
                balance =1;
            }
            while(balance){
                if(token->type==TOKEN_LEFT_PAREN)balance++;
                else if(token->type==TOKEN_RIGHT_PAREN)balance--;
                token++;
            }
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
    else if(token->type==TOKEN_IDENTIFIER){
        calcNode temp;
        iniNode(&temp);
        temp.isleaf = 1;
        temp.val = token;
        current = writeNodes(nodes , &temp);
        token++;
        int balance = 0;
        if(token->type==TOKEN_LEFT_PAREN){
            balance =1;
            token++;
        }
        while(balance){
            if(token->type==TOKEN_LEFT_PAREN)balance++;
            else if(token->type==TOKEN_RIGHT_PAREN)balance--;
            token++;
        }
    }
    else {
        compileError(token , "Invalid syntax");
    }


    while((token->type !=TOKEN_EOL)&&(token->type != TOKEN_SEMICOLON)&&((fr&& (!comma))||(token->type!=TOKEN_RIGHT_PAREN))&&((!comma)||(token->type !=TOKEN_COMMA))){
        if(!isOperator(token->type))compileError(token , "expected a valid operator");
        int cur = isOperator(token->type);
        Token* ty = token;
        token++;
        // while((current->parent != NULL)){
        while(nodes->node[current].parent != -1){
            // int par = isOperator(current->parent->val->type);
            int par = isOperator(nodes->node[nodes->node[current].parent].val->type);
            if(cur<=par){
                // current = current->parent;
                current = nodes->node[current].parent;
            }
            else break;
        }
        calcNode rightNode;
        iniNode(&rightNode);
        if(token->type==TOKEN_LEFT_PAREN){
            int ntr = buildBinTree(&token , nodes , 0 , 0);
            rightNode =nodes->node[ntr];
        }
        
        else if(isValue(token->type)){
            rightNode.isleaf=1;
            rightNode.val = token;
            token++;
        }
        else if(token->type==TOKEN_IDENTIFIER){
            rightNode.isleaf = 1;
            rightNode.val = token;
            token++;
            int balance =0;
            if(token->type==TOKEN_LEFT_PAREN){
                balance=1;
                token++;
            }
            while(balance){
                if(token->type==TOKEN_LEFT_PAREN)balance++;
                else if(token->type==TOKEN_RIGHT_PAREN)balance--;
                token++;
            }
        }
        else compileError(token , "expected a valid value");


        // if(current->parent==NULL){.
        if(nodes->node[current].parent == -1){
            calcNode newParent;
            newParent.isleaf=0;
            newParent.parent= -1;
            newParent.val = ty;
            newParent.left = current;
            newParent.right = writeNodes(nodes , &rightNode);
            // current->parent = writeNodes(nodes , &newParent);
            nodes->node[current].parent = writeNodes(nodes , &newParent);
            // current->parent->right->parent = current->parent;
            // current = current->parent->right;
            nodes->node[nodes->node[nodes->node[current].parent].right].parent = nodes->node[current].parent;
            current = nodes->node[nodes->node[current].parent].right;
        }
        else {
            
            calcNode newNode;
            iniNode(&newNode);
            newNode.isleaf = 0;
            newNode.val = ty;
            // newNode.parent = current->parent;
            newNode.parent = nodes->node[current].parent;

            newNode.left = current;
            newNode.right = writeNodes(nodes , &rightNode);

            // current->parent = writeNodes(nodes , &newNode);
            // current = current->parent;
            // current->parent->right = current;
            // current->right->parent = current;
            // current = current->right;
            nodes->node[current].parent = writeNodes(nodes , &newNode);
            current = nodes->node[current].parent;
            nodes->node[nodes->node[current].parent].right = current;
            nodes->node[nodes->node[current].right].parent = current;
            current = nodes->node[current].right;

        }

    }
    if(bra){
        if(token->type!=TOKEN_RIGHT_PAREN)compileError(token , "right paren not found");
        token++;
    }
    if(fr&& (!comma)) while(!((token->type==TOKEN_SEMICOLON)||(token->type==TOKEN_EOL)))token++;
    // while(current->parent!=NULL)current = current->parent;
    while(nodes->node[current].parent != -1)current = nodes->node[current].parent;
    *tok = token;
    return current;
}

dataType tokenTypeToDataType(Token* token){
    switch(token->type){
        case TOKEN_INT : return DATA_INT;
        case TOKEN_FLOAT : return DATA_FLOAT;
        case TOKEN_STRING :  return DATA_STRING;
        default : return DATA_VOID;
    }
}

int pushValue(Token* token ,  Chunk* chunk ){
    Value value;
    iniValue(&value ,tokenTypeToDataType(token) );
    insertDataToValue(token , &value);
    return writeValueArray(&chunk->constants , &value);
}

bool isCmp(tokenType type){
    switch(type){
        case TOKEN_LESSER:
        case TOKEN_GREATER:
        case TOKEN_LESSER_EQUAL:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL:return 1;
        default : return 0;
    }
}

int addFlag(Chunk* chunk){
    chunk->flags.flagCount++;
    return chunk->flags.flagCount -1;
}

void emitFlag(Chunk* chunk , int flag){
    if(chunk->flags.flagCount>=chunk->flags.flagCapacity){
        int oldCount = chunk->flags.flagCapacity;
        chunk->flags.flagCapacity = growCapacity(chunk->flags.flagCount);
        chunk->flags.flag = growArray(int , chunk->flags.flag , oldCount , chunk->flags.flagCapacity);
    }
    chunk->flags.flag[flag] = chunk->count;
}

void executeBinTree(int current , calcNodes* nodes , Chunk* chunk  , varMaps* maps , funcByte* func ,int gotoIfFalse , int gotoIfTrue){

    // if(current->isleaf){
    if(nodes->node[current].isleaf){
        if(nodes->node[current].val->type == TOKEN_IDENTIFIER){
            Token* it = nodes->node[current].val;
            it++;
            if(it->type==TOKEN_LEFT_PAREN){
                compileCallFunction(&(nodes->node[current].val) , chunk , func);
            }
            else {// variabale call
                bool local;
                char* identifier = tokenGetSource(nodes->node[current].val);
                if(isKeyValue(maps , identifier , nodes->node[current].val))local = 1;
                else local = 0;

                int value;
                if(local){
                    value = getKeyValue(maps , identifier , nodes->node[current].val);
                    writeChunk(chunk , OP_LOAD_VAR_LOCAL , nodes->node[current].val->line);
                }
                else {
                    value = getKeyValue(&func->global.vars , identifier , nodes->node[current].val);
                    writeChunk(chunk , OP_LOAD_VAR , nodes->node[current].val->line);
                }
                 writeChunk(chunk , value , nodes->node[current].val->line);
            }
        }
        else {
           writeChunk(chunk , OP_LOAD_CONSTANT ,nodes->node[current].val->line );
            int it = pushValue(nodes->node[current].val , chunk);
            writeChunk(chunk , it , nodes->node[current].val->line);
        }
        return;
    }
    if(nodes->node[nodes->node[current].left].val ==NULL){
        if((nodes->node[current].val->type != TOKEN_MINUS)&&(nodes->node[current].val->type !=TOKEN_PLUS))compileError(nodes->node[current].val , "invalid syntax");
        executeBinTree(nodes->node[current].right , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);
        if(nodes->node[current].val->type == TOKEN_MINUS)writeChunk(chunk , OP_NEGATE , nodes->node[current].val->line);
        return;
    }
    if((nodes->node[current].val->type == TOKEN_PLUS)||(nodes->node[current].val->type ==TOKEN_MINUS)||(nodes->node[current].val->type ==TOKEN_STAR)||(nodes->node[current].val->type == TOKEN_SLASH)||(nodes->node[current].val->type ==TOKEN_PERCENT)){

        executeBinTree(nodes->node[current].left  , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);
        executeBinTree(nodes->node[current].right , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);
        opcode op;
        switch(nodes->node[current].val->type){
            case TOKEN_PLUS : op=OP_ADD;break;
            case TOKEN_MINUS :op= OP_SUB;break;
            case TOKEN_STAR : op = OP_MUL;break;
            case TOKEN_SLASH : op = OP_DIV;break;
            case TOKEN_PERCENT : op = OP_MOD;break;
        }
        writeChunk(chunk , op , nodes->node[current].val->line);
        return;
    }
    if((gotoIfFalse>=0)&&(nodes->node[current].val->type == TOKEN_AND_AND)){
        int newFlag = addFlag(chunk);
        executeBinTree(nodes->node[current].left , nodes, chunk , maps , func , gotoIfFalse , newFlag);

        if(!isCmp(nodes->node[nodes->node[current].left].val->type)){
            writeChunk(chunk , OP_GOTO_IF_FALSE , nodes->node[current].val->line);
            writeChunk(chunk , gotoIfFalse , nodes->node[current].val->line);
        }

        emitFlag(chunk , newFlag);

        executeBinTree(nodes->node[current].right , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);

        if(!isCmp(nodes->node[nodes->node[current].right].val->type)){
            writeChunk(chunk , OP_GOTO_IF_FALSE , nodes->node[current].val->line);
            writeChunk(chunk , gotoIfFalse , nodes->node[current].val->line);
        }

        writeChunk(chunk ,OP_GOTO ,nodes->node[current].val->line );
        writeChunk(chunk , gotoIfTrue , nodes->node[current].val->line);
        return;
        
    }
    else if((gotoIfTrue>=0)&&(nodes->node[current].val->type == TOKEN_OR_OR)){
        int newFlag = addFlag(chunk);
        executeBinTree(nodes->node[current].left , nodes , chunk , maps , func , newFlag , gotoIfTrue);

        if(!isCmp(nodes->node[nodes->node[current].left].val->type)){
            writeChunk(chunk , OP_GOTO_IF_TRUE , nodes->node[current].val->line);
            writeChunk(chunk , gotoIfTrue , nodes->node[current].val->line);
        }

        emitFlag(chunk , newFlag);
        executeBinTree(nodes->node[current].right , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);

        if(!isCmp(nodes->node[nodes->node[current].right].val->type)){
            writeChunk(chunk , OP_GOTO_IF_FALSE , nodes->node[current].val->line);
            writeChunk(chunk , gotoIfFalse , nodes->node[current].val->line);
        }

        writeChunk(chunk , OP_GOTO , nodes->node[current].val->line);
        writeChunk(chunk , gotoIfTrue , nodes->node[current].val->line);
        return;
    }
    else if(isCmp(nodes->node[current].val->type)){
        executeBinTree(nodes->node[current].left , nodes , chunk ,maps , func , gotoIfFalse , gotoIfTrue);
        executeBinTree(nodes->node[current].right , nodes , chunk , maps , func , gotoIfFalse , gotoIfTrue);

        opcode op;
        switch(nodes->node[current].val->type){
            case TOKEN_GREATER : op = OP_GREATER;break;
            case TOKEN_LESSER : op = OP_LESSER;break;
            case TOKEN_GREATER_EQUAL : op = OP_GREAT_EQUAL;break;
            case TOKEN_LESSER_EQUAL : op = OP_LESS_EQUAL;break;
            case TOKEN_EQUAL_EQUAL : op= OP_EQUAL_EQUAL ; break;
            case TOKEN_BANG_EQUAL : op = OP_NOT_EQUAL ; break;
        }
        writeChunk(chunk , op , nodes->node[current].val->line);

        if(gotoIfFalse>=0){
            writeChunk(chunk , OP_GOTO_IF_FALSE , -1);
            writeChunk(chunk , gotoIfFalse , -1);
        }
        if(gotoIfTrue>=0){
            writeChunk(chunk , OP_GOTO , -1);
            writeChunk(chunk , gotoIfTrue , -1);
        }
        return;
    }

    // compileError(current->val ,"invalid syntax");
    compileError(nodes->node[current].val , "invalid syntax");
}

dataType tokenToDataType(Token* token){
    char* dataName = tokenGetSource(token);
    char* dataChar[] = {"int" , "string" , "char" , "bool" , "float" , "vector" , "void"};
    dataType dt[] = {DATA_INT , DATA_STRING , DATA_CHAR , DATA_BOOL , DATA_FLOAT , DATA_VECTOR , DATA_VOID};
    
    for(int i=0 ; i<7 ; i++)if(tokenEqual(dataChar[i] , token->start , token->end))return dt[i];
    compileError(token , "Invalid data type");
}
void compileCallFunction(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    char* funcName = tokenGetSource(token);
    int it = getKeyValue(&func->vars , funcName , token);
    Chunk* cnk = func->func+ it;
    token++;
    if(token->type != TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
    token++;
    for(int i=0 ; i!=cnk->paraCount ; i++){
        calcNodes nodes;
        iniNodes(&nodes);
        int current = buildBinTree(&token  , &nodes , 1,1);
        executeBinTree(current , &nodes , chunk  , &chunk->vars , func , -1 , -1);
        if(token->type == TOKEN_RIGHT_PAREN)break;
        if(token->type !=TOKEN_COMMA)compileError(token , "invalid syntax");
        token++;
    }
    if(token->type != TOKEN_RIGHT_PAREN)compileError(token , "invalid syntax");
    writeChunk(chunk, OP_CALL , token->line);
    writeChunk(chunk , it, token->line);
    
    token++;
    *tok = token;
}
void compileReturn(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    token++;
    calcNodes nodes;
    iniNodes(&nodes);
    int current = buildBinTree(&token  , &nodes , 1 , 0 );
    executeBinTree(current , &nodes , chunk  , &chunk->vars , func , -1 , -1);
    writeChunk(chunk , OP_RETURN , token->line);
    *tok = token;
}

void datastructures(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    dataType type = tokenToDataType(token);
    token++;
    if(token->type!=TOKEN_IDENTIFIER)compileError(token , "expected a identifier");
    char* key = tokenGetSource(token);
    declareKeyValue(&chunk->vars ,key , chunk->varCount , token , type);
    int val = chunk->varCount;
    writeKey(&chunk->vars , key , val);
    chunk->varCount++;
    writeChunk(chunk , OP_DECLARE , token->line);
    writeChunk(chunk , type , token->line);
    writeChunk(chunk , val , token->line);
    token++;
    if(token->type!=TOKEN_EQUAL)compileError(token , "invalid syntax");
    token++;
    calcNodes nodes;
    iniNodes(&nodes);
    int current=buildBinTree(&token , &nodes , 1 , 0);
    executeBinTree(current , &nodes ,chunk , &chunk->vars , func , -1 , -1);

    writeChunk(chunk , OP_STORE_LOCAL ,  token->line);
    writeChunk(chunk , val , token->line);   
    token++; 
    *tok = token;
}

// void identifier(Token** tok , Chunk* chunk , funcByte* func){
//     Token* token = *tok;
//     char* key = tokenGetSource(token);
//     int it = getKeyValue(&chunk->vars , key , token);
//     dataType type = getDataTypeKeyValue(&chunk->vars , key , token);
//     token++;
//     Token* sign = token;
//     token++;
//     switch(sign->type){
//         case TOKEN_EQUAL : {
//             Nodes nodes;
//             iniNodes(&nodes);
//             int cnt=0;
//             for(Token* t = token ; (t->type != TOKEN_SEMICOLON)&&(t->type != TOKEN_EOL) ; t++)cnt++;
//             nodes.capacity = cnt*3 + 8;
//             nodes.node = growArray(calcNode , NULL , 0 ,nodes.capacity);
//             calcNode* current = buildBinTree(&token ,type , &nodes , 1 , 0);
//             executeBinTree(current , chunk , type , &chunk->vars , func);
//         }
//     }
//     writeChunk(chunk , OP_STORE_LOCAL , token->line);
//     writeChunk(chunk , it , token->line);
// }

void identifier(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    char* key = tokenGetSource(token);
    if((token+1)->type==TOKEN_LEFT_PAREN){
        compileCallFunction(&token , chunk, func);
    }
    else {
        bool local;
        if(isKeyValue(&chunk->vars , key , token))local=1;
        else if(isKeyValue(&func->global.vars , key , token))local=0;
        else getKeyValue(&chunk->vars , key , token); // compile error , coz key does not exist

        if(local){
            int it = getKeyValue(&chunk->vars , key , token);
            dataType type = getDataTypeKeyValue(&chunk->vars , key , token);
            token++;
            Token* sign = token;
            token++;
            switch(sign->type){
                case TOKEN_EQUAL : {
                    calcNodes nodes;
                    iniNodes(&nodes);
                    int current = buildBinTree(&token  ,&nodes , 1 , 0);
                    executeBinTree(current, &nodes ,  chunk , &chunk->vars , func , -1 , -1);
                    break;
                }
                case TOKEN_PLUS_EQUAL:{
                    calcNodes nodes;
                    iniNodes(&nodes);
                    int current =buildBinTree(&token , &nodes , 1 , 0);
                    writeChunk(chunk , OP_LOAD_VAR_LOCAL , token->line);
                    writeChunk(chunk , it , token->line);
                    executeBinTree(current , &nodes , chunk , &chunk->vars , func , -1 , -1);
                    writeChunk(chunk , OP_ADD , token->line);
                    break;
                }
                case TOKEN_MINUS_EQUAL:{
                    calcNodes nodes;
                    iniNodes(&nodes);
                    int current = buildBinTree(&token , &nodes , 1,  0);
                    writeChunk(chunk , OP_LOAD_VAR_LOCAL , token->line);
                    writeChunk(chunk , it , token->line);
                    executeBinTree(current , &nodes , chunk , &chunk->vars , func , -1 , -1);
                    writeChunk(chunk , OP_SUB , token->line);
                    break;
                }
            }
            writeChunk(chunk , OP_STORE_LOCAL , token->line);
            writeChunk(chunk , it , token->line);
            
        }
        else {
            int it =getKeyValue(&func->global.vars , key , token);
            dataType type = getDataTypeKeyValue(&func->global.vars , key , token);
            token++;
            Token* sign = token;
            token++;
            switch(sign->type){
                case TOKEN_EQUAL :{
                    calcNodes nodes;
                    iniNodes(&nodes);
                    
                    int current = buildBinTree(&token  , &nodes ,1 , 0);
                    executeBinTree(current ,&nodes , chunk , &func->global.vars , func , -1 , -1);
                    break;
                }
                case TOKEN_PLUS_EQUAL:{
                    calcNodes nodes;
                    iniNodes(&nodes);
                    int current = buildBinTree(&token , &nodes , 1 , 0);
                    writeChunk(chunk , OP_LOAD_VAR , token->line);
                    writeChunk(chunk , it , token->line);
                    executeBinTree(current , &nodes , chunk , &func->global.vars , func , -1 , -1);
                    writeChunk(chunk , OP_ADD , token->line);
                    break;
                }
                case TOKEN_MINUS_EQUAL:{
                    calcNodes nodes;
                    iniNodes(&nodes);
                    int current = buildBinTree(&token, &nodes , 1 , 0);
                    writeChunk(chunk , OP_LOAD_VAR , token->line);
                    writeChunk(chunk , it , token->line);
                    executeBinTree(current , &nodes , chunk , &func->global.vars , func , -1 , -1);
                    writeChunk(chunk , OP_SUB , token->line);
                    break;
                }
            }
            writeChunk(chunk , OP_STORE , token->line);
            writeChunk(chunk , it , token->line);
        }
    }
    token++;
    *tok = token;
}

void compilePrint(Token** tok , Chunk* chunk , funcByte* func){
    Token *token = *tok;
    calcNodes nodes;
    iniNodes(&nodes);
    token++;
    int current = buildBinTree(&token , &nodes , 1 , 0);
    executeBinTree(current , &nodes, chunk , &chunk->vars , func , -1 , -1);
    writeChunk(chunk , OP_PRINT , token->line);
    while(token->type !=TOKEN_SEMICOLON)token++;
    token++;
    *tok = token;
}

void compileWhile(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    int varCount = chunk->varCount;
    if(token->type != TOKEN_WHILE)compileError(token , "invalid syntax");
    token++;
    if(token->type != TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
    int falseFlag = addFlag(chunk);
    int trueFlag = addFlag(chunk);
    int startFlag = addFlag(chunk);
    emitFlag(chunk , startFlag);

    calcNodes nodes;
    iniNodes(&nodes);
    int current = buildBinTree(&token ,&nodes , 0 ,0 );
    executeBinTree(current , &nodes , chunk , &chunk->vars , func , falseFlag , trueFlag );

    emitFlag(chunk , trueFlag);
    if(token->type==TOKEN_LEFT_BRACE)compileNewBranch(&token , chunk , func);
    else compileSingleLine(&token , chunk , func);
    writeChunk(chunk , OP_GOTO , -1);
    writeChunk(chunk , startFlag , -1);
    emitFlag(chunk , falseFlag);    
    unload(chunk , varCount , token);
    writeChunk(chunk , OP_UNLOAD , -1);
    writeChunk(chunk , varCount , -1);
    *tok = token;
}

// void compileFor(Token** tok , Chunk* chunk , funcByte* func){
//     Token* token = *tok;
//     int varCount = chunk->varCount;
//     if(token->type != TOKEN_FOR)compileError(token , "invalid syntax");
//     token++;
//     if(token->type !=TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
//     token++;
//     if(token->type!=TOKEN_SEMICOLON)compileSingleLine(&token , chunk , func);
//     else token++;

//     int falseFlag = addFlag(chunk);
//     int trueFlag = addFlag(chunk);
//     int startFlag = addFlag(chunk);
//     emitFlag(chunk , startFlag);
//     calcNodes nodes;
//     iniNodes(&nodes);
//     int current = buildBinTree(&token , &nodes , 1 , 0);
//     executeBinTree(current , &nodes , chunk ,&chunk->vars , func , falseFlag , trueFlag);
//     token++;
//     if(token->type !=TOKEN_RIGHT_PAREN)identifier(&token , chunk , func);
//     else token++;

//     emitFlag(chunk , trueFlag);
//     if(token->type ==TOKEN_LEFT_BRACE)compileNewBranch(&token , chunk , func);
//     else compileSingleLine(&token , chunk , func);
//     writeChunk(chunk , OP_GOTO , -1);
//     writeChunk(chunk , startFlag , -1);
//     emitFlag(chunk , falseFlag);
//     writeChunk(chunk , OP_UNLOAD , -1);
//     writeChunk(chunk , varCount , -1);
//     unload(chunk , varCount , token);
// }

void compileFor(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    int varCount = chunk->varCount;
    if(token->type !=TOKEN_FOR)compileError(token , "invalid syntax");
    token++;
    if(token->type !=TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
    token++;

    if(token->type !=TOKEN_SEMICOLON)compileSingleLine(&token , chunk , func);
    else token++;

    int falseFlag = addFlag(chunk);
    int trueFlag = addFlag(chunk);
    int startFlag = addFlag(chunk);
    emitFlag(chunk , startFlag);

    calcNodes nodes;
    iniNodes(&nodes);
    int current = buildBinTree(&token , &nodes , 1 , 0);
    executeBinTree(current , &nodes ,chunk , &chunk->vars , func , falseFlag , trueFlag);
    if(token->type ==TOKEN_SEMICOLON )token++;
    bool isinc;
    if(token->type==TOKEN_RIGHT_PAREN)isinc=0;
    else isinc =1;
    Token* inc = token; 
    int bal=1;
    while(bal&&(token->type !=TOKEN_EOL)){
        if(token->type==TOKEN_LEFT_PAREN)bal++;
        else if(token->type == TOKEN_RIGHT_PAREN)bal--;
        token++;
    }
    if(token->type == TOKEN_EOL)compileError(token , "invalid syntax");
    emitFlag(chunk , trueFlag);
    Token* incEnd = token-1;
    if(token->type ==TOKEN_LEFT_BRACE)compileNewBranch(&token , chunk , func);
    else compileSingleLine(&token , chunk , func);
    incEnd->type = TOKEN_SEMICOLON;
    if(isinc)identifier(&inc , chunk , func);
    incEnd->type = TOKEN_RIGHT_PAREN;
    writeChunk(chunk , OP_GOTO , -1);
    writeChunk(chunk , startFlag , -1);
    emitFlag(chunk , falseFlag);
    writeChunk(chunk , OP_UNLOAD , -1);
    writeChunk(chunk , varCount , -1);
    unload(chunk , varCount , token);
    *tok = token;
}


void compileIf(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    if(token->type!=TOKEN_IF)compileError(token , "Invalid syntax");
    token++;
    int varCount = chunk->varCount;
    if(token->type!= TOKEN_LEFT_PAREN)compileError(token , "Invalid syntax");
    calcNodes nodes;
    iniNodes(&nodes);
    // token++;
    int current = buildBinTree(&token , &nodes , 0 , 0);
    int trueFlag = addFlag(chunk);
    int falseFlag = addFlag(chunk);
    int endFlag = addFlag(chunk);
    executeBinTree(current , &nodes , chunk , &chunk->vars , func , falseFlag , trueFlag);
    
    emitFlag(chunk , trueFlag);
    if(token->type==TOKEN_LEFT_BRACE)compileNewBranch(&token, chunk , func);
    else compileSingleLine(&token , chunk , func);
    
    if(token->type == TOKEN_ELSE){
        writeChunk(chunk , OP_GOTO , -1);
        writeChunk(chunk , endFlag , -1);
    }
    unload(chunk , varCount , token);

    bool eif =0;
    bool hel = 1;
    while(token->type == TOKEN_ELSE){
        hel=0;
        if(eif)compileError(token , "invalid syntax");
        emitFlag(chunk , falseFlag);
        token++;
        if(token->type == TOKEN_IF){
            token++;
            if(token->type != TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
            // token++;
            calcNodes subnodes;
            iniNodes(&subnodes);
            int subcurrent = buildBinTree(&token , &subnodes , 0 , 0);
            falseFlag = addFlag(chunk);
            trueFlag = addFlag(chunk);
            executeBinTree(subcurrent, &subnodes , chunk , &chunk->vars , func ,falseFlag , trueFlag);
            emitFlag(chunk ,trueFlag);
            while((token->type !=TOKEN_RIGHT_PAREN)&&(token->type !=TOKEN_EOL))token++;
            if(token->type ==TOKEN_EOL)compileError(token , "invalid syntax");
            token++;
        }
        else eif=1;
        if(token->type==TOKEN_LEFT_BRACE)compileNewBranch(&token , chunk , func);
        else compileSingleLine(&token , chunk , func);
        if(token->type == TOKEN_ELSE){
            writeChunk(chunk , OP_GOTO , -1);
            writeChunk(chunk , endFlag , -1);
        }
        unload(chunk , varCount , token);
        // token++;
    }
    if(hel)emitFlag(chunk , falseFlag);
    emitFlag(chunk , endFlag);
    writeChunk(chunk , OP_UNLOAD , -1);
    writeChunk(chunk , varCount , -1);
    unload(chunk , varCount , token);
    *tok = token;
}



// line 2 | length 2 | TOKEN_IF | if
// line 2 | length 1 | TOKEN_LEFT_PAREN | (
// line 2 | length 1 | TOKEN_INT | 4
// line 2 | length 1 | TOKEN_GREATER | >
// line 2 | length 1 | TOKEN_INT | 2
// line 2 | length 1 | TOKEN_RIGHT_PAREN | )
// line 2 | length 1 | TOKEN_LEFT_BRACE | {
// line 3 | length 5 | TOKEN_PRINT | print
// line 3 | length 1 | TOKEN_INT | 2
// line 3 | length 1 | TOKEN_SEMICOLON | ;
// line 4 | length 1 | TOKEN_RIGHT_BRACE | }
// line 4 | length 4 | TOKEN_ELSE | else
// line 4 | length 2 | TOKEN_IF | if
// line 4 | length 1 | TOKEN_LEFT_PAREN | (
// line 4 | length 1 | TOKEN_INT | 4
// line 4 | length 1 | TOKEN_GREATER | >
// line 4 | length 1 | TOKEN_INT | 3
// line 4 | length 1 | TOKEN_RIGHT_PAREN | )
// line 4 | length 1 | TOKEN_LEFT_BRACE | {
// line 5 | length 5 | TOKEN_PRINT | print
// line 5 | length 1 | TOKEN_INT | 3
// line 5 | length 1 | TOKEN_SEMICOLON | ;
// line 6 | length 1 | TOKEN_RIGHT_BRACE | }
// line 6 | length 4 | TOKEN_ELSE | else
// line 6 | length 1 | TOKEN_LEFT_BRACE | {
// line 7 | length 5 | TOKEN_PRINT | print
// line 7 | length 1 | TOKEN_INT | 4
// line 7 | length 1 | TOKEN_SEMICOLON | ;
// line 8 | length 1 | TOKEN_RIGHT_BRACE | }

void compileNewBranch(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    if(token->type != TOKEN_LEFT_BRACE)compileError(token , "invalid syntax");
    token++;
    int varCount = chunk->varCount;
    int bal = 1;
    Token* end = token;
    while(bal&&(end->type != TOKEN_EOL)){
        if(end->type == TOKEN_LEFT_BRACE)bal++;
        else if(end->type == TOKEN_RIGHT_BRACE){
            bal--;
            if(bal==0)break;
        }
        end++;
    }
    if(token->type == TOKEN_EOL)compileError(token , "close brace not found");
    while(token !=end){
        switch(token->type){
            case TOKEN_DATA : {
                datastructures(&token , chunk , func);
                break;
            }
            case TOKEN_IDENTIFIER : {
                identifier(&token , chunk , func );
                break;
            }
            case TOKEN_PRINT :  {
                compilePrint(&token , chunk , func);
                break;
            }
            case TOKEN_IF:{
                compileIf(&token , chunk , func);
                break;
            }
            case TOKEN_RETURN :{
                compileReturn(&token , chunk , func);
                break;
            }
            case TOKEN_WHILE:{
                compileWhile(&token , chunk , func);
                break;
            }
                case TOKEN_FOR:{
                compileFor(&token , chunk , func);
                break;
            }
        }
        
    }
    token++;
    *tok = token;
}

void compileSingleLine(Token** tok , Chunk* chunk , funcByte* func){
    Token* token = *tok;
    switch(token->type){
        case TOKEN_DATA :{
            datastructures(&token , chunk , func);
            break;
        }
        case TOKEN_IDENTIFIER:{
            identifier(&token ,  chunk , func);
            break;
        }
        case TOKEN_PRINT:{
            compilePrint(&token , chunk , func);
            break;
        }
        case TOKEN_IF:{
            compilePrint(&token , chunk , func);
        }
        case TOKEN_RETURN:{
            compileReturn(&token , chunk , func);
            break;
        }
        case TOKEN_WHILE:{
            compileWhile(&token , chunk , func);
            break;
        }
        case TOKEN_FOR:{
            compileFor(&token , chunk , func);
            break;
        }
        default:{
            compileError(token , "Invalid syntax");
            break;
        }
    }
    *tok = token;
}


void compileGlobal(Tokens* global ,funcByte* func){
    func->global.name = "Global";
    Token* it=global->token;
    while(it->type!=TOKEN_EOL){
        switch(it->type){
            case TOKEN_DATA :{
                datastructures(&it , &func->global, func);
                break;
            }
            case TOKEN_IDENTIFIER :{
                identifier(&it , &func->global , func);
                break;
            }
            case TOKEN_PRINT : {
                compilePrint(&it , &func->global , func);
                break;
            }
            case TOKEN_IF : {
                compileIf(&it , &func->global , func);
                break;
            }
            case TOKEN_WHILE:{
                compileWhile(&it , &func->global , func);
                break;
            }
            case TOKEN_FOR:{
                compileFor(&it , &func->global , func);
                break;
            }
        }

    } 
    writeChunk(&func->global , OP_EXIT , -1);
}

void pushParameter(Chunk* chunk , parameter* pa){
    if(chunk->paraCount == chunk->paraCapacity){
        int oldCap = chunk->paraCapacity;
        chunk->paraCapacity = growCapacity(oldCap);
        chunk->paras = growArray(parameter , chunk->paras , oldCap , chunk->paraCapacity);
    }
    chunk->paras[chunk->paraCount] = *pa;
    chunk->paraCount++;
}

void compileFunc(Tokens* tokens, funcByte* func ){
    Token* token = tokens->token;
    token++;
    token++;
    if(token->type != TOKEN_IDENTIFIER)compileError(token , "expected a valid identifier");
    char* funcName = tokenGetSource(token);
    int it = getKeyValue(&func->vars , funcName , token);
    Chunk* chunk = func->func+it;
    while((token->type !=TOKEN_LEFT_BRACE)&&(token->type != TOKEN_EOL))token++;    
    if(token->type != TOKEN_LEFT_BRACE)compileError(token , "invalid syntax");
    compileNewBranch(&token , chunk , func);
}

void declareFunc(Tokens* tokens , funcByte* func){
    Token* token = tokens->token;
    Chunk chunk;
    initilizeChunk(&chunk);
    token++;
    if(token->type != TOKEN_DATA)compileError(token , "expected a valid data type");
    dataType returnValue = tokenToDataType(token);
    token++;
    if(token->type != TOKEN_IDENTIFIER)compileError(token , "expected a valid identifier");
    char* funcName = tokenGetSource(token);
    chunk.name = funcName;
    token++;
    if(token->type != TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
    token++;

    if(token->type != TOKEN_RIGHT_PAREN){
        while(token->type != TOKEN_EOL){
            parameter pa;
            if(token->type  != TOKEN_DATA)compileError(token , "expected a valid data type");
            pa.type = tokenToDataType(token);
            token++;
            if(token->type != TOKEN_IDENTIFIER) compileError(token , "expected a valid identifier");
            char* paraName = tokenGetSource(token);
            pa.index = chunk.varCount;
            declareKeyValue(&chunk.vars , paraName , chunk.varCount ,token , pa.type);
            chunk.varCount++;
            pushParameter(&chunk , &pa);
            writeChunk(&chunk , OP_DECLARE , token->line);
            writeChunk(&chunk , pa.type , token->line);
            writeChunk(&chunk , pa.index , token->line);
            token++;
            if(token->type== TOKEN_RIGHT_PAREN)break;
            if(token->type!=TOKEN_COMMA)compileError(token , "invalid syntax");
            token++;
        }
    }
    for(int i =chunk.paraCount -1 ; i>=0 ; i--){
        parameter* para = chunk.paras+i;
        writeChunk(&chunk , OP_STORE_LOCAL , token->line);
        writeChunk(&chunk , para->index , token->line);
    }

    declareKeyValue(&func->vars , funcName , func->funcCount, tokens->token , returnValue );
    if(func->funcCount == func->funcCapacity){
        int oldCap = func->funcCapacity;
        func->funcCapacity =  growCapacity(oldCap);
        func->func =growArray(Chunk , func->func , oldCap , func->funcCapacity);
    }
    func->func[func->funcCount] = chunk;
    func->funcCount++;
}


void compile(tokenFunctions* tf ,funcByte* func){
    iniFuncByte(func , tf->count);
    for(int it=0 ; it!=tf->count ; it++)declareFunc(tf->func+it , func);
    compileGlobal(&tf->mainFunc , func);
    for(int it=0 ; it!=tf->count ; it++)compileFunc(tf->func+it , func);
}