#include <vector>
#include <string>
#include <map>
#include <set>
#include <variant>
std::vector<std::string> code_str;
std::vector<std::vector<std::string>> code;
bool debug=0;
std::map<std::string , bool> supported_signs = {
    {"=" , 1},
    {"+" , 0},
    {"-" , 0},
    {"*" , 1},
    {"/" , 0},
    {"(" , 0},
    {")" , 0},
    {"\"",0},
    {"<" ,1},
    {">" , 1}
};

std::set<std::string> supported_data_structure = {
    "int",
    "string",
    "bool"
};

std::set<std::string> supported_debug = {
    "print",
    "clear"
};

std::set<std::string> keywords = {
    "int",
    "string",
    "bool",
    "print",
    "clear",
    "stoi"
};

std::set<std::string> tools = {
    "stoi"
};

using var = std::variant<int , std::string >;

std::map<std::string ,var> variables = {
    {"br" , (std::string)"\n"}
};
std::map<std::string , std::string> var_type = {
    {"br" , "str"}
};

bool var_exist(std::string s){
    return var_type.count(s);
}

std::string read_variable_type(std::string s){
    return var_type[s];
}
void write_variable_type(std::string s , std::string type){
    var_type[s]=type;
}

bool do_we_support_this_sign(char s){
    return supported_signs.count(std::string(1,s));
}

bool stack_sign(char s){
    return supported_signs[std::string(1,s)];
}

bool do_we_support_this_data_structure(std::string s){
    return supported_data_structure.count(s);
}

bool do_we_support_this_debug(std::string s){
    return supported_debug.count(s);
}

bool is_keyword(std::string s){
    return keywords.count(s);
}

bool is_tool(std::string s){
    return tools.count(s);
}