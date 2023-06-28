#include "Filter.h"
#include <string.h>
#include <cmath>
#include <iostream>
#include <chrono>

static std::unique_ptr<std::vector<std::vector<std::complex<float>>*>> twidTable;

void Filter::genTable(int d) {
    twidTable = std::make_unique<std::vector<std::vector<std::complex<float>>*>>();
    for (int i = 0; i <= log2(d); i++) {
        int p = pow(2, i);
        auto E = std::complex<float>(1);
        auto V = std::exp(std::complex<float>(0, -2 * M_PI / p));
        auto sub = new std::vector<std::complex<float>>();
        for (int j = 0; j < p; j++) {
            sub->push_back(E);
            E *= V;
        }
        twidTable->push_back(sub);
    }
}

void Filter::identity(int w, int h, const RGBA* src, RGBA* dst) {
    memcpy(dst, src, w * h * sizeof(RGBA));
}

void Filter::intensity(int w, int h, const RGBA* src, RGBA* dst) {
    for (int i = 0; i < w * h; i++) {
        int mag = round(0.299 * src[i].r + 0.587 * src[i].g + 0.114 * src[i].b);
        dst[i].r = mag;
        dst[i].g = mag;
        dst[i].b = mag;
        dst[i].a = 255;
    }
}

void Filter::intensity(int w, int h, const RGBA* src, char* dst) {
    for (int i = 0; i < w * h; i++) {
        int mag = round(0.299 * src[i].r + 0.587 * src[i].g + 0.114 * src[i].b);
        dst[i] = mag;
    }
}

void Filter::intensity(int w, int h, const RGBA* src, float* dst) {
    for (int i = 0; i < w * h; i++) {
        int mag = round(0.299 * src[i].r + 0.587 * src[i].g + 0.114 * src[i].b);
        dst[i] = mag;
    }
}

QImage* Filter::frequency(const QImage& src) {
    int w = src.width();
    int h = src.height();
    auto grayscale = std::make_unique<std::vector<float>>();
    auto intermediate = std::make_unique<std::vector<std::complex<float>>>();
    auto Fmap = std::make_unique<std::vector<std::complex<float>>>();
    auto scaled = std::make_unique<std::vector<float>>();

    grayscale->reserve(w * h);
    Filter::intensity(w, h, reinterpret_cast<const RGBA*>(src.bits()), grayscale->data());

    int dim = 0;
    if (fmax(w, h) > 4096)
        return nullptr;

    switch ((int)fmax(w, h)) {
        case 16:
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:
        case 1024:
        case 2048:
        case 4096:
            dim = fmax(w, h);
            break;
       default:
            dim = ~(int)fmax(w, h);
            int cnt = 0;
            for (; dim != -1; dim >>= 1) {
                cnt++;
            }

            dim = 1 << cnt;
            break;
    }

    scaled->reserve(dim * dim);
    intermediate->reserve(dim * dim);
    Fmap->reserve(dim * dim);

    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            scaled->data()[y * dim + x] = y < h && x < w ? grayscale->data()[y * h + x] : 0;
        }
    }

//    for (int i = 0; i < w; i++) {
//        Filter::fftv(grayscale->data() + (i * w), intermediate->data() + (i * w), w, 1);
//    }
//    for (int i = 0; i < h; i++) {
//        Filter::fftv(intermediate->data() + i, Fmap->data() + (i * w), h, w);
//    }

    Filter::fftv2d(scaled->data(), Fmap->data(), dim, 1);

//    for (int i = 0; i < w; i++) {
//        Filter::ifftv(Fmap->data() + (i * w), intermediate->data() + (i * w), w, 1);
//    }
//    for (int i = 0; i < h; i++) {
//        Filter::ifftv(intermediate->data() + i, Fmap->data() + (i * w), h, w);
//    }
//    for (int i = 0; i < 128; i++) {
//        for (int j = 0; j < 128; j++) {
//            for (int k = 0; k < 128; k++) {
////                rows->at(k) = Filter::dft(grayscale->data() + (k * w), 128, j);
//                auto duo = Filter::fft(grayscale->data() + (k * w), 128, j);
//                rows->at(k) = duo[0];
//                if (k + 64 < 128)
//                rows->at(k + 64) = duo[1];
//            }
//            Fmap->push_back(Filter::dft(rows->data(), 128, i));
//        }
//    }


