#pragma once
#include <vector>
#include <string>
#include <map>
#include <variant>
// #include <memory>
struct code_struct{
    std::string name;
    std::vector<std::string> ln;
};
struct var;


struct var{
    // std::string type;
    // std::variant<int , std::string , float ,std::vector<std::shared_ptr<var>> , std::map<std::string,std::shared_ptr<var>>> value;
    std::variant<int , std::string , float , std::vector<var> , std::map<std::string , var>> value;
};

struct funct{
    std::string name;
    std::vector<std::pair<std::string , std::string>> carry_var_name;
    std::map<std::string , var> variables;
    std::vector<code_struct> code;
};

extern std::vector<std::string> code_str;
extern std::vector<std::string> code_who;
extern bool debug;
extern std::vector<code_struct> code;
extern std::map<std::string , var> global_variable;
extern std::map<std::string , funct> functions;