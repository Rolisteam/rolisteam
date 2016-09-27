#include "canvasfield.h"

#include <QPainter>

#include <QDebug>
#include "field.h"

QHash<int,QString> CanvasField::m_pictureMap({{Field::TEXTINPUT, ":/resources/icons/Actions-edit-rename-icon.png"},
                                              {Field::TEXTAREA, ":/resources/icons/textarea.png"},
                                              {Field::TEXTFIELD, ":/resources/icons/Actions-edit-rename-icon.png"},
                                              {Field::SELECT,""},
                                              {Field::CHECKBOX,":/resources/icons/checked_checkbox.png"},
                                              {Field::IMAGE,":/resources/icons/photo.png"},
                                              {Field::BUTTON,""}});

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

    if(m_pix.isNull()||m_currentType!=m_field->getCurrentType())
    {
        m_pix=QPixmap(m_pictureMap[m_field->getCurrentType()]).scaled(32,32);
        m_currentType = m_field->getCurrentType();
    }
    if(!m_pix.isNull())
    {
        painter->drawPixmap(m_rect.center()-m_pix.rect().center(),m_pix,m_pix.rect());
    }

    painter->drawText(m_rect,flags,m_field->getId());
    painter->restore();
}
void CanvasField::setWidth(qreal w)
{
    if(w!=m_rect.width())
    {
        m_rect.setWidth(w);
        emit widthChanged();
        update();
    }
}

void CanvasField::setHeight(qreal h)
{
    if(h!=m_rect.height())
    {
        m_rect.setHeight(h);
        emit heightChanged();
        update();
    }
}
