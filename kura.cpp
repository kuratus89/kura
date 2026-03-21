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
    std::cout<<"\n\n";

    interp(code);

    for(auto val:variables){
        std::cout<<val.first<<" ";
        std::string type = read_variable_type(val.first);
        if(type=="int")std::cout<<std::get<int>(val.second);
        else if(type=="str")std::cout<<std::get<std::string>(val.second);
        std::cout<<"\n";
    }

    
}