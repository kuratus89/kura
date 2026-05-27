
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

void iniVarMap(varMap* map){
    map->isValue = 0;
    map->value = -1;
    for(int i =0 ; i<62 ; i++)map->child[i] = -1;
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

void iniVarMaps(varMaps* maps){
    maps->capacity=0;
    maps->count=0;
    maps->maps = NULL;

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

calcNode* writeNodes(Nodes* nodes , calcNode* node){
    if(nodes->capacity==nodes->count){
        printf("Error whiile compiling\ncrashed due to insufficient memory for nodes");
        exit(26);
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


// bool objValue(Token** tok , Value* val){
//     Token* token = *tok;

//     *tok = token;
//     return (token->type == TOKEN_IDENTIFIER);
// }

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
        else if(token->type==TOKEN_IDENTIFIER){
            temp.val = token;
            temp.isleaf = 1;
            current->right = writeNodes(nodes , &temp);
            current->right->parent = current;
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
    return writeValueArray(&chunk->constants , &value);
}

void executeBinTree(calcNode* current , Chunk* chunk , dataType type , varMaps* maps){

    if(current->isleaf){
        if(current->val->type == TOKEN_IDENTIFIER){
            Token* it = current->val;
            it++;
            if(it->type==TOKEN_LEFT_PAREN){// function  call
                compileError(it , "function call not yet supported");
            }
            else {// variabale call
                char* identifier = tokenGetSource(current->val);
                int value = getKeyValue(maps , identifier , current->val);
                writeChunk(chunk , OP_LOAD_VAR , current->val->line);
                writeChunk(chunk , value , current->val->line);
            }
        }
        else {
            writeChunk(chunk , OP_LOAD_CONSTANT , current->val->line);
            int it = pushValue(current->val , chunk , type);
            writeChunk(chunk , it , current->val->line);
        }
        return;
    }
    if(current->left->val==NULL){
        if((current->val->type!=TOKEN_MINUS)&&(current->val->type!=TOKEN_PLUS))compileError(current->val , "Ivalid syntax");
        executeBinTree(current->right , chunk , type,  maps );
        writeChunk(chunk , OP_NEGATE , current->val->line);
        return;
    }
    
    executeBinTree(current->left , chunk , type , maps );
    executeBinTree(current->right , chunk , type , maps);
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
void CompileReturn(Token** tok , Chunk* chunk){
    Token* token = *tok;
    token++;
    Nodes nodes;
    iniNodes(&nodes);
    int tokenCount = 0;
    for(Token* t = token; (t->type !=TOKEN_SEMICOLON)&&(t->type  != TOKEN_EOL) ; t++)tokenCount++;
    nodes.capacity = tokenCount*3 +8;
    nodes.node = growArray(calcNode , nodes.node , 0 , nodes.capacity);
    calcNode* current = buildBinTree(&token ,chunk->returnType , &nodes , 1 );
    executeBinTree(current , chunk , chunk->returnType , &chunk->vars);
    writeChunk(chunk , OP_RETURN , token->line);
    *tok = token;
}

void datastructures(Token** tok , Chunk* chunk ){
    Token* token = *tok;
    dataType type = tokenToDataType(token);
    token++;
    if(token->type!=TOKEN_IDENTIFIER)compileError(token , "expected a identifier");
    char* key = tokenGetSource(token);
    declareKeyValue(&chunk->vars ,key , chunk->varCount , token , type);
    int val = chunk->varCount;
    chunk->varCount++;
    writeChunk(chunk , OP_DECLARE , token->line);
    writeChunk(chunk , type , token->line);
    writeChunk(chunk , val , token->line);
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
    executeBinTree(current ,chunk , type, &chunk->vars);

    writeChunk(chunk , OP_STORE ,  token->line);
    writeChunk(chunk , val , token->line);    
    *tok = token;
}

void identifier(Token** tok , Chunk* chunk){
    Token* token = *tok;
    char* key = tokenGetSource(token);
    int it = getKeyValue(&chunk->vars , key , token);
    dataType type = getDataTypeKeyValue(&chunk->vars , key , token);
    token++;
    Token* sign = token;
    token++;
    switch(sign->type){
        case TOKEN_EQUAL : {
            Nodes nodes;
            iniNodes(&nodes);
            int cnt=0;
            for(Token* t = token ; (t->type != TOKEN_SEMICOLON)&&(t->type != TOKEN_EOL) ; t++)cnt++;
            nodes.capacity = cnt*3 + 8;
            nodes.node = growArray(calcNode , NULL , 0 ,nodes.capacity);
            calcNode* current = buildBinTree(&token ,type , &nodes , 1);
            executeBinTree(current , chunk , type , &chunk->vars);
        }
    }
    writeChunk(chunk , OP_STORE , token->line);
    writeChunk(chunk , it , token->line);
}




void compileGlobal(Tokens* global ,funcByte* func){
    
    Token* it=global->token;
    while(it->type!=TOKEN_EOL){
        switch(it->type){
            case TOKEN_DATA :{
                datastructures(&it , &func->global);
                break;
            }
            case TOKEN_IDENTIFIER :{
                identifier(&it , &func->global);
                break;
            }
            
        }
        while((it->type!=TOKEN_SEMICOLON)&&(it->type != TOKEN_EOL))it++;
        if(it->type !=TOKEN_EOL)it++;
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

void compileFunc(Tokens* tokens, funcByte* func , int globalVarCount){
    Token* token = tokens->token;
    Chunk chunk;
    
    initilizeChunk(&chunk);
    chunk.varCount = globalVarCount;
    token++;
    if(token->type != TOKEN_DATA)compileError(token , "expected a valid data type");
    dataType returnValue = tokenToDataType(token);
    token++;
    if(token->type != TOKEN_IDENTIFIER)compileError(token , "expected a valid identifier");
    char* funcName = tokenGetSource(token);
    token++;
    if(token->type != TOKEN_LEFT_PAREN)compileError(token , "invalid syntax");
    token++;
    
    if(token->type != TOKEN_RIGHT_PAREN){while(token->type != TOKEN_EOL){
        parameter pa;
        if(token->type != TOKEN_DATA)compileError(token , "expected a valid data type");
        pa.type = tokenToDataType(token);
        token++;
        if(token->type != TOKEN_IDENTIFIER)compileError(token , "expected a valid identifier");
        char* paraName = tokenGetSource(token);
        pa.index = chunk.varCount;
        declareKeyValue(&chunk.vars , paraName , chunk.varCount , token , pa.type);
        pushParameter(&chunk , &pa);    
        token++;
        if(token->type == TOKEN_RIGHT_PAREN)break;
        if(token->type != TOKEN_COMMA)compileError(token , "invalid syntax");
        token++;
        
    }}
    token++;
    if(token->type != TOKEN_LEFT_BRACE)compileError(token , "invalid syntax");
    int bal = 1;
    Token* end = token;
    token++;
    while((bal)&&(end->type!=TOKEN_EOL)){
        end++;
        if(end->type==TOKEN_LEFT_BRACE)bal++;
        else if(end->type==TOKEN_RIGHT_BRACE)bal--; 
    }
    if(end->type==TOKEN_EOL)compileError(token , "invalid syntax");
    while(token!=end){
        switch(token->type){
            case TOKEN_DATA: {
                datastructures(&token , &chunk);
                break;
            }
            case TOKEN_IDENTIFIER :{
                identifier(&token , &chunk);
                break;
            }
            case TOKEN_RETURN :{
                compileReturn(&token , &chunk);
                break;
            }
        }
        while((token!=end)&&(token->type != TOKEN_SEMICOLON))token++;
        if(token!=end)token++;
    }

    if(func->funcCount==func->funcCapacity){
        int oldCap = func->funcCapacity;
        func->funcCapacity = growCapacity(oldCap);
        func->func = growArray(Chunk , func->func , oldCap , func->funcCapacity);
    }
    func->func[func->funcCount] =chunk;
    func->funcCount++;
}
// <=== add ===>
// Token count : 23
//  Token capacity : 23
//  Tokens :
// line 7 | length 4 | TOKEN_FUNC | func
// line 7 | length 3 | TOKEN_DATA | int
// line 7 | length 3 | TOKEN_IDENTIFIER | add
// line 7 | length 1 | TOKEN_LEFT_PAREN | (
// line 7 | length 3 | TOKEN_DATA | int
// line 7 | length 1 | TOKEN_IDENTIFIER | a
// line 7 | length 1 | TOKEN_COMMA | ,
// line 7 | length 3 | TOKEN_DATA | int
// line 7 | length 1 | TOKEN_IDENTIFIER | b
// line 7 | length 1 | TOKEN_RIGHT_PAREN | )
// line 7 | length 1 | TOKEN_LEFT_BRACE | {
// line 8 | length 3 | TOKEN_DATA | int
// line 8 | length 1 | TOKEN_IDENTIFIER | c
// line 8 | length 1 | TOKEN_EQUAL | =
// line 8 | length 1 | TOKEN_IDENTIFIER | a
// line 8 | length 1 | TOKEN_PLUS | +
// line 8 | length 1 | TOKEN_IDENTIFIER | b
// line 8 | length 1 | TOKEN_SEMICOLON | ;
// line 9 | length 6 | TOKEN_RETURN | return
// line 9 | length 1 | TOKEN_IDENTIFIER | c
// line 9 | length 1 | TOKEN_SEMICOLON | ;
// line 10 | length 1 | TOKEN_RIGHT_BRACE | }

void compile(tokenFunctions* tf ,funcByte* func){
    iniFuncByte(func , tf->count);
    compileGlobal(&tf->mainFunc , func);
    for(int it=0 ; it!=tf->count ; it++)compileFunc(tf->func+it , func , func->global.varCount);
}