#pragma once
#include <vector>
#include <string>
void load_code(int argc , char* argv[]);
void parser(std::vector<std::string> &cst , std::vector<std::vector<std::string>> &cvs);
void interp(std::vector<std::vector<std::string>> &val);