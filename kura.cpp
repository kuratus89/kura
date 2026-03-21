#include "filo/filo.h"
#include "stora/stora.h"
#include "oupt/oupt.h"
#include <iostream>

int main(int argc , char* argv[]){
    debug =1;
    load_code(argc , argv);

    parser(code_str , code);
    for(auto val:code){
        for(auto var:val)std::cout<<var<<" : ";
        std::cout<<"\n";
    }
    // interp(code);

    // for(auto val:variables){
    //     std::cout<<val.first<<" "<<std::get<int>(val.second)<<"\n";
    // }

    
}