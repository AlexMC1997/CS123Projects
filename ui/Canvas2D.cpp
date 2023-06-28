/**
 * @file Canvas2D.cpp
 *
 * CS123 2-dimensional canvas. Contains support code necessary for Brush, Filter, Intersect, and
 * Ray.
 *
 * YOU WILL NEED TO FILL THIS IN!
 *
 */

// For your convenience, a few headers are included for you.
#include <assert.h>
#include <iostream>
#include <math.h>
#include <memory>
#include <unistd.h>
#include "Canvas2D.h"
#include "Settings.h"
#include "RayScene.h"

#include <QCoreApplication>
#include <QPainter>
#include "brush/Brush.h"
#include "brush/ConstantBrush.h"
#include "brush/LinearBrush.h"
#include "brush/QuadraticBrush.h"
#include "brush/SmudgeBrush.h"
#include "brush/FlatBrush.h"
#include "brush/Filter.h"
#include "brush/Kernel.h"
#include "brush/Imgutil.h"

Canvas2D::Canvas2D() :
    // @TODO: Initialize any pointers in this class here.
    m_rayScene(nullptr),
    m_saved(nullptr),
    m_brush(std::make_unique<ConstantBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending))
{
    Filter::genTable(4096);
}

Canvas2D::~Canvas2D()
{
}

// This is called when the canvas size is changed. You can change the canvas size by calling
// resize(...). You probably won't need to fill this in, but you can if you want to.
void Canvas2D::notifySizeChanged(int w, int h) {
}

void Canvas2D::paintEvent(QPaintEvent *e) {
    // You probably won't need to fill this in, but you can if you want to override any painting
    // events for the 2D canvas. For now, we simply call the superclass.
    SupportCanvas2D::paintEvent(e);

}

void Canvas2D::settingsChanged() {
    // TODO: Process changes to the application settings.
    std::cout << "Canvas2d::settingsChanged() called. Settings have changed" << std::endl;

    std::cout << width() << " " << height() << std::endl;

    switch (settings.brushType) {
        case BRUSH_CONSTANT:
            m_brush = std::make_unique<ConstantBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending);
            break;
        case BRUSH_LINEAR:
            m_brush = std::make_unique<LinearBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending);
            break;
        case BRUSH_QUADRATIC:
            m_brush = std::make_unique<QuadraticBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending);
            break;
        case BRUSH_SMUDGE:
            m_brush = std::make_unique<SmudgeBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending);
            break;
        case BRUSH_SPECIAL_1:
            m_brush = std::make_unique<FlatBrush>(settings.brushColor, settings.brushRadius, settings.fixAlphaBlending);
            break;
    }

    switch (settings.imageDomain) {
        case IMAGE_SPATIAL:
            if (m_saved) {
                delete m_image;
                m_image = m_saved;
                m_saved = nullptr;
            }
            break;
        case IMAGE_INTENSITY:
            if (m_saved) {
                delete m_image;
                m_image = m_saved;
            }
            m_saved = m_image;
            m_image = new QImage(m_saved->width(), m_saved->height(), QImage::Format_RGBX8888);
            Filter::intensity(m_saved->width(), m_saved->height(), reinterpret_cast<RGBA*>(m_saved->bits()), reinterpret_cast<RGBA*>(m_image->bits()));
            break;
        case IMAGE_FREQUENCY:
            if (m_saved) {
                delete m_image;
                m_image = m_saved;
            }
            m_saved = m_image;
            m_image = Filter::frequency(*m_saved);
            if (!m_image) {
                m_image = m_saved;
                m_saved = nullptr;
            }
            break;
    }

    setFixedSize(m_image->width(), m_image->height());
    update();
    notifySizeChanged(m_image->width(), m_image->height());
}

// ********************************************************************************************
// ** BRUSH
// ********************************************************************************************


void Canvas2D::mouseDown(int x, int y) {
    // @TODO: [BRUSH] Mouse interaction for brush. You will probably want to create a separate
    //        class for each of your brushes. Remember that you can use the static Settings
    //        object to get the currently selected brush and its parameters.

    // You're going to need to leave the alpha value on the canvas itself at 255, but you will
    // need to use the actual alpha value to compute the new color of the pixel

    std::cout << "Canvas2d::mouseDown() called" << std::endl;
    if (settings.imageDomain == IMAGE_SPATIAL) {
        m_brush->brushDown(x, y, this);
    }

}

void Canvas2D::mouseDragged(int x, int y) {
    // TODO: [BRUSH] Mouse interaction for Brush.
    std::cout << "Canvas2d::mouseDragged() called" << std::endl;

    if (settings.imageDomain == IMAGE_SPATIAL) {
        m_brush->brushDragged(x, y, this);
    }
}

void Canvas2D::mouseUp(int x, int y) {
    // TODO: [BRUSH] Mouse interaction for Brush.
    std::cout << "Canvas2d::mouseUp() called" << std::endl;
}



// ********************************************************************************************
// ** FILTER
// ********************************************************************************************
void Canvas2D::filterImage() {
    // TODO: [FILTER] Filter the image. Some example code to get the filter type is provided below.
    std::cout << "filtering" << std::endl;
    float k[9];
    switch(settings.filterType) {
        case FILTER_BLUR:
            ImgUtil::blurImg(*m_image, settings.blurRadius);
            break;
        case FILTER_EDGE_DETECT:
            ImgUtil::edgeDetect(*m_image, settings.edgeDetectSensitivity);
            break;
        case FILTER_ROTATE:
            ImgUtil::rotImg(*m_image, settings.rotateAngle);
            break;
    }

    // Leave this code here! This code ensures that the Canvas2D will be completely wiped before
    // drawing the new image.
    repaint(rect());
    QCoreApplication::processEvents();
}

// ********************************************************************************************
// ** RAY
// ********************************************************************************************

void Canvas2D::setScene(RayScene *scene) {
    m_rayScene.reset(scene);
}

void Canvas2D::renderImage(Camera *camera, int width, int height) {
    if (m_rayScene) {
        // @TODO: raytrace the scene based on settings
        //        YOU MUST FILL THIS IN FOR INTERSECT/RAY

        // If you want the interface to stay responsive, make sure to call
        // QCoreApplication::processEvents() periodically during the rendering.

    }
}

void Canvas2D::cancelRender() {
    // TODO: cancel the raytracer (optional)
}




