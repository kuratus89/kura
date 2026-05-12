#include "common.h"
#include "token.h"
#include "memory.h"
#include "scanner.h"

void iniTokens(Tokens* tokens , int count){
    tokens->count = 0;

    tokens->token = growArray(Token , NULL , 0 , count);    
}
void pushToken(Tokens* tokens , Token token){
    *((tokens->token)+tokens->count)= token;
    tokens->count++;
}


bool charGreater(char* starta , char* startb , char* endb){
    char* a= starta;
    char* b = startb;
    while((*a!='\0')&&(b!=endb)){
        if(*a>*b)return 1;
        else if(*a<*b)return 0;
        a++;
        b++;
    }
    if((b==endb)&&(*a!='\0'))return 1;
    return 0;
}
bool tokenEqual(char* starta  , char* startb , char* endb){
    char* a = starta;
    char* b = startb;
    while((*a!='\0')&&(b!=endb)){
        if(*a!=*b)return 0;
        a++;
        b++;
    }
    if((*a!='\0')||(b!=endb))return 0;
    return 1;
}

bool tokenizeSingleDigitSign(Token* token){
    if(token->length!=1)return 0;   
    char value = *token->start;
    if(((value>='a')&&(value<='z'))||((value>='A')&&(value<='Z'))||((value>='0')&&(value<='9')))return 0; 
    int signs[] = {'!' , '#' , '%' , '&' , '(' , ')' , '*' , '+' , ',' , '-' , '/' , ';' , '<' , '=' , '>' , '[' , ']' , '{' , '|' , '}'};
    tokenType singleDigitTokens[] = {TOKEN_BANG , TOKEN_HASH , TOKEN_PERCENT , TOKEN_AND , TOKEN_LEFT_PAREN , TOKEN_RIGHT_PAREN , 
    TOKEN_STAR , TOKEN_PLUS , TOKEN_COMMA , TOKEN_MINUS , TOKEN_SLASH , TOKEN_SEMICOLON , TOKEN_LESSER , TOKEN_EQUAL , TOKEN_GREATER ,
    TOKEN_LEFT_BRACKET , TOKEN_RIGHT_BRACKET , TOKEN_LEFT_BRACE , TOKEN_OR  , TOKEN_RIGHT_BRACE
    };
    int* it = find(signs ,value , 19 , 1);
    if(it==NULL)return 0;
    int index = it - signs;
    token->type = singleDigitTokens[index];
    return 1;
}

bool tokenizeDoubleSign(Token* token){
    if(token->length!=2)return 0;
    char value = *token->start;
    if(((value>='a')&&(value<='z'))||((value>='A')&&(value<='Z'))||((value>='0')&&(value<='9')))return 0;
    
    int firstDigit[] = {'!' , '&' , '+' , '-' , '<' , '=' , '>' , '|'};
    int secondDigit[] = {'=' , '&' , '=' , '=' , '=' , '=' , '=' , '|'} ;
    int* it = find(firstDigit ,  value , 8 , 1);
    if(it==NULL)return 0;
    int index = it - firstDigit;
    if(*(token->start+1)!=secondDigit[index])return 0;

    tokenType type[] = {
    TOKEN_BANG_EQUAL , TOKEN_AND_AND , TOKEN_PLUS_EQUAL , 
    TOKEN_MINUS_EQUAL , TOKEN_LESSER_EQUAL ,  TOKEN_EQUAL_EQUAL ,
    TOKEN_GREATER_EQUAL , TOKEN_OR_OR
    };
    token->type = type[index];
    return 1;    
}

bool tokenizeString(Token* token){
    if((*(token->start)!='"')||(*(token->end-1)!='"'))return 0;
    token->end--;
    token->start++;
    token->type = TOKEN_STRING ;
    return 1;
}

bool tokenizeInt(Token* token){
    for(char* it = token->start ; it!=token->end ; it++)if(((*it)>'9')||((*it)<'0'))return 0;
    token->type = TOKEN_INT;
    return 1;
    
}

bool tokenizeKeywords(Token* token){
    // i have 2 ideas in mind , first is to use trees with binary search and second is array with binary search
    // but since i have less time , i will go with array with bin search , later i will update it to trees one.
    char* keywords[] = {
        "NULL",
        "break",
        "continue",
        "else",
        "exit",
        "false",
        "for",
        "if",
        "return",
        "true",
        "while"
    };
    
    tokenType keyType[] = {
        TOKEN_NULL , TOKEN_BREAK , TOKEN_CONTINUE ,TOKEN_ELSE , TOKEN_EXIT , TOKEN_FALSE 
        , TOKEN_FOR ,TOKEN_IF , TOKEN_RETURN , TOKEN_TRUE , TOKEN_WHILE
    };
    
    int low = 0;
    int high = 10;
    int mid;
    while(low<=high){
        mid = (low+high)/2;
        char* it = keywords[mid];
        if(tokenEqual(it , token->start , token->end )){
            token->type = keyType[mid];
            return 1;
        }
        if(charGreater(it , token->start , token->end ))high = mid -1;
        else low = mid+1;        
    }
    return 0;
}

bool tokenizeDataStructure(Token* token){
    char* data[] = {"bool", "float" , "int" , "string" };
    for(int i=0 ; i<4 ; i++){
        if(tokenEqual(data[i] , token->start , token->end)){
            token->type = TOKEN_DATA;
            return 1;
        }
    }
    return 0;
}




void tokenize(scar* scan , Tokens* tokens){
    int cnt = scan->count;
    iniTokens(tokens , cnt+1);
    scanPtr* sp= scan->pointers;
    Token token;
    while(sp->line>0){
        token.start = sp->start;
        token.end = sp->end;
        token.line = sp->line;
        token.length = sp->length;
        sp++;
        if(tokenizeSingleDigitSign(&token));
        else if(tokenizeDoubleSign(&token));
        else if(tokenizeString(&token));
        else if(tokenizeInt(&token));
        else if(tokenizeDataStructure(&token));
        else if(tokenizeKeywords(&token));
        else token.type = TOKEN_IDENTIFIER;

        pushToken(tokens , token);
    }
    token.start = sp->start;
    token.end = sp->start;
    token.line = sp->line;
    token.length = sp->length;
    token.type = TOKEN_EOL;
    pushToken(tokens , token);

}