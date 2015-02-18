#include "colorbutton.h"
#include <QColorDialog>
ColorButton::ColorButton(QWidget * parent)
    : QPushButton(parent)
{
    connect(this,SIGNAL(clicked()),this,SLOT(changeColor()));
}
void ColorButton::setColor(QColor color)
{
    m_color=color;
    setPalette(QPalette(color));
}
const QColor& ColorButton::color() const
{
    return m_color;
}
void ColorButton::changeColor()
{
        QColor color = QColorDialog::getColor(m_color);
        if (color.isValid())
        {
            setPalette(QPalette(color));
            m_color = color;
            emit colorChanged(m_color);
        }
}
