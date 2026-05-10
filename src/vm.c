#include "chunk.h"
#include "vm.h"
#include <stdio.h>
#include "debug.h"
#include "common.h"
#include "scanner.h"
VM vm;

void resetStack(){
    vm.stackTop = vm.stack;

}
void initilizeVM(){
    resetStack();
}

void freeVM(){
    resetStack();
}

Value popStackVM(){
    vm.stackTop--;
    return *vm.stackTop;
}

void pushStackVM(Value value){
    *vm.stackTop = value;
    vm.stackTop++;
}

Value* topStackVM(){
    return vm.stackTop-1;
} 


static interpretResult run(){
    while(true){
        #ifdef DEBUG_STACK_VM
            printf("trace stackVm :\n");
            for(Value* it = vm.stack ; it!=vm.stackTop ; it++){
                printf("[");
                printValue(*it);
                printf("]\n");
            }
            printf("trace stackVM ends\n");
        #endif
        #ifdef DEBUG_CHUNK
            printf("trace disassemble chunk :");
            disassembleInstruction(vm.chunk ,(int) (vm.ip - vm.chunk->code));
            
        #endif
        uint8_t instruction ;
        switch(instruction = *vm.ip++){
            case OP_RETURN:{return INTERPRET_OK;}
            case OP_CONSTANT:{
                Value constant = vm.chunk->constants.values[*vm.ip++];
                pushStackVM(constant);
                break;
            }
            case OP_NEGATE : {
                pushStackVM(- popStackVM());
                break;
            }
            case OP_ADD : {
                double a = popStackVM();
                *topStackVM()+=a;
                break;
            }
            case OP_SUB : {
                double a = popStackVM();
                *topStackVM()-=a;
                break;
            }
            case OP_MUL : {
                double a = popStackVM();
                *topStackVM()*=a;
                break;
            }
            case OP_DIV : {
                double a = popStackVM();
                *topStackVM()/=a;
            }
        }
    }
}


interpretResult interpret(char* source){
    scanToPtr(source);
}