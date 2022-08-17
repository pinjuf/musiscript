#include "codepts.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <random>

std::string dtostr(double d)
{
    std::ostringstream oss;
    oss << std::setprecision(32);
    oss << d;
    return oss.str();
}

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

    if (!strcmp(tokens[0].c_str(), "numeq") ||
        !strcmp(tokens[0].c_str(), "==")) {
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

    if (!strcmp(tokens[0].c_str(), "numgt") ||
        !strcmp(tokens[0].c_str(), ">")) {
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

    if (!strcmp(tokens[0].c_str(), "numlt") ||
        !strcmp(tokens[0].c_str(), "<")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }

        if (a<b)
            *output = "true";
        else
            *output = "false";
    }

    if (!strcmp(tokens[0].c_str(), "numgeqt") ||
        !strcmp(tokens[0].c_str(), ">=")) {
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

    if (!strcmp(tokens[0].c_str(), "numleqt") ||
        !strcmp(tokens[0].c_str(), "<=")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;
        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }

        if (a<=b)
            *output = "true";
        else
            *output = "false";
    }

    if (!strcmp(tokens[0].c_str(), "pi")) {
        *output = dtostr(M_PI);
    }

    if (!strcmp(tokens[0].c_str(), "randint")) {
        if (tokens.size() != 3)
            return -1;
        int a, b;
        try {
            a = std::stoi(tokens[1]);
            b = std::stoi(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }
        int res = a + rand() % (b - a);
        *output = dtostr(res);
    }

    if (!strcmp(tokens[0].c_str(), "randfloat")) {
        if (tokens.size() != 3)
            return -1;
        double a, b;
        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument) {
            return -1;
        }
        double res = a + (b - a) * ((double)rand() / (double)RAND_MAX);
        *output = dtostr(res);
    }

    return 0;
}
