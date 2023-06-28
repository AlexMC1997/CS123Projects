#ifndef FILTER_H
#define FILTER_H

#include "RGBA.h"
#include "qimage.h"
#include <vector>
#include <memory>
#include <complex>

namespace Filter
{
    void identity(int w, int h, const RGBA* src, RGBA* dst);
    void intensity(int w, int h, const RGBA* src, RGBA* dst);
    void intensity(int w, int h, const RGBA* src, char* dst);
    void intensity(int w, int h, const RGBA* src, float* dst);
    QImage* frequency(const QImage& src);
    void genTable(int d);
    std::complex<float> dft(float* space, int N, int x);
    std::complex<float> dft(std::complex<float>* space, int N, int x);
    std::complex<float> idft(std::complex<float>* space, int N, int v);
    void fftv(float* space, std::complex<float>*, int N, int stride);
    void fftv(std::complex<float>* space, std::complex<float>* dst, int N, int stride);
    void fftv2d(float* space, std::complex<float>* dst, int dims, int stride);
    void fftv2d(std::complex<float>* space, std::complex<float>* dst, int dims, int stride);
    void fftv2dl(float* space, std::complex<float>* dst, int w, int h, int dw, int stride);
    void fftv2dl(std::complex<float>* space, std::complex<float>* dst, int w, int h, int dw, int stride);
    void ifftv(std::complex<float>* space, std::complex<float>*, int N, int stride);
};

#endif // FILTER_H