//    auto thing = Fmap->data();
//    for (int i = 0; i < h; i++) {
//        for (int j = 0; j < w; j++) {
//            unsigned char magc = round(thing[(i * w) + j].real());
//            dst[i * w + j] = RGBA{magc, magc, magc, 255};
//        }
//    }

    float dc = log2(std::norm(Fmap->data()[0]));
    int i = 0;
    auto result = new QImage(dim, dim, QImage::Format_RGBX8888);
    auto dst = reinterpret_cast<RGBA*>(result->bits());
//    for (int i = 0; i < 128; i++) {
//        for (int j = 0; j < 128; j++) {
//            for (int k = 0; k < 128; k++) {
//                rows->at(k) = Filter::idft(Fmap->data() + (k * 128), 128, j);
//            }
//            unsigned char val = round(Filter::idft(rows->data(), 128, i).real());
////            std::cout << val << std::endl;
//            dst[i * w + j] = RGBA{val, val, val, 255};
//        }
//    }

    for (; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            auto p = Fmap->data()[(((i + (dim / 2)) % dim) * dim) + ((j + (dim / 2)) % dim)];
            float mag = std::norm(p);
            float fmag = round(log2(mag) * (255 / dc));
            float Rscale = log2(fabs(p.real())) * (255 / dc);
            float Bscale = log2(fabs(p.imag())) * (255 / dc);
            unsigned char magc = fmag;
            unsigned char rmagc = round(Rscale);
            unsigned char bmagc = round(Bscale);
            dst[i * dim + j] = RGBA{magc, magc, magc, 255};
        }
    }
    return result;
}

void Filter::fftv(std::complex<float>* space, std::complex<float>* dst, int N, int stride) {
    int M = abs(N);
    if (M < 2) {
        dst[0] = space[0];
        return;
    }

    std::complex<float> E(1, 0);
    auto V = std::exp(std::complex<float>(0, -2 * M_PI / N));

    Filter::fftv(space, dst, N / 2, stride * 2);
    Filter::fftv(space + stride, dst + M / 2, N / 2, stride * 2);

    for (int y = 0; y < M / 2; y++) {
        auto a = dst[y];
        auto b = E * dst[y + (M / 2)];
        dst[y] = a + b;
        dst[y + (M/2)] = a - b;
        E *= V;
    }
}

void Filter::ifftv(std::complex<float>* space, std::complex<float>* dst, int N, int stride) {
    if (N < 2) {
        dst[0] = space[0];
        return;
    }

    float scale = 1.0 / N;
    std::complex<float> E(1, 0);
    auto V = std::exp(std::complex<float>(0, 2 * M_PI / N));

    Filter::fftv(space, dst, -N / 2, stride * 2);
    Filter::fftv(space + stride, dst + N / 2, -N / 2, stride * 2);

    for (int y = 0; y < N / 2; y++) {
        auto a = scale * dst[y];
        auto b = scale * E * dst[y + (N / 2)];
        dst[y] = a + b;
        dst[y + (N/2)] = a - b;
        E *= V;
    }
}

void Filter::fftv(float* space, std::complex<float>* dst, int N, int stride) {
    int M = abs(N);
    if (M < 2) {
        dst[0] = space[0];
        return;
    }

    Filter::fftv(space, dst, N / 2, stride * 2);
    Filter::fftv(space + stride, dst + M / 2, N / 2, stride * 2);

    std::complex<float> E(1, 0);
    auto V = std::exp(std::complex<float>(0, -2 * M_PI / N));

    for (int y = 0; y < M / 2; y++) {
        auto a = dst[y];
        auto b = E * dst[y + (M / 2)];
        dst[y] = a + b;
        dst[y + (M/2)] = a - b;
        E *= V;
    }
}

