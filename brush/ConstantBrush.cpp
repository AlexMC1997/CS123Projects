/**
 * @file   ConstantBrush.cpp
 *
 * Implementation of a brush with a constant mask distribution.
 *
 * You should fill this file in while completing the Brush assignment.
 */

#include "ConstantBrush.h"
#include <iostream>

ConstantBrush::ConstantBrush(RGBA color, int radius, bool fixAlpha)
    : Brush(color, radius, fixAlpha)
{
    // @TODO: [BRUSH] You'll probably want to set up the mask right away.
    makeMask();
}


ConstantBrush::~ConstantBrush()
{
}

void ConstantBrush::makeMask() {
    int sq = m_radius * m_radius;
    int d = 0;
    for (int i = 0; i < m_radius * 2 + 1; i++) {
        auto& row = m_mask->at(i);
        for (int j = 0; j < m_radius * 2 + 1; j++) {
            d = ((j - m_radius) * (j - m_radius)) + ((i - m_radius) * (i - m_radius));
            row->at(j) = d <= sq ? 1.0 : 0;
        }
    }
}


