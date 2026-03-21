#include <iostream>
#include <fstream>
#include <cstdlib>
#include "../stora/stora.h"
#include "../oupt/oupt.h"

void error_filo(std::string s){
    print_debug("program crash in filo");
    print_debug(s);
    exit(0);
}

void error_at_line(std::string s , int y){
    print_debug("program crash due to->"+s);
    print_debug("program crash at line "+ std::to_string(y));
    print_debug("->"+ code_str[y]);
}

void load_code(int argc , char* argv[]){

    if(argc<2)error_filo("no code found");

    std::ifstream file(argv[1]);

    if(!file)error_filo("fail to load code");

    std::string lin;

    while(std::getline(file , lin)){
        // if(lin.empty())continue;
        code_str.push_back(lin);
    }
    
}

void parser(std::vector<std::string> &cst , std::vector<std::vector<std::string>> &cvs){
    bool qot=0;
    for(auto &val:cst){
        std::vector<std::string> temp;
        std::string str;
        for(int i=0;i<val.size() ; i++){
            if(val[i]=='"'){
                if(qot){
                    qot =0;
                    str.push_back('"');
                    temp.push_back(str);
                    str.clear();
                }
                else {
                    temp.push_back(str);
                    str.clear();
                    str.push_back('"');
                    qot=1;
                }
            }
            else if(qot)str.push_back(val[i]);
            else if((val[i]>='a')&&(val[i]<='z')){
                str.push_back(val[i]);
            }
            else if((val[i]>='A')&&(val[i]<='Z')){
                str.push_back(val[i]-'A'+'a');
            }
            else if((val[i]>='0')&&(val[i]<='9')){
                str.push_back(val[i]);
            }
            else if(do_we_support_this_sign(val[i])){
                if(!str.empty())temp.push_back(str);

                temp.push_back(std::string(1,val[i]));

                str.clear();
            }
            else if(val[i]==' '){
                if(!str.empty())temp.push_back(str);
                str.clear();
            }
            else error_filo("sign '"+ std::string(1,val[i]) + "' not supported");
        }
        if(!str.empty())temp.push_back(str);
        cvs.push_back(temp);
    }
}

int arithmetic(int x , int y ,std::string s){
    if(s=="+")return (x+y);
    else if(s=="-")return (x-y);
    else if(s=="*")return (x*y);
    else if(s=="/")return (x/y);
    else error_filo("invalid arithmetic sign");
}

bool is_int(std::string s){
    bool ans=1;
    for(auto val:s)if((val<'0')||(val>'9'))ans=0;
    return ans;
}

std::pair<int , int>  calc_int(std::vector<std::vector<std::string>> &val , int y , int x , bool fs){
    int cn=0;
    bool fr=1;
    std::string sign = "+";
    for(;x<val[y].size() ; x++){
        if(val[y][x]==")"){
            return (std::pair{cn , x});
        }
        else if(val[y][x]=="("){
            std::pair<int , int> ca;
            ca = calc_int(val , y ,  x+1 , 0);
            cn = arithmetic(cn , ca.first , sign);
            sign = "";
            x = ca.second;
        }
        else if(is_int(val[y][x])){
            int b = stoi(val[y][x]);
            cn = arithmetic(cn , b , sign);
            sign = "";
        }
        else if(var_exist(val[y][x])){
            if(read_variable_type(val[y][x])!="int")error_filo("variable"+ val[y][x] + "cannot be use as int");
            int b = std::get<int>(variables[val[y][x]]);
            cn = arithmetic(cn , b , sign);
            sign = "";
        }
        else {
            if((!sign.empty())&&(!fr))error_filo("integer value not found for arithmetic , at line ->"+ std::to_string(y));
            sign = val[y][x];
        }
        if(fr)fr=0;
    }
    if(fs)return (std::pair<int , int>{cn , x});
    error_filo("no closing bracket found");
}

void set_int(std::vector<std::vector<std::string>> &val , int y , int x){
    x++;
    std::string var_name = val[y][x];
    bool er=0;
    std::pair<int , int> te;
    for(auto var:var_name) if((var>'z')||(var<'a'))er=1;

    if(er)error_filo("only a-z character allowed as variable name \n error at line ->"+ std::to_string(y+1));
    x++;
    if(val[y][x]=="="){
        te = calc_int(val , y , x+1 , 1);
    }
    else error_filo("invalid syntax \n error at line ->"+ std::to_string(y+1));

    if(var_exist(var_name))error_filo("cannot create 2 variable with same name , at line ->"+ std::to_string(y));
    write_variable_type(var_name , "int");
    variables[var_name] = (int)te.first;
}

void set_str(std::vector<std::vector<std::string>> &val , int y , int x){
    x++;
    std::string name = val[y][x];
    bool er=0;
    for(auto var:name)if((var>'z')&&(var<'a'))er=1;

    if(er)error_at_line("only a-z characters are allowed as variable name" , y);

}

void process_data_structure(std::vector<std::vector<std::string>> &val , int y , int x){
    std::string type = val[y][x];
    if(type=="int")set_int(val , y , x);
    if(type == "str")set_str(val , y , x);
    else error_filo("invalid data type at line ->"+ std::to_string(y));

}

void interp(std::vector<std::vector<std::string>> &val){
    for(int y=0 ; y<val.size() ; y++){
        if(val[y].empty())continue;

        if(do_we_support_this_data_structure(val[y][0])){
            process_data_structure(val , y , 0);
        }
    }
}




