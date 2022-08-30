#include <cmath>
#include <vector>
#include <stack>
#include <string.h>
#include <sstream>

#include "math.h"
#include "util.h"

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

std::string replace_unary_ops(std::string & in) {
    // Basic concept
    // 1. Read the input string from left to right
    // 2. If it's an operator, add it to the positive var
    // 3. If it's something else and we're parsing,
    //    replace the last operator according to the positive var and the following ops with spaces

    bool parsing = false;
    bool positive = true;

    size_t last_pos = 0;

    for (size_t i = 0; i < in.length(); i++) {
        if (in[i] == '+') {
            if (!parsing)
                last_pos = i;
            parsing = true;
        }
        else if (in[i] == '-') {
            if (!parsing) {
                last_pos = i;
            }
            positive = !positive;
            parsing = true;
        }
        else if (parsing) {
            parsing = false;
            if (positive)
                in[last_pos] = '+';
            else
                in[last_pos] = '-';

            for (size_t j = last_pos + 1; j < i; j++)
                in[j] = ' ';

            positive = true;
        }
    }

    return in;
}

std::vector<std::string> split_infix(std::string in) { // Tokenizer, needs to be tested
    in = strip_line(in);
    in = replace_all(in, " ", "");
    in = replace_unary_ops(in);
    in = replace_all(in, "(-(", "(0-("); // This is a hack to make the parser work
    in = replace_all(in, "(+(", "(0+(");
    // TODO: Check for the following patters: ^[+-][a-Z](  | Example: -sin({pi}/4)
    //                                        ([+-][a-Z](  | Example: 1+(-sin({pi}/4))

    std::vector<std::string> out;
    std::string tmp;

    for (size_t i = 0; i < in.size(); i++) {
        // Check if the current char can cause a split
        bool is_splitter = false;
        for (size_t j = 0; j < sizeof(OPERATORS" (),") / sizeof(char); j++) {
            if (in[i] == OPERATORS" (),"[j]) {
                is_splitter = true;
                break;
            }
        }

        if (is_splitter) {
            if (in[i] == ' ') {
                continue;
            }

            if (in[i] == '+' || in[i] == '-') { // Check if the current char is a sign
                if (tmp.empty()) { // If the current token is empty, it's likely a sign
                    if (out.empty()) { // If the output is empty, it's a sign
                        tmp += in[i];
                        continue;
                    } else if (out.back() != ")") { // If the last token is a bracket, it's a sign
                        tmp += in[i];
                        continue;
                    }
                }
            }

            if (tmp.size() > 0) {
                out.push_back(tmp);
                tmp.clear();
            }
            out.push_back(std::string(1, in[i]));

        } else { // Normal char, add it to the tmp string
            tmp += in[i];
        }
    }

    if (tmp.size() > 0) {
        out.push_back(tmp);
    }

    return out;
}

