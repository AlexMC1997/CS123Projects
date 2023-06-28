#include "FlatBrush.h"

#include "Canvas2D.h"
#include <math.h>
#include <iostream>

FlatBrush::FlatBrush(RGBA color, int radius, bool fixAlpha)
    : Brush(color, radius, fixAlpha),
      oldx(100),
      oldy(100),
      grad()
{
    for (int i = 0; i < 10; i++)
        grad.push(std::vector<int>{0, 0});
    makeMask(150, 150);
}


FlatBrush::~FlatBrush()
{
}

void FlatBrush::makeMask() {
    for (int i = 0; i < m_radius * 2 + 1; i++) {
        auto& row = m_mask->at(i);
        for (int j = 0; j < m_radius * 2 + 1; j++) {
            row->at(j) = 1.0;
        }
    }
}

void FlatBrush::makeMask(int x, int y) {
    int sq = m_radius * m_radius;
    int d = 0;
    if (x == oldx && y == oldy)
        return;
    float mag = 1.0 / sqrt(((oldy - y) * (oldy - y)) + ((x - oldx) * (x - oldx)));
    for (int i = 0; i < m_radius * 2 + 1; i++) {
        auto& row = m_mask->at(i);
        for (int j = 0; j < m_radius * 2 + 1; j++) {
            d = ((j - m_radius) * (j - m_radius)) + ((i - m_radius) * (i - m_radius));
            row->at(j) = d < sq ? mag * ((oldy - y) * (i - m_radius) - (x - oldx) * (j - m_radius)) / m_radius : 0;
            row->at(j) = row->at(j) < 0 ? 0 : row->at(j);
//            printf("%2.2f ", row->at(j));
        }
//        std::cout << std::endl;
    }
}

void FlatBrush::brushDragged(int mouseX, int mouseY, Canvas2D *canvas) {
    int width = canvas->getImage()->width();
    int height = canvas->getImage()->height();
    int posX = 0;
    int posY = 0;
    std::vector<float>* row;
    RGBA* map = canvas->data();

    for (int i = -m_radius; i <= m_radius; i++) {
        posY = mouseY + i;
        if (posY < 0 || posY > height - 1)
            continue;
        row = m_mask->at(i + m_radius);
        for (int j = -m_radius; j <= m_radius; j++) {
            posX = mouseX + j;

            if (posX < 0 || posX > width - 1)
                continue;

            float mask_scale = row->at(j + m_radius) * (m_color.a / 255.0);
            float dense_scale = 1.0 - mask_scale;
            RGBA canv_col = map[posY * width + posX];

            if (m_fixAlpha) {
                canv_col = m_density->at(posY * width + posX);

                if (canv_col.a >= 255 * mask_scale)
                    continue;

                m_density->at(posY * width + posX).a = 255 * mask_scale;
            }
            map[posY * width + posX].r = round(canv_col.r * dense_scale + m_color.r * mask_scale);
            map[posY * width + posX].g = round(canv_col.g * dense_scale + m_color.g * mask_scale);
            map[posY * width + posX].b = round(canv_col.b * dense_scale + m_color.b * mask_scale);
            map[posY * width + posX].a = 255;
        }
    }
    canvas->update();
    makeMask(mouseX, mouseY);
    grad.push(std::vector<int>{mouseX, mouseY});
    auto vec = grad.front();
    grad.pop();
    oldx = vec[0];
    oldy = vec[1];
}
