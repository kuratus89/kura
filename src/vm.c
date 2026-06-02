#include "chunk.h"
#include "vm.h"
#include "debug.h"
#include "common.h"
#include "scanner.h"
#include "memory.h"
#include "debug.h"
VM vm;
bool runVM;

void debugVM(){
    disassembleStackVM(&vm);
}

int getInstructionLine(){
    return (vm.currentFunc->ip - vm.currentFunc->chunk->code);
}
void runTimeError(uint8_t *opc , char* msg){
    printf("RUNTIME ERROR\nOP CODE -%d\nLine - %d\n%s", (int)*opc , getInstructionLine(), msg);
}

void resetStack(){
    vm.stackTop = vm.stack;
}

static inline Feeder* pushNewFeed(Chunk* chunk , bool ini, int varsCount){
    if(ini){
        vm.currentFunc = vm.feeds;
        vm.currentFunc->varsCount =0;
    }
    else {
        if((vm.currentFunc - vm.feeds)+ 1 >= FEED_MAX){
            runTimeError(0 , "func stack overflow");
            return NULL;
        }
        vm.currentFunc++;
        vm.currentFunc->varsCount = varsCount;
    }
    vm.currentFunc->chunk = chunk;
    vm.currentFunc->ip = chunk->code;
    // vm.currentFunc->flags = growArray(uint8_t* , NULL , 0 , vm.currentFunc->chunk->flagCount);
    return vm.currentFunc;
}


void iniVM(Chunk* chunk , Chunk* func){
    resetStack();
    initilizeValueArray(&vm.vars);
    vm.vars.capacity =  VAR_STACK_MAX;
    vm.vars.values = growArray(Value , vm.vars.values , 0 , vm.vars.capacity);
    pushNewFeed(chunk , 1 , 0);
    vm.runCnt = 0;  
    vm.varFuncCount = 0; 
    vm.Functions = func;
}

static inline Value* popStackVM(){
    return --vm.stackTop;
}


static inline void pushStackVM(Value* value){
    // *vm.stackTop = value;
    // vm.stackTop++;
    Value val = cloneValue(value);
    *vm.stackTop++ = val;
}

static inline Value* topStackVM(){
    return vm.stackTop-1;
}

static inline Feeder* topFunc(){
    return vm.currentFunc;
}

static inline uint8_t* nextInstruction(){
    vm.runCnt++;
    return topFunc()->ip++;
}

static inline uint8_t* peekInstruction(){
    return topFunc()->ip;
}





