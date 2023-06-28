#ifndef LinearBrush_Header
#define LinearBrush_Header


#include "Brush.h"

/**
 * @class LinearBrush
 *
 * Linear brush, whose mask has linear falloff.
 */
class LinearBrush
    : public Brush
{

public:
    LinearBrush(RGBA color, int radius, bool fixAlpha);
    virtual ~LinearBrush();

    virtual void brushUp(int x, int y, Canvas2D *canvas) {}

protected:
    void makeMask(); // Constructs the mask for this brush.
};

#endif
