#ifndef MATH_H
#define MATH_H

#define OPERATORS "^*/+-"

#include <vector>
#include <string>
#include <complex>

void fft(std::vector<double> * in, std::vector<std::complex<double>> * out);
void ifft(std::vector<std::complex<double>> * in, std::vector<double> * out);

int rpn(const std::string & in, double * out);
int lrpn(std::string in, bool * out);

std::vector<std::string> split_infix(std::string in);
std::vector<std::string> split_infix_logical(std::string in);
int shunting_yard(std::vector<std::string> in, std::vector<std::string> & out);
int shunting_yard_logical(std::vector<std::string> in, std::vector<std::string> & out);

int eval_infix(std::string in, double * out);
int eval_infix_logical(std::string in, bool * out);

double get_at_in_band(double in, double * band, size_t band_size);

#endif
