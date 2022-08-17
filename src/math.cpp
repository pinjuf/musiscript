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

int get_op_priority(std::string op) {
    if (op == "+" || op == "-")
        return 1;
    else if (op == "*" || op == "/")
        return 2;
    else if (op == "^")
        return 3;

    return -1;
}

std::vector<std::string> split_infix(std::string in) { // This needs to be rewritten.
    std::vector<std::string> out;
    std::string tmp;

    for (int i = 0; i < in.size(); i++) {
        if (in[i] == ' ') {         // Space is a clear separator
            if (tmp.size() > 0) {
                out.push_back(tmp);
                tmp = "";
            }
            continue;
        }

        bool is_single_special_char = false;
        for (int j = 0; j < strlen(OPERATORS"(),"); j++) {
            if (in[i] == (OPERATORS"(),")[j]) {
                is_single_special_char = true;
                break;
            }
        }

        if (is_single_special_char) { // Some operator/parentheses/comma was passed
            if (in[i] == '-') { // A minus can be a unary operator or a negative number
                if (out.empty() || get_op_priority(out.back()) > 0 || out.back() == "(") {
                    tmp += in[i];
                    continue;
                }
            }
            if (tmp.size() > 0) {
                out.push_back(tmp);
                tmp = "";
            }
            out.push_back(std::string(1, in[i]));
        } else { // Any other char
            tmp += in[i];
        }
    }

    if (tmp.size() > 0) {
        out.push_back(tmp);
    }

    return out;
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
                if (val_stack.size() > 1) {
                    a = val_stack.top();val_stack.pop();
                    b = val_stack.top();val_stack.pop();
                    val_stack.push(b-a);
                } else if (val_stack.size() == 1) {
                    a = val_stack.top();val_stack.pop();
                    val_stack.push(-a);
                } else {
                    return -1;
                }
            }   
            else if (!strcmp(comm.c_str(), "/")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(b/a);
            }
            else if (!strcmp(comm.c_str(), "mod")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(fmod(b,a));
            }

            else if (!strcmp(comm.c_str(), "pow") || !strcmp(comm.c_str(), "^")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(pow(b, a));
            }   
            else if (!strcmp(comm.c_str(), "sqrt")) {
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
        if (!strcmp(comm.c_str(), "true") || !strcmp(comm.c_str(), "1"))
            val_stack.push(true);
        else if (!strcmp(comm.c_str(), "false") || !strcmp(comm.c_str(), "0"))
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

bool is_left_associated(std::string op) {
    if (op == "^")
        return false;
    else if (op == "+" || op == "-")
        return true;
    else if (op == "*" || op == "/")
        return true;
    return false;
}

int shunting_yard(std::vector<std::string> in, std::vector<std::string> &out) {
    // Shunting yard algorithm
    // Input: tokens in infix notation
    // Output: tokens in postfix notation
    // Source: https://en.wikipedia.org/wiki/Shunting_yard_algorithm

    std::stack<std::string> op_stack;

    for (std::string curr : in) {
        try { // Scenario 1: Number
            stod(curr, NULL);
            out.push_back(curr);
            continue;
        } catch (std::invalid_argument) {}

        if (curr == "(") { // Scenario 2: Open Parenthesis
            op_stack.push(curr);
            continue;
        }

        if (curr == ")") { // Scenario 3: Close Parenthesis
            if (op_stack.empty()) {
                return -1;
            }
            while (op_stack.top() != "(") {
                out.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop(); // Discard top parenthesis

            // Check if top == function
            if (!op_stack.empty() && get_op_priority(op_stack.top()) == -1 && op_stack.top() != "(") {
                out.push_back(op_stack.top());
                op_stack.pop();
            }

            continue;
        }

        bool is_operator = false;
        for (char ch : OPERATORS) {
            if (curr == (std::string)&ch) {
                is_operator = true;
                break;
            }
        }
        if (is_operator) { // Scenario 4: Operator
            while (!op_stack.empty()) {
                std::string curr_top = op_stack.top();
                if (curr_top == "(") {
                    break;
                }

                if (!( // Forgive me, oh future debugger
                    (get_op_priority(curr_top)>get_op_priority(curr)) ||
                    (get_op_priority(curr_top)==get_op_priority(curr) && is_left_associated(curr_top))
                    )) {
                    break;
                }

                op_stack.pop();
                out.push_back(curr_top);
            }
            op_stack.push(curr);

            continue;
        }

        if (curr == ",") { // Ignore comma
            continue;
        }

        // Scenario 5: Function
        op_stack.push(curr);
    }

    while (!op_stack.empty()) {
        out.push_back(op_stack.top());
        if (op_stack.top() == "(") { // Mismatched parenthesis
            return -1;
        }
        op_stack.pop();
    }

    return 0;
}

int eval_infix(std::string in, double * out) {
    // First, convert input to postfix notation, then pass to RPN
    std::vector<std::string> tokens = split_infix(in);
    std::vector<std::string> postfix;
    if (shunting_yard(tokens, postfix) == -1) {
        return -1;
    }
    std::string tmp;
    for (std::string curr : postfix)
        tmp += curr + " ";
    if (rpn(tmp, out) == -1) {
        return -1;
    }

    return 0;
}
