/**
 * @file   Brush.cpp
 *
 * Implementation of a brush that smudges the image on the canvas as the mouse is dragged.
 *
 * You should fill this file in for the Brush assignment.
 */

#include "SmudgeBrush.h"

#include "Canvas2D.h"
#include <cmath>
#include <iostream>

SmudgeBrush::SmudgeBrush(RGBA color, int radius, bool fixAlpha) :
    QuadraticBrush(color, radius, fixAlpha)
{
    m_paint = std::make_unique<std::vector<RGBA>>(pow(m_radius * 2 + 1, 2));
    makeMask();
}


SmudgeBrush::~SmudgeBrush()
{
    // @TODO: [BRUSH] Be sure not to leak memory!  If you're using automatic memory management everywhere,
    //       this will be empty.
}

void SmudgeBrush::brushUp(int x, int y, Canvas2D* canvas) {
}


//! create a mask with a distribution of your choice (probably want to use quadratic for best results)
void SmudgeBrush::makeMask() {
    int sq = m_radius * m_radius;
    int d = 0;
    for (int i = 0; i < m_radius * 2 + 1; i++) {
        m_mask->at(i)= new std::vector<float>(m_radius * 2 + 1);
        auto& row = m_mask->at(i);
        for (int j = 0; j < m_radius * 2 + 1; j++) {
            d = ((j - m_radius) * (j - m_radius)) + ((i - m_radius) * (i - m_radius));
            row->at(j) = d < sq ? 1.0 - sqrt(static_cast<float>(d)) / m_radius : 0;
        }
    }
}

void SmudgeBrush::brushDown(int x, int y, Canvas2D *canvas) {
    pickUpPaint(x, y, canvas);
}

//! Picks up paint from the canvas before drawing begins.
void SmudgeBrush::pickUpPaint(int x, int y, Canvas2D* canvas) {
    RGBA* data = canvas->data();
    int width = canvas->width();
    int height = canvas->height();

    for (int i = 0; i < m_radius * 2 + 1; i++) {
        if (y - m_radius + i >= height || y - m_radius + i < 0)
            continue;
        for (int j = 0; j < m_radius * 2 + 1; j++) {
            if (x - m_radius + j >= width || x - m_radius + j < 0)
                continue;
            m_paint->at(i * (2 * m_radius + 1) + j) = data[(y - m_radius + i) * width + (x - m_radius + j)];
        }
    }
}

void SmudgeBrush::brushDragged(int mouseX, int mouseY, Canvas2D* canvas) {
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

            float mask_scale = row->at(j + m_radius) * (m_color.a / 256.0);
            float dense_scale = 1.0 - mask_scale;
            RGBA paint_col = m_paint->at((i + m_radius) * (2 * m_radius + 1) + j + m_radius);
            RGBA canv_col = map[posY * width + posX];
            map[posY * width + posX].r = round(canv_col.r * dense_scale + paint_col.r * mask_scale);
            map[posY * width + posX].g = round(canv_col.g * dense_scale + paint_col.g * mask_scale);
            map[posY * width + posX].b = round(canv_col.b * dense_scale + paint_col.b * mask_scale);
            map[posY * width + posX].a = 255;
        }
    }
    canvas->update();
    // now pick up paint again...
    pickUpPaint(mouseX, mouseY, canvas);
}