interpretResult run(){
    runVM=1;
    while(runVM){

        switch(*nextInstruction()){
            case OP_ADD:{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(topFunc()->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                if((topStackVM())->type != (topStackVM() - 1)->type){
                    runTimeError(topFunc()->ip  , "cannot add values of different data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                switch(topStackVM()->type){
                    case (DATA_INT):{
                        int x = *((int*)popStackVM()->value);
                        *(int*)(topStackVM()->value) = x + *(int*)topStackVM()->value;
                        break;
                    }
                }
                break;
            }

            case OP_SUB : {
                if(vm.stackTop - vm.stack<2){
                    runTimeError(topFunc()->ip, "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(topFunc()->ip, "cannot subtract values of different data types");
                    return ( INTERPRET_RUNTIME_ERROR);
                }
                switch(topStackVM()->type){
                    case (DATA_INT):{
                        int x = *(int*)popStackVM()->value;
                        *(int*)topStackVM()->value= *(int*)topStackVM()->value - x;
                        break;
                    }
                }

                break;

            }
            case OP_MUL : {
                if(vm.stackTop - vm.stack<2){
                    runTimeError(topFunc()->ip, "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type !=(topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot multiply values of diffrent data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(topStackVM()->type){
                    case (DATA_INT):{
                        int x = *(int*)popStackVM()->value;
                        *(int*)topStackVM()->value = *(int*)topStackVM()->value * x;
                        break;
                    }
                }
                break;
            }
            case OP_DIV :{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip ,"cannot divide values of diffrent data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(topStackVM()->type){
                    case (DATA_INT):{
                        int x = *(int*)popStackVM()->value;
                        *(int*)topStackVM()->value = *(int*)topStackVM()->value / x;
                        break;
                    }
                }
                break;
            }
            
            case OP_NEGATE :{
                if(vm.stackTop==vm.stack){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(topStackVM()->type){
                    case (DATA_INT):{
                        *(int*)topStackVM()->value = *(int*)topStackVM()->value * (-1);
                        break;
                    }
                }
                break;
            }
            case OP_GREATER : {
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                if(topStackVM()->type !=(topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType ");
                    return(INTERPRET_RUNTIME_ERROR);
                }

                Value value;
                iniValue(&value , DATA_BOOL);
                bool ans;

                switch(topStackVM()->type){
                    case DATA_INT:{
                        int rhs = *(int*)popStackVM()->value;
                        int lhs = *(int*)popStackVM()->value;
                        ans = rhs<lhs;
                        break;
                    }
                }
                *(bool*)value.value = ans;
                pushStackVM(&value);
                break;
            }
            case OP_LESSER :{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                Value value;
                iniValue(&value , DATA_BOOL);
                switch(topStackVM()->type){
                    case DATA_INT :{
                        int rhs = *(int*)popStackVM()->value;
                        int lhs = *(int*)popStackVM()->value;
                        *(int*)value.value = rhs>lhs;
                        break;
                    }
                }
                pushStackVM(&value);
                break;

            }
            case OP_GREAT_EQUAL:{
                if(vm.stackTop - vm.stack<2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                Value value;
                iniValue(&value ,DATA_BOOL);
                switch(topStackVM()->type){
                    case DATA_INT:{
                        int rhs = *(int*)popStackVM()->value;
                        int lhs = *(int*)popStackVM()->value;
                        *(int*)value.value= lhs <=rhs;
                        break;
                    }
                }
                
                pushStackVM(&value);
                break;
            }
            case OP_LESS_EQUAL:{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                Value value;
                iniValue(&value , DATA_BOOL);
                switch(topStackVM()->type){
                    case DATA_INT :{
                        int rhs = *(int*)popStackVM()->value;
                        int lhs = *(int*)popStackVM()->value;
                        *(bool*)value.value = rhs >= lhs;
                        break;
                    }
                }
                pushStackVM(&value);
                break;
            }

            case OP_EQUAL_EQUAL:{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                Value value;
                iniValue(&value , DATA_BOOL);
                switch(topStackVM()->type){
                    case DATA_INT :{
                        *(bool*)value.value = *(int*)popStackVM()->value ==*(int*)popStackVM()->value;
                        break;
                    }
                }
                pushStackVM(&value);
                break;
            }
            case OP_NOT_EQUAL :{
                if(vm.stackTop - vm.stack <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return ( INTERPRET_RUNTIME_ERROR);
                }
                if(topStackVM()->type != (topStackVM()-1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                Value value;
                iniValue(&value , DATA_BOOL);
                switch(topStackVM()->type){
                    case DATA_INT:{
                        *(bool*)value.value = *(int*)popStackVM()->type != *(int*)popStackVM()->type;
                        break;
                    }
                }
                pushStackVM(&value);
                break;
            }

            case OP_LOAD_CONSTANT :{
                int it = (int)*nextInstruction();
                pushStackVM((topFunc()->chunk->constants.values + it));
                break;
            }

            case OP_DECLARE : {
                dataType type = (dataType)*nextInstruction();
                Value value;
                iniValue(&value , type);
                int it = (int)*nextInstruction();
                writeValueArrayIndex( &vm.vars , &value , it + topFunc()->varsCount);
                break;
            }

            case OP_LOAD_VAR :{
                int it = (int)*nextInstruction();
                pushStackVM((vm.vars.values + it));
                break;
            }
            case OP_LOAD_VAR_LOCAL:{
                int it = (int)*nextInstruction();
                pushStackVM(vm.vars.values + it + topFunc()->varsCount);
                break;
            }

            case OP_STORE :{
                int it = (int)*nextInstruction();
                Value* top = popStackVM();
                writeValueArrayIndex(&vm.vars , top , it);
                break;
            }

            case OP_STORE_LOCAL :{
                int it = (int)* nextInstruction();
                Value* top = popStackVM();
                writeValueArrayIndex(&vm.vars , top , it + topFunc()->varsCount);
                break;
            }

            case OP_GOTO :{
                int it = (int)* nextInstruction();
                int ip = *(topFunc()->chunk->flags.flag + it);
                topFunc()->ip = topFunc()->chunk->code + ip;
                break;
            }

            case OP_GOTO_IF_TRUE:{
                if(vm.stackTop == vm.stack){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                int it = (int)* nextInstruction();
                bool todo;
                switch(topStackVM()->type){
                    case DATA_BOOL :{
                        todo = *(bool*)popStackVM()->value;
                        break;
                    }
                    case DATA_INT:{
                        todo = !(*(int*)popStackVM()->value ==0);
                        break;
                    }
                }
                if(todo){
                    int ip= *(topFunc()->chunk->flags.flag + it);
                    topFunc()->ip = topFunc()->chunk->code +ip;
                }
                break;
            }
            case OP_GOTO_IF_FALSE:{
                if(vm.stackTop == vm.stack){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                int it =(int)*nextInstruction();
                bool todo;
                switch(topStackVM()->type){
                    case DATA_BOOL :{
                        todo = !(*(bool*)popStackVM()->value);
                        break;
                    }
                    case DATA_INT:{
                        todo = (*(int*)popStackVM()->value == 0);
                        break;
                    }
                }
                if(todo){
                    int ip = *(topFunc()->chunk->flags.flag + it);
                    topFunc()->ip = topFunc()->chunk->code +ip;
                }
                break;
            }


            case OP_UNLOAD : {
                int it = (int)* nextInstruction();
                break;
            }

            case OP_RETURN :{
                vm.currentFunc--;
                break;
            }

            case OP_EXIT:{
                runVM =0;
                break;
            }
            case OP_CALL : {
                int it = (int)*nextInstruction();
                pushNewFeed(vm.Functions+it , 0 , topFunc()->chunk->varCount + topFunc()->varsCount);
                break;
            }
            case OP_PRINT:{
                printValue(popStackVM());
                printf("\n");
                break;
            }

        }
        #ifdef DEBUG
        // debugVM();
        #endif
    }

    return (INTERPRET_OK);
}


