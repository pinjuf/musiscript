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
