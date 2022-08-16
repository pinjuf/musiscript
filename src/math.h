#ifndef MATH_H
#define MATH_H

#define OPERATORS "^*/+-" // Order matters! (shows priority)

#include <vector>
#include <string>

void fft(double * in, double * out, int n, int sign);
void ifft(double * in, double * out, int n, int sign);

int rpn(std::string in, double * out);
int lrpn(std::string in, bool * out);

std::vector<std::string> split_infix(std::string in);
int shunting_yard(std::vector<std::string> in, std::vector<std::string> & out);

int eval_infix(std::string in, double * out);

#endif