std::vector<std::string> split_infix_logical(std::string in) { // Tokenizer for logical expressions
    std::vector<std::string> out;
    std::string tmp;

    for (size_t i = 0; i < in.size(); i++) {
        if (in[i] == ' ') {         // Space is a clear separator
            if (tmp.size() > 0) {
                out.push_back(tmp);
                tmp = "";
            }
            continue;
        }

        if (in[i] == '(' || in[i] == ')') { // Parentheses are clear separators
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
    std::vector<std::string> input = split_string(in, ' ');
    int sign;
    bool has_sign;

    for (std::string comm : input) {
        has_sign = false;
        sign = 1;
        if (comm[0] == '-') { // Hacky fix for function tokens like -sin(...)
            sign = -1;
            has_sign = true;
        } else if (comm[0] == '+') {
            sign = 1;
            has_sign = true;
        }

        try {
            val_stack.push(stod(comm, NULL));
        } catch (std::invalid_argument const&) {
            // Operators
            if (!strcmp(comm.c_str(), "+")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(a+b);
                continue;
            }   
            if (!strcmp(comm.c_str(), "*")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(a*b);
                continue;
            }   
            if (!strcmp(comm.c_str(), "-")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(b-a);
                continue;
            }   
            if (!strcmp(comm.c_str(), "/")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(b/a);
                continue;
            }
            if (!strcmp(comm.c_str(), "^")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(pow(b, a));
                continue;
            }

            // Functions
            if (has_sign)
                comm = comm.substr(1);
            if (!strcmp(comm.c_str(), "mod")) {
                if (val_stack.size() < 2) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(fmod(b,a)*sign);
            }

            else if (!strcmp(comm.c_str(), "pow")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                b = val_stack.top();val_stack.pop();
                val_stack.push(pow(b, a)*sign);
            }   
            else if (!strcmp(comm.c_str(), "sqrt")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(sqrt(a)*sign);
            }   
            else if (!strcmp(comm.c_str(), "ln")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(log(a)*sign);
            }   

            else if (!strcmp(comm.c_str(), "sin")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(sin(a)*sign);
            }
            else if (!strcmp(comm.c_str(), "cos")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(cos(a)*sign);
            }
            else if (!strcmp(comm.c_str(), "tan")) {
                if (val_stack.size() < 1) {return -1;}
                a = val_stack.top();val_stack.pop();
                val_stack.push(tan(a)*sign);
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
        else if (!strcmp(comm.c_str(), "not")) {
            if (val_stack.size() < 1) {return -1;}
            bool a = val_stack.top();val_stack.pop();
            a =! a;
            val_stack.push(a);
        }
        else if (!strcmp(comm.c_str(), "and")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(a && b);
        }
        else if (!strcmp(comm.c_str(), "or")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(a || b);
        }
        else if (!strcmp(comm.c_str(), "xor")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(a ^ b);
        }
        else if (!strcmp(comm.c_str(), "nand")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(!(a && b));
        }
        else if (!strcmp(comm.c_str(), "nor")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(!(a || b));
        }
        else if (!strcmp(comm.c_str(), "xnor")) {
            if (val_stack.size() < 2) {return -1;}
            a = val_stack.top();val_stack.pop();
            b = val_stack.top();val_stack.pop();
            val_stack.push(!(a ^ b));
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
        } catch (std::invalid_argument const&) {}

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

        bool is_operator = get_op_priority(curr) != -1;
        if (is_operator) { // Scenario 4: Operator
            while (!op_stack.empty()) {
                std::string curr_top = op_stack.top();
                if (curr_top == "(") {
                    break;
                }
                if (get_op_priority(curr_top) < 0) {
                    break;
                }

                if (!( // Forgive me, oh future debugger
                    (get_op_priority(curr_top)>get_op_priority(curr)) ||
                    (get_op_priority(curr_top)==get_op_priority(curr) && is_left_associated(curr))
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

int shunting_yard_logical(std::vector<std::string> in, std::vector<std::string> &out) {
    std::stack<std::string> op_stack;

    for (std::string curr : in) {
        if (curr == "true" || curr == "false" || curr == "0" || curr == "1") { // Scenario 1: Number
            out.push_back(curr);
        }

        else if (curr == "(") { // Scenario 2: Open Parenthesis
            op_stack.push(curr);
        }

        else if (curr == ")") { // Scenario 3: Close Parenthesis
            if (op_stack.empty()) {
                return -1;
            }

            while (op_stack.top() != "(") {
                out.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop(); // Discard top parenthesis

            if (!op_stack.empty() && op_stack.top()=="not") {
                out.push_back(op_stack.top());
                op_stack.pop();
            }
        }

        else if (curr == "not") { // Scenario 4: Not (only function)
            op_stack.push(curr);
        }

        else { // Scenario 5: Operator
            while (!op_stack.empty()) {
                std::string curr_top = op_stack.top();
                if (curr_top == "(") {
                    break;
                }

                op_stack.pop();
                out.push_back(curr_top);
            }
            op_stack.push(curr);
        }
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
    for (std::string curr : postfix) // That's a bodge
        tmp += curr + " ";

    if (rpn(tmp, out) == -1) {
        return -1;
    }

    return 0;
}

int eval_infix_logical(std::string in, bool * out) {
    std::vector<std::string> tokens = split_infix_logical(in);
    std::vector<std::string> postfix;
    if (shunting_yard_logical(tokens, postfix) == -1) {
        return -1;
    }
    std::string tmp;
    for (std::string curr : postfix)
        tmp += curr + " ";
    if (lrpn(tmp, out) == -1) {
        return -1;
    }

    return 0;
}
