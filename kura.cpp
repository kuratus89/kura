#include "filo/filo.h"
#include "stora/stora.h"
#include "debug/debug.h"
#include <iostream>

int main(int argc , char* argv[]){
    debug =1;
    load_code(argc , argv);
    kuser(code_str , code_who);
    voimer(code_who , code , 0 , INT_MAX);
    // for(auto val:code){
    //     for(auto var:val.ln)std::cout<<var<<" : ";
    //     std::cout<<"\n";
    // }

    gloaby(code);  

    print_global_var();
    
    std::cout<<"\n\n";

    std::cout<<functions.size();

    std::cout<<"\n\n";

    for(auto val:functions){
        print_func_details(val.second);
        std::cout<<"\n\n";
    }



    
}