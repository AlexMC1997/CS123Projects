#include "Imgutil.h"
#include "Filter.h"
#include <cmath>
#include <glm.hpp>
#include <iostream>


void ImgUtil::rotImg(QImage& img, float deg) {
    int w = img.width();
    int h = img.height();
    RGBA* data = reinterpret_cast<RGBA*>(img.bits());
    RGBA* ndata = new RGBA[w * h];
    float rad = deg * M_PI / 180;
    memset(ndata, 255, w * h * sizeof(RGBA));

    float dy = sin(rad);
    float dx = cos(rad);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float xn = x - (w / 2.0);
            float yn = y - (h / 2.0);
            float coordx = (xn * dx - yn * dy) + w/2.0;
            float coordy = (xn * dy + yn * dx) + h/2.0;
            if (coordy < 0 || coordx < 0 || ceil(coordx) >= w || ceil(coordy) >= h)
                continue;
            int y0 = floor(coordy);
            int y1 = y0 == coordy ? y0 + 1 : ceil(coordy);
            int x0 = floor(coordx);
            int x1 = x0 == coordx ? x0 + 1 : ceil(coordx);

            float rx1 = (coordx - x0) * data[y0 * w + x1].r + (x1 - coordx) * data[y0 * w + x0].r;
            float gx1 = (coordx - x0) * data[y0 * w + x1].g + (x1 - coordx) * data[y0 * w + x0].g;
            float bx1 = (coordx - x0) * data[y0 * w + x1].b + (x1 - coordx) * data[y0 * w + x0].b;

            float rx2 = (coordx - x0) * data[y1 * w + x1].r + (x1 - coordx) * data[y1 * w + x0].r;
            float gx2 = (coordx - x0) * data[y1 * w + x1].g + (x1 - coordx) * data[y1 * w + x0].g;
            float bx2 = (coordx - x0) * data[y1 * w + x1].b + (x1 - coordx) * data[y1 * w + x0].b;

            float ry = (coordy - y0) * rx2 + (y1 - coordy) * rx1;
            float gy = (coordy - y0) * gx2 + (y1 - coordy) * gx1;
            float by = (coordy - y0) * bx2 + (y1 - coordy) * bx1;

            unsigned char cr = round(glm::clamp<float>(ry, 0, 255));
            unsigned char cg = round(glm::clamp<float>(gy, 0, 255));
            unsigned char cb = round(glm::clamp<float>(by, 0, 255));

            ndata[y * w + x] = RGBA{cr, cg, cb, 255};
//            int ty = round(coordy);
//            int tx = round(coordx);
//            ndata[y * w + x] = RGBA{data[ty * w + tx].r, data[ty * w + tx].g, data[ty * w + tx].b, 255};
        }
    }

    memcpy(data, ndata, w * h * sizeof(RGBA));
    delete[] ndata;
}

void ImgUtil::edgeDetect(QImage& img, float sens) {
//    float arr[] = {
//        -1.0, 0.0, 1.0,
//        -2.0, 0.0, 2.0,
//        -1.0, 0.0, 1.0
//    };
//    float arr2[9] = {
//            -1.0, -2.0, -1.0,
//            0.0, 0.0, 0.0,
//            1.0, 2.0, 1.0
//    };
//    int w = img.width();
//    int h = img.height();
//    auto k = Kernel{3, arr};
//    auto k2 = Kernel{3, arr2};
//    auto gx = k * img;
//    auto gy = k2 * img;
//    RGBA* bits = reinterpret_cast<RGBA*>(img.bits());

//    for (int i = 0; i < w * h; i++) {
//        bits[i].r = glm::clamp<float>(sqrt(pow(gx->bits()[(i*4)], 2) + pow(gy->bits()[(i*4)], 2)), 0, 255);
//        bits[i].g = glm::clamp<float>(sqrt(pow(gx->bits()[(i*4)+1], 2) + pow(gy->bits()[(i*4)+1], 2)), 0, 255);
//        bits[i].b = glm::clamp<float>(sqrt(pow(gx->bits()[(i*4)+2], 2) + pow(gy->bits()[(i*4)+2], 2)), 0, 255);
//    }

    float arr[9] = {
        0.0, -1.0f + sens, 0.0,
        -1.0f + sens, 4.0f, -1.0f + sens,
        0.0, -1.0f + sens, 0.0
    };
    auto nimg = Kernel{3, arr} * img;
    QImage* nnimg = new QImage(nimg->width(), nimg->height(), QImage::Format_RGBX8888);
    Filter::intensity(nimg->width(), nimg->height(), reinterpret_cast<RGBA*>(nimg->bits()), reinterpret_cast<RGBA*>(nnimg->bits()));
    memcpy(img.bits(), nnimg->bits(), img.width() * img.height() * 4);
}

void ImgUtil::blurImg(QImage& img, int rad) {
    float arr[(2 * rad + 1)];
    for (int i = 0; i < 2 * rad + 1; i++) {
        arr[i] = gauss(i - rad, rad / 3.0);
    }
    auto nimg = Kernel{rad * 2 + 1, 1, arr} * img;
    auto nnimg = Kernel{1, rad * 2 + 1, arr} * *nimg;
    memcpy(img.bits(), nnimg->bits(), img.width() * img.height() * 4);
}

void ImgUtil::boxBlurImg(QImage& img, int rad) {
    float arr[(2 * rad + 1)];
    for (int i = 0; i < 2 * rad + 1; i++) {
        arr[i] = 1;
    }
    auto nimg = Kernel{rad * 2 + 1, 1, arr} * img;
    auto nnimg = Kernel{1, rad * 2 + 1, arr} * *nimg;
    memcpy(img.bits(), nnimg->bits(), img.width() * img.height() * 4);
}

float ImgUtil::gauss2d(float x, float y, float stddev) {
    float denom = pow(stddev, 2) * 2;
    float ep = exp(-(pow(x, 2) + pow(y, 2)) / denom);
    return ep / (denom * M_PI);
}

float ImgUtil::gauss(float x, float stddev) {
    float denom = pow(stddev, 2) * 2;
    float ep = exp(-(pow(x, 2)) / denom);
    return ep / sqrt(denom * M_PI);
}
