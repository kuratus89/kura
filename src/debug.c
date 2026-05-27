#include "common.h"
#include "chunk.h"
#include "value.h"
#include "scanner.h"
#include "token.h"
#include "vm.h"
char* disassembleTokenType(tokenType type){
    switch(type){
        //SINGLE DIGIT SIGNS
        
        case TOKEN_LEFT_PAREN:        return("TOKEN_LEFT_PAREN");
        case TOKEN_RIGHT_PAREN:       return("TOKEN_RIGHT_PAREN");
        case TOKEN_LEFT_BRACE:        return("TOKEN_LEFT_BRACE");
        case TOKEN_RIGHT_BRACE:       return("TOKEN_RIGHT_BRACE");
        case TOKEN_LEFT_BRACKET:      return("TOKEN_LEFT_BRACKET");
        case TOKEN_RIGHT_BRACKET:     return("TOKEN_RIGHT_BRACKET");
        case TOKEN_COMMA:             return("TOKEN_COMMA");
        case TOKEN_DOT:               return("TOKEN_DOT");
        case TOKEN_STAR:              return("TOKEN_STAR");
        case TOKEN_MINUS:             return("TOKEN_MINUS");
        case TOKEN_PLUS:              return("TOKEN_PLUS");
        case TOKEN_GREATER:           return("TOKEN_GREATER");
        case TOKEN_LESSER:            return("TOKEN_LESSER");
        case TOKEN_SLASH:             return("TOKEN_SLASH");
        case TOKEN_SEMICOLON:         return("TOKEN_SEMICOLON");
        case TOKEN_EQUAL:             return("TOKEN_EQUAL");
        case TOKEN_HASH:              return("TOKEN_HASH");
        case TOKEN_PERCENT:           return("TOKEN_PERCENT");
        case TOKEN_BANG:              return("TOKEN_BANG");
        case TOKEN_AND:               return("TOKEN_AND");
        case TOKEN_OR:                return("TOKEN_OR");

        //DOUBLE DIGIT SIGNS

        case TOKEN_EQUAL_EQUAL:       return("TOKEN_EQUAL_EQUAL");
        case TOKEN_GREATER_EQUAL:     return("TOKEN_GREATER_EQUAL");
        case TOKEN_LESSER_EQUAL:      return("TOKEN_LESSER_EQUAL");
        case TOKEN_AND_AND:           return("TOKEN_AND_AND");
        case TOKEN_OR_OR:             return("TOKEN_OR_OR");
        case TOKEN_BANG_EQUAL:        return("TOKEN_BANG_EQAUL");
        case TOKEN_MINUS_EQUAL:       return("TOKEN_MINUS_EQUAL");
        case TOKEN_PLUS_EQUAL:        return("TOKEN_PLUS_EQUAL");

        //KEYWORDS AND LITRALS

        case TOKEN_IDENTIFIER:        return("TOKEN_IDENTIFIER");
        case TOKEN_STRING:            return("TOKEN_STRING");
        case TOKEN_IF:                return("TOKEN_IF");
        case TOKEN_ELSE:              return("TOKEN_ELSE");
        case TOKEN_WHILE:             return("TOKEN_WHILE");
        case TOKEN_RETURN:            return("TOKEN_RETURN");
        case TOKEN_FOR:               return("TOKEN_FOR");
        case TOKEN_BREAK:             return("TOKEN_BREAK");
        case TOKEN_CONTINUE:          return("TOKEN_CONTINUE");
        case TOKEN_TRUE:              return("TOKEN_TRUE");
        case TOKEN_FALSE:             return("TOKEN_FALSE");
        case TOKEN_INT:               return("TOKEN_INT");
        case TOKEN_FUNC:              return("TOKEN_FUNC");
    
        //OTHERS

        case TOKEN_DATA:              return("TOKEN_DATA");
        case TOKEN_NULL:              return("TOKEN_NULL");
        case TOKEN_EXIT:              return("TOKEN_EXIT");
        case TOKEN_EOL:               return("TOKEN_EOL");

        
    }
    return ("UNKNOW_TOKEN");
}

char* disassembleDataType(dataType type){
    switch(type){
        case DATA_BOOL : return ("DATA_BOOL");
        case DATA_CHAR : return ("DATA_CHAR");
        case DATA_FLOAT : return ("DATA_FLOAT");
        case DATA_INT : return ("DATA_INT");
        case DATA_STRING : return ("DATA_STRING");
        case DATA_VECTOR : return ("DATA_VECTOR");
    }
    return("UNKNOW_DATA");
}
int printInstructer(const char* instruct , int offset){
    printf("%s\n" , instruct);
    return offset+1;
}

