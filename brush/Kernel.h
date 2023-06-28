#ifndef KERNEL_H
#define KERNEL_H

#include "glm.hpp"
#include "qimage.h"
#include <memory>
#include <vector>

class Kernel
{
public:
    Kernel(int n);
    Kernel(int w, int h);
    Kernel(int n, float arr[]);
    Kernel(int w, int h, float arr[]);
    Kernel(glm::mat3 mat);
    int width() const;
    int height() const;
    std::vector<float>* data();
    float at(int i ) const;
    float at(int x, int y) const;
    QImage* operator*(const QImage& q);
private:
    int m_w;
    int m_h;
    std::vector<float> m_data;
};

#endif // KERNEL_H
