#pragma once

#include <vector>
#include <string>

std::vector<std::string> split_string(std::string str, char delimiter);
std::string replace_all(std::string& str, const std::string& from, const std::string& to);
std::string dtostr(double d);
std::string strip_line(std::string str);
size_t get_current_line(std::ifstream& file);
