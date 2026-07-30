#include "chunk.h"
#include "vm.h"
#include "debug.h"
#include "common.h"
#include "scanner.h"
#include "memory.h"
#include "debug.h"





// note-> 1) fix pushStackVM to new memory system , 2) update new function like getRuntimeStack() , popRuntimeStack()

VM vm;
bool runVM;

void debugVM(){
    disassembleRuntimeStack();
}

int getInstructionLine(){
    return (vm.currentFunc->ip - vm.currentFunc->chunk->code);
}
void runTimeError(uint8_t *opc , char* msg){
    printf("RUNTIME ERROR\nOP CODE -%d\nLine - %d\n%s", (int)*opc , getInstructionLine(), msg);
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
    // initilizeValueArray(&vm.vars);
    iniMemory();
    // vm.vars.capacity =  VAR_STACK_MAX;
    // vm.vars.values = growArray(Value , vm.vars.values , 0 , vm.vars.capacity);
    pushNewFeed(chunk , 1 , 0);
    vm.runCnt = 0;  
    vm.varFuncCount = 0; 
    vm.Functions = func;
}




// static inline void pushStackVM(Value* value){
//     // *vm.stackTop = value;
//     // vm.stackTop++;
//     Value val = cloneValue(value);
//     *vm.stackTop++ = val;
// }

