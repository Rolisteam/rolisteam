#include "field.h"
#include <QPainter>
#include <QMouseEvent>


Field::Field(QWidget *parent)
    : QWidget(parent)
{
    m_label = new QLabel(this);
    m_size = QSize(64,30);
    m_label->resize(m_size);
    m_label->setScaledContents(true);
    m_bgColor = QColor(Qt::white);
    m_textColor = QColor(Qt::black);
    m_font = font();
    m_key = "key";
}

void Field::writeQML()
{

}

void Field::drawField()
{
    QImage imgField(m_size, QImage::Format_ARGB32_Premultiplied);
    imgField.fill(m_bgColor);

    QPainter painterImg(&imgField);
    painterImg.setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(m_textColor);

    painterImg.setFont(m_font);
    painterImg.setPen(pen);

    QRectF rect(QPointF(0,0),QSizeF(m_size));


    painterImg.drawText(rect,Qt::AlignCenter,m_key);


    m_label->setPixmap(QPixmap::fromImage(imgField));
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
QPoint Field::position() const
{
    return m_pos;
}

void Field::setPosition(const QPoint &pos)
{
    m_pos = pos;
    drawField();
}
QSize Field::size() const
{
    return m_size;
}

void Field::setSize(const QSize &size)
{
    m_size = size;
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
        emit clickOn(this);
    }
}

void Field::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
       m_pos = ev->pos();
       move(m_pos);
    }
}







