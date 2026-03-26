#pragma once
#include <vector>
#include <string>
#include "../stora/stora.h"
void load_code(int argc , char* argv[]);
void kuser(std::vector<std::string> &st , std::vector<std::string> &who);
void voimer(std::vector<std::string> &who , std::vector<code_struct> &code , int start , int end);
void gloaby(std::vector<code_struct>&code);
