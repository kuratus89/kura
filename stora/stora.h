#pragma once
#include <vector>
#include <string>
#include <map>
#include <variant>
extern std::vector<std::string> code_str;
extern std::vector<std::vector<std::string>> code;
extern bool debug;
using var = std::variant<int , std::string>;
extern std::map<std::string , var> variables;

void write_variable_type(std::string s , std::string type);
bool do_we_support_this_sign(char s);
bool do_we_support_this_data_structure(std::string s);
bool var_exist(std::string s);
std::string read_variable_type(std::string s);

