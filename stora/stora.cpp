#include <vector>
#include <string>
#include <map>
#include <set>
#include <variant>
std::vector<std::string> code_str;
std::vector<std::vector<std::string>> code;
bool debug=0;
std::set<std::string> supported_signs = {
    "=",
    "+",
    "-",
    "*",
    "/",
    "(",
    ")",
    "\""
};

std::set<std::string> supported_data_structure = {
    "int",
    "string",
    "bool"
};

using var = std::variant<int , std::string >;

std::map<std::string ,var> variables;
std::map<std::string , std::string> var_type;

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

bool do_we_support_this_data_structure(std::string s){
    return supported_data_structure.count(s);
}

