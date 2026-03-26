#include "../stora/stora.h"
#include "rui.h"
#include "../debug/debug.h"
#include "filo.h"

void cvn(std::vector<std::pair<std::string , std::string >> &carv , std::vector<std::string> &code , int &x){
    if(code[x]!="(")error_exit("invalid syntax");
    x++;
    while((x<code.size())&&(code[x]!=")")){
        std::pair<std::string , std::string> temp; 
        if(!is_data_structure(code[x]))error_exit("invalid data type");
        temp.first = code[x];
        x++;
        if((x>=code.size())||(code[x]==")"))error_exit("name for data not provided");
        if(!is_str(code[x]))error_exit("only a-z characters are allowed as varible name");
        temp.second = code[x];
        carv.push_back(temp);
        x++;
        if(code[x]==",")x++;
    }
    if((x>=code.size())||(code[x]!=")"))error_exit("invalid syntax");
}

void process_func(code_struct &code){
    if(functions.count(code.name))error_exit("function "+ code.name + "already exists");
    funct temp;
    int x=0;
    if(!is_str)error_exit("only a-z characters are allowed as functions name");
    temp.name = code.ln[x];
    x++;
    cvn(temp.carry_var_name, code.ln , x);
    x++;
    if(code.ln[x]!="{")error_exit("invalid syntax");
    x++;
    int y = code.ln.size()-2;
    if((code.ln[y]!="}")||(code.ln[y+1]!=";"))error_exit("invalid syntax");
    voimer(code.ln , temp.code , x ,y);  
    functions[code.name] = temp;  
}