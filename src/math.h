#ifndef MATH_H
#define MATH_H

#include <cmath>

void fft(double * in, double * out, int n, int sign);
void ifft(double * in, double * out, int n, int sign);

#endif
