
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

void printer(std::vector<std::vector<std::string>> &val , int y , int x){
    if(val[y][x]!="<<")error_at_line("" , y);
    std::string s;
    rui_str(val , y , x +1, s , ">>");
    print_debug(s);
}

std::map<std::string , void(*)(std::vector<std::vector<std::string>>&, int, int)> degu = {
    {"print" , printer}
};

void debuger(std::vector<std::vector<std::string>> &val , int y){
    if(degu.count(val[y][0]))degu[val[y][0]](val , y , 1);
}