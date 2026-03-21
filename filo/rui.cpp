#include "../stora/stora.h"
#include "../debug/debug.h"



void rui_str(std::vector<std::vector<std::string>> &val , int y , int x, std::string &s , std::string stp){
    for(;x<val[y].size() ; x++){
        if((!stp.empty())&&(stp==val[y][x]))return;
        if((val[y][x][0]=='"')&&(val[y][x].back()=='"')){
            std::string cs;
            for(int i=1 ; i<val[y][x].size()-1 ; i++)cs.push_back(val[y][x][i]);
            s+=cs;
        }
        else if(var_exist(val[y][x])){
            if(read_variable_type(val[y][x])!="str")error_at_line("cannot merge "+ read_variable_type(val[y][x])+ " with string" , y);
            s+=std::get<std::string> (variables[val[y][x]]);
        }
        else if(is_tool(val[y][x])){

        }
        else error_at_line("invalid syntax , unexpected character -> '"+ val[y][x] + "'" , y);
        x++;
        if((x>=val[y].size())||(val[y][x]==stp))continue;
        if(val[y][x]!="+")error_at_line("invalid syntax unexpected character-> '"+ val[y][x]+"'" , y);
    }
}