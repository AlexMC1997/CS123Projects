#ifndef __BRUSH_H__    //header guards
#define __BRUSH_H__

#include <vector>

#include "RGBA.h"
#include <memory>


class Canvas2D;

/**
 * @class Brush
 *
 * The superclass for all brushes!
 */
class Brush {
public:
    // Constructor for brush.  Pass it the initial color, flow and radius.
    Brush(RGBA color, int radius, bool fixAlpha);

    // Destructor for brush.  Clean up all brush allocated resources.
    virtual ~Brush();

    RGBA getRGBA() const;
    int getAlpha() const;
    int getBlue() const;
    int getRed() const;
    int getGreen() const;
    int getRadius() const;

    void setAlpha(int alpha);
    void setBlue(int blue);
    void setGreen(int green);
    void setRed(int red);
    void setRGBA(const RGBA &rgba);
    void setRadius(int radius);

    virtual void brushDown(int x, int y, Canvas2D* canvas);
    virtual void brushDragged(int x, int y, Canvas2D* canvas);
    virtual void brushUp(int x, int y, Canvas2D* canvas) = 0;


protected:
    // Pure virtual function that will create the mask distribution.
    // To be be implemented in subclasses.
    virtual void makeMask() = 0;

    // C++ coding convention is to put m_ in front of member variables
    RGBA m_color;
    std::unique_ptr<std::vector<std::vector<float>*>> m_mask;
    int m_radius;
    std::unique_ptr<std::vector<RGBA>> m_density;
    bool m_fixAlpha;

};


#endif // __BRUSH_H__

