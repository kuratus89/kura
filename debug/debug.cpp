
#include <iostream>
#include "../stora/stora.h"
#include "../filo/rui.h"



void print_debug(std::string cou){
    
    if(!debug)return;

    std::cout<<cou;
}

void error_at_line(std::string s , int y){
    if(s.empty())s = "invalid syntax\n";
    print_debug("program crash due to->"+s+ "\n");
    print_debug("program crash at line "+ std::to_string(y)+ "\n") ;
    print_debug("->"+ code_str[y]+ "\n");
    exit(0);
}

void error_exit(std::string s){
    if(s.empty())s= "invalid syntax\n";
    print_debug("program crash due to -> " + s + "\n");
    exit(1);
}

void print_global_var(){
    for(auto val:global_variable){
        std::cout<<val.first<<":";
        if(std::holds_alternative<int>(val.second.value))std::cout<<std::get<int>(val.second.value);
        else if(std::holds_alternative<std::string>(val.second.value))std::cout<<std::get<std::string>(val.second.value);
        std::cout<<"\n";
    }
}

void code_line_printer(std::vector<std::string> &ln){
    for(auto val:ln)std::cout<<val<<":";
    std::cout<<"\n";
}

void print_func_details(funct &fa){
    std::cout<<"functions name ->";
    std::cout<<fa.name<<"\n";
    std::cout<<fa.carry_var_name.size()<<" carry variables\n";
    for(auto val:fa.carry_var_name)std::cout<<val.first<<" : "<<val.second<<"\n";
    std::cout<<"code :\n";
    for(auto val:fa.code){
        code_line_printer(val.ln);
    }
    
}