void Filter::fftv2d(float* space, std::complex<float>* dst, int dims, int stride) {
    int D = abs(dims);
    std::complex<float>* int_data = new std::complex<float>[dims * dims];
    std::complex<float> E(1, 0);

    if (dims < 2) {
        dst[0] = space[0];
        return;
    }

    Filter::fftv2dl(space, int_data, dims / 2, D, D, stride * 2);
    Filter::fftv2dl(space + stride, int_data + (D / 2), dims / 2, D, D, stride * 2);

    int half = D / 2;
    auto twidSub = twidTable->at((int)log2(dims));

    for (int x = 0; x < D / 2; x++) {
        E = twidSub->at(x);
        for (int y = 0; y < D * D; y+=D) {
            auto a = int_data[y + x];
            auto b = E * int_data[y + x + half];
            int_data[y + x] = a + b;
            int_data[y + x + half] = a - b;
        }
    }

    //transpose
    for (int y = 0; y < dims; y++) {
        for (int x = y + 1; x < dims; x++) {
            auto c = int_data[y * dims + x];
            int_data[y * dims + x] = int_data[x * dims + y];
            int_data[x * dims + y] = c;
        }
    }

    Filter::fftv2dl(int_data, dst, dims / 2, D, D, stride * 2);
    Filter::fftv2dl(int_data + stride, dst + D / 2, dims / 2, D, D, stride * 2);

    E = 1;

    for (int x = 0; x < D / 2; x++) {
        E = twidSub->at(x);
        for (int y = 0; y < D * D; y+=D) {
            auto a = dst[y + x];
            auto b = E * dst[y + x + half];
            dst[y + x] = a + b;
            dst[y + x + half] = a - b;
        }
    }

    delete[] int_data;
}

void Filter::fftv2dl(float* space, std::complex<float>* dst, int w, int h, int dw, int stride) {
    int W = abs(w);
    if (W < 2) {
        for (int y = 0; y < h * dw; y+=dw)
            dst[y] = space[y];
        return;
    }

    Filter::fftv2dl(space, dst, w / 2, h, dw, stride * 2);
    Filter::fftv2dl(space + stride, dst + W / 2, w / 2, h, dw, stride * 2);

    auto twidSub = twidTable->at((int)log2(W));
    std::complex<float> E(1, 0);
    int half = W / 2;

    for (int x = 0; x < half; x++) {
        E = twidSub->at(x);
        for (int y = 0; y < h * dw; y+=dw) {
            auto a = dst[y + x];
            auto b = E * dst[y + x + half];
            dst[y + x] = a + b;
            dst[y + x + half] = a - b;
        }
    }
}

void Filter::fftv2dl(std::complex<float>* space, std::complex<float>* dst, int w, int h, int dw, int stride) {
    int W = abs(w);
    if (W < 2) {
        for (int y = 0; y < h * dw; y+=dw)
            dst[y] = space[y];
        return;
    }

    Filter::fftv2dl(space, dst, w / 2, h, dw, stride * 2);
    Filter::fftv2dl(space + stride, dst + W / 2, w / 2, h, dw, stride * 2);

    auto twidSub = twidTable->at((int)log2(W));
    std::complex<float> E(1, 0);
    int half = W / 2;

    for (int x = 0; x < half; x++) {
        E = twidSub->at(x);
        for (int y = 0; y < h * dw; y+=dw) {
            auto a = dst[y + x];
            auto b = E * dst[y + x + half];
            dst[y + x] = a + b;
            dst[y + x + half] = a - b;
        }
    }
}

std::complex<float> Filter::dft(float* space, int N, int v) {
    std::complex<float> result(0, 0);
    std::complex<float> E(1, 0);
    auto V = std::exp(std::complex<float>(0, -2 * M_PI * v / N));

    for (int y = 0; y < N; y++) {
        result += space[y] * E;
        E *= V;
    }

    return result;
}

std::complex<float> Filter::dft(std::complex<float>* space, int N, int v) {
    std::complex<float> E(1, 0);
    std::complex<float> result(0, 0);
    auto V = std::exp(std::complex<float>(0, -2 * M_PI * v / N));

    for (int y = 0; y < N; y++) {
        result += space[y] * E;
        E *= V;
    }

    return result;
}

std::complex<float> Filter::idft(std::complex<float>* space, int N, int v) {
    std::complex<float> E(1, 0);
    std::complex<float> result(0, 0);
    auto V = std::exp(std::complex<float>(0, 2 * M_PI * v / N));

    for (int y = 0; y < N; y++) {
        result += space[y] * E;
        E *= V;
    }

    return (1.0f / N) * result;
}
