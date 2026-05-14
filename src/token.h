#pragma once
#include "scanner.h"
#include "value.h"
typedef enum{
    //SINGLE DIGIT SIGNS
    TOKEN_LEFT_PAREN , TOKEN_RIGHT_PAREN , 
    TOKEN_LEFT_BRACE , TOKEN_RIGHT_BRACE ,
    TOKEN_LEFT_BRACKET , TOKEN_RIGHT_BRACKET ,
    TOKEN_COMMA , TOKEN_DOT , TOKEN_STAR ,
    TOKEN_MINUS , TOKEN_PLUS , TOKEN_SLASH ,
    TOKEN_SEMICOLON , TOKEN_EQUAL , TOKEN_GREATER , 
    TOKEN_LESSER , TOKEN_HASH , TOKEN_PERCENT , 
    TOKEN_BANG , TOKEN_AND , TOKEN_OR ,

    //DOUBLE DIGIT SIGNS
    TOKEN_EQUAL_EQUAL , TOKEN_GREATER_EQUAL ,
    TOKEN_LESSER_EQUAL , TOKEN_AND_AND , TOKEN_OR_OR ,
    TOKEN_BANG_EQUAL , TOKEN_MINUS_EQUAL , 
    TOKEN_PLUS_EQUAL ,

    // KEYWORDS AND LITRALS
    TOKEN_IDENTIFIER , TOKEN_STRING  ,
    TOKEN_IF , TOKEN_ELSE , TOKEN_WHILE ,
    TOKEN_RETURN , TOKEN_FOR , TOKEN_BREAK ,
    TOKEN_CONTINUE ,TOKEN_TRUE , TOKEN_FALSE,
    TOKEN_INT , TOKEN_FUNC ,

    // OTHERS
    TOKEN_DATA ,TOKEN_NULL , TOKEN_EXIT , TOKEN_EOL
}tokenType;




typedef struct{
    int line;
    tokenType type;
    char* start;
    char* end;
    int length;
}Token;

typedef struct{
    int count;
    int capacity;
    Token* token;
    dataType returnValue;
    char* name;
}Tokens;



typedef struct{
    int count;
    int capacity;
    Tokens* func;
    Tokens mainFunc;
}tokenFunctions;

void tokenize(scar* scan , Tokens* tokens);
void functinize(Tokens* tokens , tokenFunctions* tf);