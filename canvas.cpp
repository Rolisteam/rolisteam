#include "canvas.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{

}
QImage Canvas::image() const
{
    return m_image;
}

void Canvas::setImage(const QImage &image)
{
    m_image = image;
}


