#include "Kernel.h"
#include "qimage.h"
#include "RGBA.h"
#include <algorithm>

Kernel::Kernel(int n) :
    m_w{n},
    m_h{n},
    m_data(n*n)
{
}

Kernel::Kernel(int w, int h) :
    m_w{w},
    m_h{h},
    m_data(w*h)
{
}

Kernel::Kernel(int n, float arr[]) : Kernel(n)
{
    for (int i = 0; i < n * n; i++) {
        m_data[i] = arr[i];
    }
}

Kernel::Kernel(int w, int h, float arr[]) : Kernel(w, h)
{
    for (int i = 0; i < w * h; i++) {
        m_data[i] = arr[i];
    }
}

Kernel::Kernel(glm::mat3 mat) : Kernel(3)
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m_data[i * 3 + j] = mat[i][j];
        }
    }
}

int Kernel::width() const {
    return m_w;
}

int Kernel::height() const {
    return m_h;
}

std::vector<float>* Kernel::data() {
    return &m_data;
}

float Kernel::at(int i) const {
    if (i >= m_w * m_h)
        return 0;
    return m_data[i];
}

float Kernel::at(int x, int y) const {
    if (x >= m_w || y >= m_h)
        return 0;
    return m_data[y * m_h + x];
}

Kernel operator*(const Kernel&& p, const Kernel&& q) {
    int n = p.height() * p.width();
    if (q.height() != p.width() || q.width() != p.width()) {
        return Kernel(0, nullptr);
    }

    auto k = Kernel{p.width(), p.height()};
    auto v = k.data();

    for (int i = 0; i < n * n; i++) {
        v->at(i) = q.at(i) * p.at(i);
    }

    return k;
}

QImage* Kernel::operator*(const QImage& q) {
    int w = q.width();
    int h = q.height();
    int half_w = m_w / 2;
    int half_h = m_h / 2;
    float R = 0;
    float G = 0;
    float B = 0;
    float weight = 0;
    for (int i = 0; i < m_w * m_h; i++) {
        if (abs(this->at(i)) > 0.002)
            weight += this->at(i);
    }

    weight = weight == 0 ? 1 : weight;
    auto qn = new QImage{w, h, QImage::Format_RGBX8888};
    auto orig = q.bits();
    auto data = qn->bits();

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            R = 0, G = 0, B = 0;
            int yn = 0;
            int xn = 0;
            for (int k = 0; k < m_h; k++) {
                yn = i - (half_h - k);
                if (yn < 0) {
                    yn = 0;
                } else if (yn >= h) {
                    yn = h - 1;
                }
                for (int t = 0; t < m_w; t++) {
                    float mod = this->at(k * m_w + t);
                    if (abs(mod) <= 0.002)
                        continue;
                    xn = j - (half_w - t);
                    if (xn < 0 || xn >= w) {
                        xn = 0;
                    } else if (xn >= w) {
                        xn = w - 1;
                    }
                    if (mod == 1) {
                        R += orig[yn * 4 * w + xn * 4];
                        G += orig[yn * 4 * w + xn * 4 + 1];
                        B += orig[yn * 4 * w + xn * 4 + 2];
                    } else {
                        R += orig[yn * 4 * w + xn * 4] * mod;
                        G += orig[yn * 4 * w + xn * 4 + 1] * mod;
                        B += orig[yn * 4 * w + xn * 4 + 2] * mod;
                    }
                }
            }
            data[i * 4 * w + j * 4] = glm::clamp<float>(abs(round(R / weight)), 0, 255);
            data[i * 4 * w + j * 4 + 1] = glm::clamp<float>(abs(round(G / weight)), 0, 255);
            data[i * 4 * w + j * 4 + 2] = glm::clamp<float>(abs(round(B / weight)), 0, 255);
            data[i * 4 * w + j * 4 + 3] = orig[i * 4 * w + j * 4 + 3];
        }
    }

    return std::move(qn);
}
