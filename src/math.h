#ifndef MATH_H
#define MATH_H

#define OPERATORS "^*/+-"

#include <vector>
#include <string>

void fft(const double * in, double * out, int n, int sign);
void ifft(double * in, double * out, int n, int sign);

int rpn(std::string in, double * out);
int lrpn(std::string in, bool * out);

std::vector<std::string> split_infix(std::string in);
std::vector<std::string> split_infix_logical(std::string in);
int shunting_yard(std::vector<std::string> in, std::vector<std::string> & out);
int shunting_yard_logical(std::vector<std::string> in, std::vector<std::string> & out);

int eval_infix(std::string in, double * out);
int eval_infix_logical(std::string in, bool * out);

#endif
