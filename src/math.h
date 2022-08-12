#ifndef MATH_H
#define MATH_H

#include <string>

void fft(double * in, double * out, int n, int sign);
void ifft(double * in, double * out, int n, int sign);

int rpn(std::string in, double * out);
int lrpn(std::string in, bool * out);

#endif
