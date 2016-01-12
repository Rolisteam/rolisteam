#include "field.h"
#include <QPainter>
#include <QMouseEvent>

int Field::m_count = 0;
Field::Field(QPointF topleft,QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    ++m_count;
    m_rect.setTopLeft(topleft);
    m_rect.setBottomRight(topleft);

    m_bgColor = QColor(Qt::white);
    m_textColor = QColor(Qt::black);
    m_font = font();
    m_key = QStringLiteral("key %1").arg(m_count);
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
}

void Field::writeQML()
{

}
QRectF Field::boundingRect() const
{
    return m_rect;
}
void Field::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();


    painter->drawRect(m_rect);
    painter->drawText(m_rect,m_key);

    painter->restore();


}
void Field::setNewEnd(QPointF nend)
{
    m_rect.setBottomRight(nend);
}

void Field::drawField()
{
  /*  QImage imgField(m_size, QImage::Format_ARGB32_Premultiplied);
    imgField.fill(m_bgColor);

    QPainter painterImg(&imgField);
    painterImg.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(m_textColor);

    painterImg.setFont(m_font);
    painterImg.setPen(pen);

    QRectF rect(QPointF(0,0),QSizeF(m_size));


    painterImg.drawText(rect,Qt::AlignCenter,m_key);


    m_label->setPixmap(QPixmap::fromImage(imgField));*/
}
QString Field::key() const
{
    return m_key;
}

void Field::setKey(const QString &key)
{
    m_key = key;
    drawField();
}

Field::BorderLine Field::border() const
{
    return m_border;
}

void Field::setBorder(const Field::BorderLine &border)
{
    m_border = border;
    drawField();
}
QColor Field::bgColor() const
{
    return m_bgColor;
}

void Field::setBgColor(const QColor &bgColor)
{
    m_bgColor = bgColor;
    drawField();
}
QColor Field::textColor() const
{
    return m_textColor;
}

void Field::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
    drawField();
}
QFont Field::font() const
{
    return m_font;
}

void Field::setFont(const QFont &font)
{
    m_font = font;
    drawField();
}

void Field::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton)
    {
       // emit clickOn(this);
    }
}