// static inline Value* topStackVM(){
//     return vm.stackTop-1;
// }

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
                if(getRuntimeStackSize()<2){
                    runTimeError(topFunc()->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                if((getRuntimeStack(0))->type != getRuntimeStack(1)->type){
                    runTimeError(topFunc()->ip  , "cannot add values of different data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                switch(getRuntimeStack(0)->type){
                    case (DATA_INT):{
                        int x = *((int*)popRuntimeStack()->value);
                        *(int*)(getRuntimeStack(0)->value) = x + *(int*)getRuntimeStack(0)->value;
                        break;
                    }
                }
                break;
            }

            case OP_SUB : {
                if(getRuntimeStackSize()<2){
                    runTimeError(topFunc()->ip, "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(topFunc()->ip, "cannot subtract values of different data types");
                    return ( INTERPRET_RUNTIME_ERROR);
                }
                switch(getRuntimeStack(0)->type){
                    case (DATA_INT):{
                        int x = *(int*)popRuntimeStack()->value;
                        *(int*)getRuntimeStack(0)->value= *(int*)getRuntimeStack(0)->value - x;
                        break;
                    }
                }

                break;

            }
            case OP_MUL : {
                if(getRuntimeStackSize()<2){
                    runTimeError(topFunc()->ip, "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type !=getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot multiply values of diffrent data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(getRuntimeStack(0)->type){
                    case (DATA_INT):{
                        int x = *(int*)popRuntimeStack()->value;
                        *(int*)getRuntimeStack(0)->value = *(int*)getRuntimeStack(0)->value * x;
                        break;
                    }
                }
                break;
            }
            case OP_DIV :{
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip ,"cannot divide values of diffrent data types");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(getRuntimeStack(0)->type){
                    case (DATA_INT):{
                        int x = *(int*)popRuntimeStack()->value;
                        *(int*)getRuntimeStack(0)->value = *(int*)getRuntimeStack(0)->value / x;
                        break;
                    }
                }
                break;
            }
            case OP_MOD:{
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type !=getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot mod values of diffrent data types");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                switch(getRuntimeStack(0)->type){
                    case(DATA_INT):{
                        int x = *(int*)popRuntimeStack()->value;
                        // int y = *(int*)popStackVM()->value;
                        // iniStackValue(&value , DATA_INT);
                        *(int*)getRuntimeStack(0)->value = *(int*)getRuntimeStack(0)->value%x;
                        // *(int*)value.value = y%x;
                        break;
                    }
                }
                break;
            }
            
            case OP_NEGATE :{
                if(getRuntimeStackSize()<1){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                switch(getRuntimeStack(0)->type){
                    case (DATA_INT):{
                        *(int*)getRuntimeStack(0)->value = *(int*)getRuntimeStack(0)->value * (-1);
                        break;
                    }
                }
                break;
            }

            case OP_GREATER : {
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }

                if(getRuntimeStack(0)->type !=getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType ");
                    return(INTERPRET_RUNTIME_ERROR);
                }

                // Value* value = pushRuntimeStackValue(DATA_BOOL );
                // iniValue(&value , DATA_BOOL);
                // iniStackValue(&value , DATA_BOOL);
                bool ans;

                switch(getRuntimeStack(0)->type){
                    case DATA_INT:{
                        int rhs = *(int*)popRuntimeStack()->value;
                        int lhs = *(int*)popRuntimeStack()->value;
                        ans = rhs<lhs;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                *(bool*)value->value = ans;
                // pushStackVM(&value);
                break;
            }
            case OP_LESSER :{
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                // Value* value = pushRuntimeStackValue(DATA_BOOL);
                bool ans;
                switch(getRuntimeStack(0)->type){
                    case DATA_INT :{
                        int rhs = *(int*)popRuntimeStack()->value;
                        int lhs = *(int*)popRuntimeStack()->value;
                        // *(bool*)value->value = rhs>lhs;
                        ans = rhs>lhs;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                *(bool*)value->value = ans;

                break;

            }
            case OP_GREAT_EQUAL:{
                if(getRuntimeStackSize()<2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                // Value* value = pushRuntimeStackValue(DATA_BOOL);
                bool ans;
                switch(getRuntimeStack(0)->type){
                    case DATA_INT:{
                        int rhs = *(int*)popRuntimeStack()->value;
                        int lhs = *(int*)popRuntimeStack()->value;
                        // *(bool*)value->value= lhs <=rhs;
                        ans = lhs<=rhs;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                *(bool*)value->value = ans;                
                break;
            }
            case OP_LESS_EQUAL:{
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                // Value* value = pushRuntimeStackValue(DATA_BOOL);
                bool ans;
                switch(getRuntimeStack(0)->type){
                    case DATA_INT :{
                        int rhs = *(int*)popRuntimeStack()->value;
                        int lhs = *(int*)popRuntimeStack()->value;
                        // *(bool*)value->value = rhs >= lhs;
                        ans = rhs>=lhs;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                
                *(bool*)value->value = ans;
                break;
            }

            case OP_EQUAL_EQUAL:{
                if(getRuntimeStackSize()<2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                // Value* value = pushRuntimeStackValue(DATA_BOOL);
                bool ans;
                switch(getRuntimeStack(0)->type){
                    case DATA_INT :{
                        // *(bool*)value->value = *(int*)popRuntimeStack()->value ==*(int*)popRuntimeStack()->value;
                        ans = *(int*)popRuntimeStack()->value == *(int*)popRuntimeStack()->value;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                *(bool*)value->value = ans;
                break;
            }
            case OP_NOT_EQUAL :{
                if(getRuntimeStackSize() <2){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return ( INTERPRET_RUNTIME_ERROR);
                }
                if(getRuntimeStack(0)->type != getRuntimeStack(1)->type){
                    runTimeError(vm.currentFunc->ip , "cannot compare values of diffrent dataType");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                // Value* value = pushRuntimeStackValue(DATA_BOOL); 
                bool ans;
                switch(getRuntimeStack(0)->type){
                    case DATA_INT:{
                        // *(bool*)value->value = *(int*)popRuntimeStack()->value != *(int*)popRuntimeStack()->value;
                        ans = *(int*)popRuntimeStack()->value != *(int*)popRuntimeStack()->value;
                        break;
                    }
                }
                Value* value = pushRuntimeStackValue(DATA_BOOL);
                *(bool*)value->value = ans;
                break;
            }

            case OP_LOAD_CONSTANT :{
                int it = (int)*nextInstruction();
                Value* constant = topFunc()->chunk->constants.values+it;
                pushRuntimeStackCloneValue(constant);
                break;
            }

            case OP_DECLARE : {
                dataType type = (dataType)*nextInstruction();
                int it = (int)*nextInstruction();
                // iniStackValue(type , it);
                iniStackMemoryValue(it+ topFunc()->varsCount , type);
                break;
            }

            case OP_LOAD_VAR :{
                int it = (int)*nextInstruction();
                // pushStackVM(getStackMemoryValueIndex(it));
                pushRuntimeStackCloneValue(getStackMemoryValueIndex(it));
                break;
            }
            case OP_LOAD_VAR_LOCAL:{
                int it = (int)*nextInstruction();
                // pushStackVM(vm.vars.values + it + topFunc()->varsCount);
                // pushStackVM(GetStackMemoryValueIndex(it+ topFunc()->varsCount));
                pushRuntimeStackCloneValue(getStackMemoryValueIndex(it+ topFunc()->varsCount));
                break;
            }

            case OP_STORE :{
                int it = (int)*nextInstruction();
                Value* top = popRuntimeStack();
                // writeValueArrayIndex(&vm.vars , top , it);
                writeStackMemoryValueIndex(top , it);
                break;
            }

            case OP_STORE_LOCAL :{
                int it = (int)* nextInstruction();
                Value* top = popRuntimeStack();
                // writeValueArrayIndex(&vm.vars , top , it + topFunc()->varsCount);
                writeStackMemoryValueIndex(top , it+ topFunc()->varsCount);
                break;
            }

            case OP_GOTO :{
                int it = (int)* nextInstruction();
                int ip = *(topFunc()->chunk->flags.flag + it);
                topFunc()->ip = topFunc()->chunk->code + ip;
                break;
            }

            case OP_GOTO_IF_TRUE:{
                if(getRuntimeStackSize()<1){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                int it = (int)* nextInstruction();
                bool todo;
                switch(getRuntimeStack(0)->type){
                    case DATA_BOOL :{
                        todo = *(bool*)popRuntimeStack()->value;
                        break;
                    }
                    case DATA_INT:{
                        todo = !(*(int*)popRuntimeStack() ==0);
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
                if(getRuntimeStackSize()<1){
                    runTimeError(vm.currentFunc->ip , "stack underflow");
                    return(INTERPRET_RUNTIME_ERROR);
                }
                int it =(int)*nextInstruction();
                bool todo;
                switch(getRuntimeStack(0)->type){
                    case DATA_BOOL :{
                        todo = !(*(bool*)popRuntimeStack()->value);
                        break;
                    }
                    case DATA_INT:{
                        todo = (*(int*)popRuntimeStack()->value == 0);
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
                printValue(popRuntimeStack());
                printf("\n");
                break;
            }

        }
        #ifdef DEBUG
        debugVM();
        #endif
    }

    return (INTERPRET_OK);
}


