#include <cmath>
#include <vector>
#include <stack>
#include <string.h>
#include <sstream>

#include "math.h"

void fft(double * in, double * out, int n, int sign) {
    if (n == 1) {
        out[0] = in[0];
        return;
    }
    double * even = new double[n / 2];
    double * odd = new double[n / 2];
    for (int i = 0; i < n / 2; i++) {
        even[i] = in[2 * i];
        odd[i] = in[2 * i + 1];
    }
    fft(even, out, n / 2, sign);
    fft(odd, out + n / 2, n / 2, sign);
    for (int i = 0; i < n / 2; i++) {
        double t = out[i + n / 2];
        double w = exp(-sign * 2 * M_PI * i / n);
        out[i + n / 2] = out[i] - w * t;
        out[i] += w * t;
    }
    delete[] even;
    delete[] odd;
}

void ifft(double * in, double * out, int n, int sign) {
    fft(in, out, n, -sign);
    for (int i = 0; i < n; i++) {
        out[i] /= n;
    }
}

int rpn(std::string in, double * out) {
    std::stack<double> val_stack;
    double a, b;
    std::vector<std::string> input;
    std::stringstream ss(in);
    std::string tok;
    while(getline(ss, tok, ' ')) {
        if (!tok.empty())
            input.push_back(tok);
    }

    for (std::string comm : input) {
        try {
            val_stack.push(stod(comm, NULL));
        } catch (std::invalid_argument) {
            if (!strcmp(comm.c_str(), "+")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(a+b);
            }   
            else if (!strcmp(comm.c_str(), "*")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(a*b);
            }   
            else if (!strcmp(comm.c_str(), "-")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(b-a);
            }   
            else if (!strcmp(comm.c_str(), "/")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(b/a);
            }

            else if (!strcmp(comm.c_str(), "^")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(pow(b, a));
            }   
            else if (!strcmp(comm.c_str(), "v")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(sqrt(a));
            }   
            else if (!strcmp(comm.c_str(), "ln")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(log(a));
            }   

            else if (!strcmp(comm.c_str(), "sum")) {
                a = 0;
                while (!val_stack.empty()) {
                    a += val_stack.top();val_stack.pop();
                }
                val_stack.push(a);
            }

            else {
                return -1;
            }
        }
    }

    if (val_stack.empty()) {
        return -1;
    }

    *out = val_stack.top();

    return 0;
}

int lrpn(std::string in, bool * out) { // Logical RPN, essentially a RPN with boolean operators
    std::stack<bool> val_stack;
    bool a, b;
    std::vector<std::string> input;
    std::stringstream ss(in);
    std::string tok;
    while(getline(ss, tok, ' ')) {
        if (!tok.empty())
            input.push_back(tok);
    }

    for (std::string comm : input) {
        if (!strcmp(comm.c_str(), "true"))
            val_stack.push(true);
        else if (!strcmp(comm.c_str(), "false"))
            val_stack.push(false);
        else if (!strcmp(comm.c_str(), "!")) {
            if (val_stack.size() < 1) {return -1;}
            bool a = val_stack.top();val_stack.pop();
            a =! a;
            val_stack.push(a);
        }
        else if (!strcmp(comm.c_str(), "&&")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(a && b);
        }
        else if (!strcmp(comm.c_str(), "||")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(a || b);
        }
        else {
            return -1;
        }
    }

    if (val_stack.empty()) {
        return -1;
    }

    *out = val_stack.top();

    return 0;
}
