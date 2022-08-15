#include "codepts.h"

#include <stdexcept>
#include <vector>
#include <sstream>

int eval_codepointer(std::string input, std::string * output) {

    std::vector<std::string> tokens;

    std::stringstream ss(input);
    std::string tok;
    while (getline(ss, tok, ' ')) {
        if (!tok.empty())
            tokens.push_back(tok);
    }

    if (tokens.size() == 0)
        return -1;

    if (!strcmp(tokens[0].c_str(), "numeq")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }

        if (a==b)
            *output = "true";
        else
            *output = "false";
    }

    if (!strcmp(tokens[0].c_str(), "numgt")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }

        if (a>b)
            *output = "true";
        else
            *output = "false";
    }

    if (!strcmp(tokens[0].c_str(), "numgeqt")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }

        if (a>=b)
            *output = "true";
        else
            *output = "false";
    }

    return 0;
}
