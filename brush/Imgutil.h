#ifndef IMGUTIL_H
#define IMGUTIL_H

#include "qimage.h"
#include "RGBA.h"
#include "Kernel.h"

namespace ImgUtil
{
    void rotImg(QImage& img, float deg);
    void edgeDetect(QImage& img, float sens);
    void blurImg(QImage& img, int rad);
    void boxBlurImg(QImage& img, int rad);
    float gauss(float x, float stddev);
    float gauss2d(float x, float y, float stddev);
}
#endif // IMGUTIL_H