void printValue(Value* value){
    switch(value->type){
        case DATA_BOOL:{ 
            bool b = *(bool*)value->value;
            if(b)printf("True");
            else printf("False");
            return;
        }
        case DATA_CHAR:{
            char c = *(char*)value->value;
            printf("%c" , c);
            return;
        }
        case DATA_FLOAT:{
            float f = *(float*)value->value;
            printf("%f" , f);
            return;
        }
        case DATA_INT : {
            int i = *(int*)value->value;
            printf("%d" , i);
            return;
        }
    }
}

int constantInstruction(const char* name , Chunk* chunk , int offset){

    uint8_t constant = chunk->code[offset +1];
    printf("%s %d '" , name , constant);
    printValue(&chunk->constants.values[constant]);
    printf("'\n");
    return offset+2;
}

int declareInstruction(const char* name , Chunk* chunk , int offset){
    uint8_t type = chunk->code[offset+1];
    uint8_t varAddress = chunk->code[offset+2];
    printf("%s Data type - %s Address - %d\n" , name , disassembleDataType(type) , varAddress);
    return offset+3;
}

int VarIoInstruction(const char* name , Chunk* chunk , int offset){
    uint8_t varAddress = chunk->code[offset+1];
    printf("%s Address - %d\n" , name , varAddress);
    return offset+2;
}

int disassembleInstruction(Chunk* chunk , int offset){
    printf("%04d %d-> " , offset , chunk->lines[offset]);

    uint8_t inst = chunk->code[offset];
    switch(inst){
        case OP_RETURN: return printInstructer("OP_RETURN" , offset);
        case OP_LOAD_CONSTANT: return constantInstruction("OP_LOAD_CONSTANT" , chunk , offset);
        case OP_ADD: return printInstructer("OP_ADD" , offset);
        case OP_SUB : return printInstructer("OP_SUB" , offset);
        case OP_MUL : return printInstructer("OP_MUL" , offset);
        case OP_DIV : return printInstructer("OP_DIV" , offset);
        case OP_NEGATE : return printInstructer("OP_NEGATE" , offset);
        case OP_DECLARE : return declareInstruction("OP_DECLARE" , chunk ,offset);
        case OP_STORE : return VarIoInstruction("OP_STORE_VAR" , chunk , offset);
        case OP_LOAD_VAR : return VarIoInstruction("OP_LOAD_VAR" , chunk , offset);
        case OP_EXIT : return printInstructer("OP_EXIT" , offset);
        default : printf("Unknow opCode %d\n",inst);
    }
    return offset+1;
}

void disassembleChunk(Chunk* chunk , const char* name){
    printf("<=== %s ===>\nCount -> %d\nCapacity -> %d\n\n" , name , chunk->count , chunk->capacity);
    for(int offset=0 ; offset< chunk->count;)offset = disassembleInstruction(chunk , offset);
}

void printScan(scanPtr* scan){
    printf("line:%d | size: %d |" , (*scan).line , (*scan).length);
    for(char* it =(*scan).start ;it!=(*scan).end ; it++)printf("%c" , (*it));
    printf("\n");
}

void disassembleScan(scar* fs ){
    int cnt = (*fs).count;
    printf("%d scans found!\n" , cnt);
    for(int i=0 ; i<cnt ; i++){
        printScan(((*fs).pointers+i));
    }
}




void disassembleToken(Token* token){
    printf("line %d | length %d | %s | " , token->line , token->length , disassembleTokenType(token->type));
    for(char* it = token->start ; it!=token->end ; it++)printf("%c" , *it);
    printf("\n");
}

void disassembleTokens(Tokens* tokens){
    Token* it = tokens->token;
    printf("<=== %s ===>\n" , tokens->name);
    printf("Token count : %d \n Token capacity : %d\n Tokens :\n" , tokens->count , tokens->capacity);
    while(it->line>0){
        disassembleToken(it);
        it++;
    }
}

void disassembleFuncToken(tokenFunctions* tf){
    printf("Number of non-global function : %d \n" , tf->count);
    disassembleTokens(&tf->mainFunc);
    for(int i=0 ; i<tf->count ; i++)disassembleTokens(tf->func+i);    
}

void disassembleStackVM(VM* vm){
    printf("Virtual Machine Stack |stack size - %d | VM run count - %d\n" ,vm->stackTop - vm->stack , vm->runCnt);
    Value* it = vm->stack;
    while(it!=vm->stackTop){
        printValue(it);
        printf("\n");
        it++;
    }
}