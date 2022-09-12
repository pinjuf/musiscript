#include "util.h"
#include "config.h"

#include <sstream>
#include <iomanip>
#include <fstream> 

std::vector<std::string> split_string(std::string str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;
    while(getline(ss, tok, delimiter)) {
        if (!tok.empty())
            internal.push_back(tok);
    }
    return internal;
}

std::string replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string dtostr(double d) {
    std::ostringstream oss;
    oss << std::setprecision(DIGIT_PREC);
    oss << d;
    return oss.str();
}

std::string strip_line(std::string str) {
    str.erase(0, str.find_first_not_of(" \n\r\t"));
    str.erase(str.find_last_not_of(" \n\r\t") + 1);
    return str;
}

size_t get_current_line(std::ifstream& file) {
    size_t current_pos = file.tellg();
    size_t original_pos = current_pos;
    size_t line_count = 0;
    while(current_pos > 0) {
        current_pos--;
        file.seekg(current_pos);
        if (file.peek() == '\n') {
            line_count++;
        }
    }

    file.seekg(original_pos);

    return line_count;
}

