#ifndef FLATEDGE_H
#define FLATEDGE_H

#include "Brush.h"
#include <queue>

class FlatBrush : public Brush
{
public:
    FlatBrush(RGBA color, int radius, bool fixAlpha);
    virtual ~FlatBrush();

    virtual void brushUp(int x, int y, Canvas2D *canvas) {}
    virtual void brushDragged(int x, int y, Canvas2D *canvas);
protected:
    void makeMask();
    void makeMask(int x, int y);
private:
    int oldx;
    int oldy;
    std::queue<std::vector<int>> grad;
};

#endif // FLATEDGE_H
