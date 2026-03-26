#include "../stora/stora.h"
#include "../debug/debug.h"

int arithmetic(int x , int y , std::string sign){
    if(sign=="+")return (x+y);
    else if(sign=="-")return (x-y);
    else if(sign=="*")return (x*y);
    else if(sign=="/")return (x/y);
    else error_exit("invalid arithmetic sign-> '" + sign +"'");
}

bool is_int(std::string s){
    for(auto val:s)if((val>'9')||(val<'0'))return 0;
    return 1;
}

bool is_str(std::string s){
    for(auto val:s)if((val>'z')||(val<'a'))return 0;
    return 1;
}

bool vari_int(std::vector<std::map<std::string , var>*> &varib ,int &cn ,  std::string &n ,std::string &sign){
    for(auto val:varib){
        if((*val).count(n)){
            if(!std::holds_alternative<int>((*val)[n].value))error_exit(n + " is not a <int> variable");
            cn = arithmetic(cn , std::get<int>((*val)[n].value) , sign);
            sign = "";
            return 1;
        }
    }
    return 0;
}

bool vari_str(std::vector<std::map<std::string , var>*> &varib , std::string &s , std::string &n){
    for(auto val:varib){
        if((*val).count(n)){
            if(!std::holds_alternative<std::string>((*val)[n].value))error_exit(n + " is not a <string> variable");
            s+=std::get<std::string>((*val)[n].value);
            return 1;
        }
    }
    return 0;
}


int int_merjo(std::vector<std::string> &lin , int x  , std::vector<std::map<std::string , var>*> &varib){
    int cn=0;
    bool fir=1;
    std::string sign="+";
    for(;lin[x]!=";" ; x++){
        if(lin[x]==")")return cn;
        if(lin[x]=="("){
            int b;
            b = int_merjo(lin , x+1  , varib);
            cn = arithmetic(cn, b , sign);
            sign="";
            while((x<lin.size())&&(lin[x]!=")"))x++;
        }
        else if(is_int(lin[x])){
            cn = arithmetic(cn , std::stoi(lin[x]) , sign);
            sign = "";
        }
        else if(!vari_int(varib , cn , lin[x] , sign)){
            if((!fir)&&(sign!=""))error_exit("invalid syntax");
            sign = lin[x];
        }
        if(fir)fir=0;
    }
    return cn;
}



void process_int(code_struct&code , int x ,std::vector<std::map<std::string , var>*> &varib){

    for(auto val:varib)if((*val).count(code.ln[x]))error_exit("variable " + code.ln[x] + " already exist");
    std::string name = code.ln[x];
    int val;
    x++;
    if(code.ln[x]=="=")val = int_merjo(code.ln , x+1 , varib);
    else error_exit("invalid syntax");

    (*varib.back())[name].value = (int)val;
}


std::string str_merjo(std::vector<std::string> &lin , int &x , std::vector<std::map<std::string , var>*> &varib){
    std::string s;
    bool sign =0;
    for(;lin[x]!=";" ; x++){
        if(lin[x]==")")return s;
        if(sign){
            sign=0;
            if(lin[x]!="+")error_exit("invalid syntax");
        }
        else{
            sign =1;
            if(lin[x]=="("){
                x++;
                s+=str_merjo(lin , x , varib);
            }
            else if(lin[x]=="\""){
                x++;
                s+=lin[x];
                x++;
            }
            else if(!vari_str(varib , s , lin[x])){
                error_exit("invalid syntax");
            }
        }
        
    }
    return s;
}

void process_str(code_struct&code , int x , std::vector<std::map<std::string , var>*> &varib){
    for(auto val:varib)if((*val).count(code.ln[x]))error_exit("variable "+ code.ln[x] +" aleready exist");
    std::string name = code.ln[x];
    std::string val;
    int l = x+2;
    if(code.ln[x+1]=="=")val =str_merjo(code.ln , l , varib);
    else error_exit("invalid syntax");

    (*varib.back())[name].value = val;

}
std::map<std::string , void(*)(code_struct& , int y ,  std::vector<std::map<std::string , var>*>&)> ma={
    {"int" , process_int},
    {"string" , process_str},
    {"str" , process_str}
};

void process_data_structure(code_struct &code , std::vector<std::map<std::string , var>*> &varib){
    if(ma.count(code.name))ma[code.name](code, 0 , varib);
    else error_exit("invalid syntax");
}

bool is_data_structure(std::string s){
    return (ma.count(s));
}