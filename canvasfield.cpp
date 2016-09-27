#include "canvasfield.h"

#include <QPainter>


#include "field.h"


CanvasField::CanvasField(Field* field)
    : m_field(field)
{
    m_rect.setCoords(0,0,0,0);
}
void CanvasField::setNewEnd(QPointF nend)
{
    m_rect.setBottomRight(nend);
}
Field* CanvasField::getField() const
{
    return m_field;
}

void CanvasField::setField(Field* field)
{
    m_field = field;
}
QRectF CanvasField::boundingRect() const
{
    return m_rect;
}
QPainterPath CanvasField::shape() const
{
    QPainterPath path;
    path.moveTo(0,0);
    path.lineTo(m_rect.width(),0);
    path.lineTo(m_rect.width(),m_rect.height());
    path.lineTo(0,m_rect.height());
    path.closeSubpath();
    return path;
}
void CanvasField::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if(NULL==m_field)
        return;
    painter->save();
    painter->fillRect(m_rect,m_field->bgColor());
    painter->setPen(Qt::black);
    painter->drawRect(m_rect);
    int flags =0;
    Field::TextAlign align = m_field->getTextAlignValue();
    if(align <3)
    {
        flags = Qt::AlignTop;
    }
    else if(align <6)
    {
        flags = Qt::AlignVCenter;
    }
    else
    {
        flags = Qt::AlignBottom;
    }

    if(align%3==0)
    {
        flags |= Qt::AlignRight;
    }
    else if(align%3==1)
    {
        flags |= Qt::AlignHCenter;
    }
    else
    {
        flags |= Qt::AlignLeft;
    }


    painter->drawText(m_rect,flags,m_field->getId());
    painter->restore();
}
