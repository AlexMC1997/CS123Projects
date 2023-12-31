#include "SupportCanvas2D.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QString>

#include "RGBA.h"

SupportCanvas2D::SupportCanvas2D()
{
    m_draggingLeft = m_draggingRight = false;
    m_image = new QImage(1, 1, QImage::Format_RGBX8888);
}

SupportCanvas2D::~SupportCanvas2D()
{
    delete m_image;
}

void SupportCanvas2D::resize(int width, int height) {
    // clamp negative sizes so we always have at least one pixel
    if (width < 1) width = 1;
    if (height < 1) height = 1;

    delete m_image;
    m_image = new QImage(width, height, QImage::Format_RGBX8888 /* this corresponds to the RGBA struct */);

    // set the new image to white
    memset(m_image->bits(), 255, width * height * sizeof(RGBA));
    for (int i = sizeof(RGBA)-1; (unsigned long) i < width * height * sizeof(RGBA); i+=sizeof(RGBA)) {
        m_image->bits()[i] = 255;
    }

    // Reset the marquee when we change canvas size
    m_marqueeStart = m_marqueeStop;

    // resize and repaint the window (resizing the window doesn't always repaint it, like when you
    // set the same size twice)
    setFixedSize(width, height);
    update();
    notifySizeChanged(width, height);
}

void SupportCanvas2D::paintEvent(QPaintEvent *, QImage *image) {
    QPainter painter(this);
    if (image) {
        painter.drawImage(QPoint(0, 0), *image);
    } else {
        painter.drawImage(QPoint(0, 0), *m_image);
    }

    if (m_marqueeStart != m_marqueeStop) {
        int minX = qMin(m_marqueeStart.x(), m_marqueeStop.x()),
            minY = qMin(m_marqueeStart.y(), m_marqueeStop.y());
        int maxX = qMax(m_marqueeStart.x(), m_marqueeStop.x()),
            maxY = qMax(m_marqueeStart.y(), m_marqueeStop.y());

        // Darken the parts of the image that aren't selected.
        painter.fillRect(contentsRect(), QColor(0, 0, 0, 191));

        // Un-darken the parts of the image that are selected.
        if (minX < maxX && minY < maxY)
            painter.drawImage(minX, minY, *m_image, minX, minY, maxX - minX, maxY - minY);

        // Outline the selection in black.
        painter.setPen(QPen(QColor(0, 0, 0, 255)));
        painter.drawRect(minX, minY, maxX - minX, maxY - minY);

        // Outline the selection again with a dashed white pen, effectively giving the selection a
        // dashed black and white outline.
        QPen pen(QColor(255, 255, 255, 255));
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawRect(minX, minY, maxX - minX, maxY - minY);
    }
}

bool SupportCanvas2D::loadImage(const QString &file) {
    QImage *temp = new QImage();
    bool success = false;

    if (temp->load(file)) {
        // Make sure the image is RGB (not monochrome, for example).
        if (temp->format() != QImage::Format_RGBX8888) {
            QImage *old = temp;
            temp = new QImage(old->convertToFormat(QImage::Format_RGBX8888));
            delete old;
        }

        // Show the new image.
        resize(temp->width(), temp->height());
        memcpy(data(), temp->bits(), temp->sizeInBytes());
        update();

        // Remember the filename so we can revert to it
        m_lastfile = file;
        success = true;
    }

    delete temp;
    return success;
}

bool SupportCanvas2D::saveImage(const QString &file) {
    return m_image->save(file, nullptr, -1);
}

void SupportCanvas2D::newImage() {
    // Reset the marquee when we load a new picture.
    m_marqueeStart = m_marqueeStop;

    // Load a black 500x500 image.
    resize(512, 512);
}

bool SupportCanvas2D::revertImage() {
    if (!m_lastfile.isNull()) {
        if (loadImage(m_lastfile)) {
            // Reset the marquee when we load a new picture.
            m_marqueeStart = m_marqueeStop;
            return true;
        }
        QMessageBox::critical(this, "Error", "Could not load image \"" + m_lastfile + "\"");
    }
    return false;
}

void SupportCanvas2D::clearImage() {
    resize(width(), height());
}

bool SupportCanvas2D::saveImage() {
    QString file = QFileDialog::getSaveFileName(this);
    if (!file.isNull()) {
        // Qt needs an extension to decide what format to save in, so make sure it gets one.
        if (!file.contains('.'))
            file += ".png";

        if (saveImage(file))
            return true;
        QMessageBox::critical(this, "Error", "Could not save image \"" + file + "\"");
    }
    return false;
}

void SupportCanvas2D::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mouseDown(event->x(), event->y());
        m_draggingLeft = true;
        update();
    } else if (event->button() == Qt::RightButton) {
        m_marqueeStart = m_marqueeStop = event->pos();
        m_draggingRight = true;
        update();
    }
}

void SupportCanvas2D::mouseMoveEvent(QMouseEvent *event) {
    // m_draggingLeft and m_draggingRight are not mutually exclusive.
    if (m_draggingLeft) {
        mouseDragged(event->x(), event->y());
        update();
    }
    if (m_draggingRight) {
        m_marqueeStop = event->pos();
        update();
    }
}

void SupportCanvas2D::mouseReleaseEvent(QMouseEvent *event) {
    // Move the mouse again in case this event is at a different position then the last event.
    mouseMoveEvent(event);

    if (m_draggingLeft && event->button() == Qt::LeftButton) {
        mouseUp(event->x(), event->y());
        m_draggingLeft = false;
        update();
    } else if (m_draggingRight && event->button() == Qt::RightButton) {
        m_draggingRight = false;
        update();
    }
}

void SupportCanvas2D::settingsChanged() {
    // TODO: This will be called when the application settings have been changed.
}

