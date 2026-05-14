#include "common.h"
#include "token.h"
#include "memory.h"
#include "scanner.h"
#include "debug.h"

char* tokenGetSource(Token* token){
    char* val = NULL;
    int cnt = token->length +1;
    val = growArray(char , val , 0 , cnt);
    cnt=0;
    for(char* it = token->start ; it!=token->end; it++ , cnt++)val[cnt] = *it;
    val[cnt] = '\0';
    return val;
}

void tokenError(char* msg , Token* token){
    printf("fail to tokenize at line : %d \ntoken type : %s \ntoken : %s \nerror : %s " ,token->line , disassembleTokenType(token->type) , tokenGetSource(token) ,  msg);
    exit(56);
}

void iniTokens(Tokens* tokens , int count){
    tokens->count = 0;
    tokens->capacity = count;
    tokens->name = "Unknown Function";
    tokens->token = growArray(Token , NULL , 0 , count);    
}

void writeToken(Tokens* tokens , Token* token){
    if(tokens->capacity == tokens->count){
        int oldCap = tokens->capacity;
        tokens->capacity = growCapacity(oldCap);
        tokens->token = growArray(Token , tokens->token , oldCap , tokens->capacity);
    }
    tokens->token[tokens->count] = *token;
    tokens->count++;
}

void freeWasteToken(Tokens* tokens){
    if(tokens->count==tokens->capacity)return;
    growArray(Token , tokens->token , tokens->capacity , tokens->count);
    tokens->capacity = tokens->count;
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

bool tokenEqualString(Token* token , char* s){
    char* a = token->start;
    char* b = s;
    while((a!=token->end)&&(*b!='\0')){
        if(*a!=*b)return 0;
        a++;
        b++;
    }
    if((a!=token->end)||(*b!='\0'))return 0;
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
    int* it = find(signs ,value , 20 , 1);
    if(it==NULL)return 0;
    int index = it - signs;
    token->type = singleDigitTokens[index];
    return 1;
}

bool tokenizeDoubleSign(Token* token){
    if(token->length!=2)return 0;
    char value = *token->start;
    if(((value>='a')&&(value<='z'))||((value>='A')&&(value<='Z'))||((value>='0')&&(value<='9')))return 0;
    
    int firstDigit[] = {'!' , '&' , '+' , '-' , '<' , '=' , '>' , '|' };
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
        "func",
        "if",
        "return",
        "true",
        "while"
    };
    
    tokenType keyType[] = {
        TOKEN_NULL , TOKEN_BREAK , TOKEN_CONTINUE ,TOKEN_ELSE , TOKEN_EXIT , TOKEN_FALSE 
        , TOKEN_FOR ,TOKEN_FUNC ,TOKEN_IF , TOKEN_RETURN , TOKEN_TRUE , TOKEN_WHILE
    };
    
    int low = 0;
    int high = 11;
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

void iniTokenFunc(tokenFunctions* tf , int size){
    tf->count =0;
    tf->capacity = size;
    tf->func = growArray(Tokens , NULL ,0 , size);
    iniTokens(&tf->mainFunc , 0);
}

void freeTokenFunc(tokenFunctions* tf){
    freeArray(Tokens , tf , tf->capacity);
    tf->count=0;
    tf->capacity=0;
    iniTokens(&tf->mainFunc , 0);
}

void writeTokenFunc(tokenFunctions* tf , Tokens* tokens){
    if(tf->count == tf->capacity){
        int oldCap = tf->capacity;
        tf->capacity = growCapacity(oldCap);
        tf->func = growArray(Tokens , tf->func , oldCap , tf->capacity);
    }
    tf->func[tf->count] = *tokens;
    tf->count++;
}



void functinize(Tokens* tokens, tokenFunctions* tf){
    
    Token* it = tokens->token;
    iniTokenFunc(tf , 0);
    tf->mainFunc.name = "Global Function";
    Token eol;
    eol.start = NULL;
    eol.end = NULL;
    eol.length = -1;
    eol.line = -1;
    eol.type = TOKEN_EOL;
    while(it->length>0){
        if(it->type==TOKEN_FUNC){
            Tokens tokens;
            iniTokens(&tokens , 0);
            Token* start = it;
            // while(it->type!=TOKEN_LEFT_BRACE){
            //     if(it->length<0)tokenError("Invalid syntax for function" , start->line);
            //     writeToken(&tokens , it);
            //     it++;
            // }
            // writeToken(&tokens , it);
            writeToken(&tokens , it);
            it++;
            if(it->type!=TOKEN_DATA)tokenError("Invalid Type : expected a valid data type" , it);
            writeToken(&tokens , it);
            it++;
            if(it->type!=TOKEN_IDENTIFIER)tokenError("Invalid Type : expected a identifier" , it);
            writeToken(&tokens , it);
            tokens.name = tokenGetSource(it);
            it++;
            if(it->type!=TOKEN_LEFT_PAREN)tokenError("Invalid Type : expected left paren" , it);
            writeToken(&tokens , it);
            it++;
            int bal=1;
            while(bal){
                if(it->length<0)tokenError("Invalid syntax for function :expected right paren" , it);
                writeToken(&tokens , it);
                if(it->type==TOKEN_LEFT_PAREN)bal++;
                else if(it->type==TOKEN_RIGHT_PAREN)bal--;
                it++;
            }
            bal=1;
            if(it->type!=TOKEN_LEFT_BRACE)tokenError("Invalid syntax for function : expected left brace" , it);
            it++;
            while(bal){
                if(it->length<0)tokenError("Invalid syntax for function : expected right brace" , it);
                writeToken(&tokens , it);
                if(it->type==TOKEN_LEFT_BRACE)bal++;
                else if(it->type==TOKEN_RIGHT_BRACE)bal--;
                it++;
            }
            writeToken(&tokens , &eol);
            freeWasteToken(&tokens);
            writeTokenFunc(tf , &tokens);
            continue;
        }
        else {
            writeToken(&tf->mainFunc , it);
            it++;
        }
    }
    writeToken(&tf->mainFunc , &eol);
    freeWasteToken(&tf->mainFunc);
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

        writeToken(tokens , &token);
    }
    token.start = NULL;
    token.end = NULL;
    token.length = -1;
    token.line = -1;
    token.type = TOKEN_EOL;
    writeToken(tokens , &token);
    freeWasteToken(tokens);
}