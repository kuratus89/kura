#include "common.h"
#include "scanner.h"
#include "memory.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

void initilizeScan(scar* vak){
    vak->capacity=0;
    vak->count =0;
    vak->pointers = NULL;
}
void freeScan(scar* vak){
    freeArray(scanPtr , vak->pointers , vak->capacity);
    initilizeScan(vak);
}
void pushScan(scanPtr* vok, scar* vak){
    if(vak->capacity - vak->count ==0){
        int oldCap = vak->capacity;
        vak->capacity = growCapacity(oldCap);
        vak->pointers = growArray(scanPtr , vak->pointers , oldCap , vak->capacity);
    }
    vak->pointers[vak->count] = *vok;
    vak->count++;
}


bool spaceScan(scanPtr* scan , scar* fs){
    if((*scan->end!=' ')&&(*scan->end!='\n')&&(*scan->end!='\r')&&(*scan->end!='\t')&&(*scan->end!='\0'))return 0;
    if(scan->end!=scan->start){
        scan->length = scan->end - scan-> start;
        pushScan(scan , fs);
    }
    if(*scan->end=='\0')return 1;
    else if(*scan->end=='\n')scan->line++;

    scan->end++;
    scan->start= scan->end;
    return 1;
}

bool stringScan(scanPtr* scan, scar* fs){
    // in future i will upgrade it to support ' also.
    if(*scan->end!='"')return 0;
    
    if(scan->end!=scan->start){
        scan->length = scan->end - scan->start;
        pushScan(scan, fs);
        scan->start=scan->end;
    }
    scan->end++;
    scan->length = scan->end - scan->start;
    pushScan(scan , fs);
    scan->start = scan->end;
    while((*scan->end!='\0')&&(*scan->end!='"')){
        if(*scan->end=='\n')scan->line++;
        scan->end++;
    }
    if(*scan->end=='\0'){
        printf("closing string not found");
        exit(10);
    }
    scan->length = scan->end - scan->start;
    pushScan(scan, fs);
    scan->start =scan->end;
    scan->end++;
    scan->length = scan->end - scan->start;
    pushScan(scan , fs);
    // scan->end++;
    scan->start = scan->end;
    return 1;
}

bool intScan(scanPtr* scan, scar* fs){
    if(((*scan->end<'0')||(*scan->end>'9'))&&(*scan->end!='.'))return 0;
    scan->end++;
    return 1;
}

bool charScan(scanPtr* scan , scar* fs){
    if(((*scan->end<'a')||(*scan->end>'z'))&&((*scan->end<'A')||(*scan->end>'Z')))return 0;
    scan->end++;
    return 1;
}

void doubleSign(scanPtr* scan){
    int doubleSign[14] = {'!' , '=' , '&' , '&' , '<' , '=' , '>' , '=' , '=' , '=' , '+' , '=' , '-' , '=' };
    for(int i=0 ; i<14 ; i+=2){
        if((*scan->end==doubleSign[i])&&(*(scan->end+1)==doubleSign[i+1])){
            scan->end+=2;
            return;
        }
    }
    scan->end++;
}
bool signs(scanPtr* scan , scar* fs){
    int sign[19] = {'!' , '#', '%' , '&' , '(' , ')' , '*' , '+' , ',' , '-' , '/' , ';' , '<' , '=' , '>' , '[' , ']' , '{' ,'}'};
    if(find(sign  , (int) *scan->end , 19 , 1)==NULL)return 0;

    if(scan->end!=scan->start){
        scan->length =  scan->end - scan->start;
        pushScan(scan , fs);
        scan->start= scan->end;
    }

    doubleSign(scan);
    if(*scan->end=='#'){
        while((*scan->end!='\n')&&(*scan->end!='\0'))scan->end++;
        scan->end++;
        scan->start=scan->end;
        return 1;
    }
    if((*scan->end=='/')&&(*(scan->end+1)=='/')){
        scan->end++;
        while(!((*(scan->end+1)=='\0')||((*scan->end=='/')&&(*(scan->end+1)=='/')))){
            scan->end++;
            if(*scan->end=='\n')scan->line++;
        }
        scan->end++;
        if(*scan->end=='\0'){
            printf("closing comment not found");
            exit(10);
        }
        scan->end++;
        scan->start = scan->end;
        return 1;
    }
    if(scan->start!=scan->end){
        scan->length = scan->end - scan->start;
        pushScan(scan , fs);
        scan->start = scan->end;
    }
    return 1;
}


void scanToPtr(char* source){
    scar fs;
    initilizeScan(&fs);
    scanPtr scan;
    scan.start = source;
    scan.end = source;
    scan.line = 1;
    while(*scan.end!='\0'){
        if(spaceScan(&scan, &fs))continue;
        if(stringScan(&scan, &fs))continue;
        if(intScan(&scan , &fs))continue;
        if(charScan(&scan , &fs))continue;
        if(signs(&scan , &fs))continue;
        printf("Invalid character :'%c'\n" , *scan.end);
        exit(10);
    }
    if(scan.start!=scan.end){
        scan.line = scan.end - scan.start;
        pushScan(&scan , &fs);
    }
    disassembleScan(&fs);
    


}