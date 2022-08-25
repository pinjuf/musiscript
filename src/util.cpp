#include "util.h"
#include "config.h"

#include <sstream>
#include <iomanip>

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
    oss << std::setprecision(DIGIT_PREC); // Never enough precision for a double, TODO: Move to prec to config and func to utils
    oss << d;
    return oss.str();
}
