/**
 * @file   Brush.cpp
 *
 * Implementation of common functionality of bitmap brushes.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "Brush.h"
#include "Canvas2D.h"
#include <iostream>
#include <cmath>

static unsigned char lerp(unsigned char a, unsigned char b, float percent) {
    float fa = static_cast<float>(a) / 255.0f;
    float fb = static_cast<float>(b) / 255.0f;
    return static_cast<unsigned char>((fa + (fb - fa) * percent) * 255.0f + 0.5f);
}



Brush::Brush(RGBA color, int radius, bool fixAlpha) :
    // Pro-tip: Initialize all variables in the initialization list
    m_color(color),
    m_radius(radius),
    m_fixAlpha(fixAlpha)
{
    m_density = std::make_unique<std::vector<RGBA>>();
    m_mask = std::make_unique<std::vector<std::vector<float>*>>(m_radius * 2 + 1);
    for (int i = 0; i < m_radius * 2 + 1; i++) {
        m_mask->at(i)= new std::vector<float>(m_radius * 2 + 1);
    }
}


Brush::~Brush()
{
    // Pro-tip: The destructor (here) is where you free all the resources
    // you allocated during the lifetime of the class
    // Pro-tip: If you use std::unique_ptr or std::vector, you probabably
    // won't need to put anything in here.
}

int Brush::getAlpha() const {
    return m_color.a;
}

RGBA Brush::getRGBA() const {
    return m_color;
}

int Brush::getBlue() const {
    return m_color.b;
}

int Brush::getGreen() const {
    return m_color.g;
}

int Brush::getRadius() const {
    return m_radius;
}

int Brush::getRed() const {
    return m_color.r;
}

void Brush::setAlpha(int alpha) {
    m_color.a = alpha;
}

void Brush::setRGBA(const RGBA &rgba) {
    m_color = rgba;
}

void Brush::setBlue(int blue) {
    m_color.b = blue;
}

void Brush::setGreen(int green) {
    m_color.g = green;
}

void Brush::setRed(int red) {
    m_color.r = red;
}

void Brush::setRadius(int radius) {
    m_radius = radius;
    makeMask();
}

void Brush::brushDown(int x, int y, Canvas2D *canvas) {
    RGBA* data = canvas->data();
    int size = canvas->getImage()->width() * canvas->getImage()->height();
    m_density->resize(size);

    for (int i = 0; i < size; i++) {
        m_density->at(i) = RGBA{data[i].r, data[i].g, data[i].b, 0};
    }
}

void Brush::brushDragged(int mouseX, int mouseY, Canvas2D* canvas) {
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
}
