#include "overlay.h"

#include <QPainter>
#include <QPen>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDebug>

#define DISTANCE_SELECTION 20
#define GREY_COLOR_LEVEL 60


Overlay::Overlay(QRect rect, QWidget * parent)
: QWidget(parent),m_rect(rect),m_fullRect(rect)
{
    auto y = m_rect.y();
    m_rect.setY(0);
    m_rect.setHeight(m_rect.height()-y);

    setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_TransparentForMouseEvents);
}
void Overlay::paintEvent(QPaintEvent *)
{
        QPainter painter(this);

        QPainterPath toShow;
        toShow.addRect(m_fullRect);

        QPainterPath toHide;
        toHide.addRect(m_rect);
        toShow=toShow.subtracted(toHide);

        painter.fillPath(toShow,QColor(GREY_COLOR_LEVEL,GREY_COLOR_LEVEL,GREY_COLOR_LEVEL,100));
        auto pen = painter.pen();
        pen.setWidth(4);
        pen.setStyle(Qt::DashDotLine);
        pen.setColor(Qt::red);
        painter.setPen(pen);

        painter.drawRect(m_rect);
        auto offset = DISTANCE_SELECTION/2;
        painter.fillRect(m_rect.topLeft().x()-offset,m_rect.topLeft().y()-offset,DISTANCE_SELECTION,DISTANCE_SELECTION,Qt::red);
        painter.fillRect(m_rect.topRight().x()-offset,m_rect.topRight().y()-offset,DISTANCE_SELECTION,DISTANCE_SELECTION,Qt::red);
        painter.fillRect(m_rect.bottomLeft().x()-offset,m_rect.bottomLeft().y()-offset,DISTANCE_SELECTION,DISTANCE_SELECTION,Qt::red);
        painter.fillRect(m_rect.bottomRight().x()-offset,m_rect.bottomRight().y()-offset,DISTANCE_SELECTION,DISTANCE_SELECTION,Qt::red);
}

QRect Overlay::rect() const
{
    return m_rect;
}

void Overlay::setRect(const QRect &rect)
{
    m_rect = rect;
    update();
}
int Overlay::computeDistance(QPoint p, QPoint r)
{
  auto result = p-r;
  return result.manhattanLength();
}
void Overlay::mousePressEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    if(computeDistance(pos,m_rect.topLeft())<DISTANCE_SELECTION)
    {
      m_currentCorner = First;
    }
    else if(computeDistance(pos,m_rect.topRight())<DISTANCE_SELECTION)
    {
      m_currentCorner = Second;
    }
    else if(computeDistance(pos,m_rect.bottomRight())<DISTANCE_SELECTION)
    {
      m_currentCorner = Third;
    }
    else if(computeDistance(pos,m_rect.bottomLeft())<DISTANCE_SELECTION)
    {
      m_currentCorner = Fourth;
    }
    else if(computeDistance(pos,m_rect.center())<DISTANCE_SELECTION)
    {
      m_currentCorner = Center;
    }
    else
    {
      m_currentCorner = None;
    }

    if(m_currentCorner == None)
      QWidget::mousePressEvent(event);
    else
      m_lastPosition = pos;
}

void Overlay::mouseMoveEvent(QMouseEvent* event)
{
  auto pos = event->pos();
  auto deplace = pos-m_lastPosition;

    switch(m_currentCorner)
    {
      case First:
      m_rect.setTopLeft(pos);
      break;
    case Second:
      m_rect.setTopRight(pos);
      break;
    case Third:
      m_rect.setBottomRight(pos);
      break;
    case Fourth:
      m_rect.setBottomLeft(pos);
      break;
    case Center:
      m_rect.translate(deplace);
      break;
    default:
      break;
    }
    m_lastPosition = event->pos();
    update();
}
