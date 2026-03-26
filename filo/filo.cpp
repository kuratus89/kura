#include <iostream>
#include <fstream>
#include <cstdlib>
#include "../stora/stora.h"
#include "../debug/debug.h"
#include "rui.h"
#include "function.h"

void error_filo(std::string s){
    print_debug("program crash in filo");
    print_debug(s);
    exit(1);
}




void load_code(int argc , char* argv[]){
    if(argc<2)error_filo("no code found");

    std::ifstream file(argv[1]);

    if(!file)error_filo("fail to load code");

    std::string lin;

    while(std::getline(file , lin)){
        code_str.push_back(lin);
    }
}



void kuser(std::vector<std::string> &st , std::vector<std::string> &who){
    bool str=0;
    for(auto &val:st){
        std::string temp;
        for(auto &var:val){
            if(str){
                if(var=='"'){
                    str=0;
                    who.push_back(temp);
                    temp.clear();
                    who.push_back(std::string(1,'"'));
                }
                else {
                    temp.push_back(var);
                }
            }
            else {
                if(((var>='a')&&(var<='z'))||((var<='0')&&(var>='9')))temp.push_back(var);
                else if((var>='A')&&(var<='Z'))temp.push_back(var - 'A'+'a');
                else{
                    if(!temp.empty())who.push_back(temp);
                    temp.clear();
                    if(var!=' ')who.push_back(std::string(1 , var));
                    if(var=='"')str=1;
                }
            }
        }
        if(!temp.empty())who.push_back(temp);
    }
}

void count_bra(std::string s , int &cur , int &rou){
    if(s=="{")cur++;
    else if(s=="}")cur--;
    else if(s=="(")rou++;
    else if(s==")")rou--;
}

void voimer(std::vector<std::string> &who , std::vector<code_struct> &code , int start , int end){
    int cr=0;
    int ru=0;
    for(int i = start;(i<who.size())&&(i<end) ; i++){
        code_struct cn;
        cn.name = who[i];
        i++;
        for(;(i<who.size()); i++){
            count_bra(who[i] , cr , ru);
            cn.ln.push_back(who[i]);
            if((cr==0)&&(ru==0)&&(who[i]==";"))break;
        }        
        if((cr!=0)||(ru!=0))error_filo("no closing bracket found");
        if(i>=who.size())error_filo("missing semicolon");
        code.push_back(cn);  
    }
}

void gloaby(std::vector<code_struct> &code){
    std::vector<std::map<std::string , var>*> varib;
    varib.push_back(&global_variable);
    for(auto val:code){
        if(is_data_structure(val.name))process_data_structure(val , varib);
        else if(val.name == "func") process_func(val);
    }
}
