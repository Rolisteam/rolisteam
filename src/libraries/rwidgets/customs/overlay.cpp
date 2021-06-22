#include "overlay.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

constexpr int k_distance_selection= 10;
constexpr int k_grey_color_level= 60;
constexpr int k_pen_width= 4;
constexpr int k_opacity= 200;

#include "core/worker/utilshelper.h"

int computeDistance(QPoint p, QPoint r)
{
    auto result= p - r;
    return result.manhattanLength();
}

qreal ratioFromEnum(Overlay::Ratio ratio)
{
    static QHash<Overlay::Ratio, qreal> hash(
        {{Overlay::Ratio::Ratio_Square, 1}, {Overlay::Ratio::Ratio_Tv, 4. / 3.}, {Overlay::Ratio::Ratio_Tv, 16. / 9.}});
    return hash.value(ratio);
}

inline uint qHash(const Overlay::Ratio& key, uint seed)
{
    return qHash(static_cast<int>(key), seed) ^ static_cast<int>(key);
}

QRect computeRectGivenRatio(const QRect& rect, Overlay::Ratio ratio)
{
    if(ratio == Overlay::Ratio::Ratio_Unconstrained)
        return rect;

    return helper::utils::computerBiggerRectInside(rect, ratioFromEnum(ratio)).toRect();
}

Overlay::Overlay(QWidget* parent) // QRect rect, Ratio ratio,, m_rect(rect), m_fullRect(rect), m_ratio(ratio)
    : QWidget(parent), m_currentCorner(None), m_lastPosition()
{

    setAttribute(Qt::WA_NoSystemBackground);
}

Overlay::Ratio Overlay::ratio() const
{
    return m_ratio;
}

void Overlay::initRect()
{
    auto y= m_selectedRect.y();
    m_selectedRect.setY(0);
    m_selectedRect.setHeight(m_selectedRect.height() - y);

    m_selectedRect= computeRectGivenRatio(m_selectedRect, m_ratio);
}

void Overlay::setRatio(Ratio ratio)
{
    if(ratio == m_ratio)
        return;
    m_ratio= ratio;
    emit ratioChanged();
}

void Overlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QPainterPath toShow;
    toShow.addRect(rect());

    QPainterPath toHide;
    toHide.addRect(m_selectedRect);
    toShow= toShow.subtracted(toHide);

    // qDebug() << "paint event" << toShow << toHide << rect() << m_selectedRect;

    painter.fillPath(toShow, QColor(k_grey_color_level, k_grey_color_level, k_grey_color_level, k_opacity));
    painter.save();
    auto pen= painter.pen();
    pen.setWidth(k_pen_width);
    pen.setStyle(Qt::DashDotLine);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawRect(m_selectedRect);
    painter.restore();

    auto offset= k_distance_selection / 2;
    painter.fillRect(m_selectedRect.topLeft().x() - offset, m_selectedRect.topLeft().y() - offset, k_distance_selection,
                     k_distance_selection, Qt::red);
    painter.fillRect(m_selectedRect.topRight().x() - offset, m_selectedRect.topRight().y() - offset,
                     k_distance_selection, k_distance_selection, Qt::red);
    painter.fillRect(m_selectedRect.bottomLeft().x() - offset, m_selectedRect.bottomLeft().y() - offset,
                     k_distance_selection, k_distance_selection, Qt::red);
    painter.fillRect(m_selectedRect.bottomRight().x() - offset, m_selectedRect.bottomRight().y() - offset,
                     k_distance_selection, k_distance_selection, Qt::red);
    painter.fillRect(m_selectedRect.center().x() - offset, m_selectedRect.center().y() - offset, k_distance_selection,
                     k_distance_selection, Qt::red);
}

QRect Overlay::selectedRect() const
{
    return m_selectedRect;
}

void Overlay::setSelectedRect(const QRect& rect)
{
    if(rect == m_selectedRect)
        return;
    m_selectedRect= computeRectGivenRatio(rect, m_ratio);
    emit selectedRectChanged(m_selectedRect);
    update();
}

void Overlay::mousePressEvent(QMouseEvent* event)
{
    auto pos= event->pos();
    if(computeDistance(pos, m_selectedRect.topLeft()) < k_distance_selection)
    {
        m_currentCorner= First;
    }
    else if(computeDistance(pos, m_selectedRect.topRight()) < k_distance_selection)
    {
        m_currentCorner= Second;
    }
    else if(computeDistance(pos, m_selectedRect.bottomRight()) < k_distance_selection)
    {
        m_currentCorner= Third;
    }
    else if(computeDistance(pos, m_selectedRect.bottomLeft()) < k_distance_selection)
    {
        m_currentCorner= Fourth;
    }
    else if(computeDistance(pos, m_selectedRect.center()) < k_distance_selection)
    {
        m_currentCorner= Center;
    }
    else
    {
        m_currentCorner= None;
    }

    if(m_currentCorner == None)
        QWidget::mousePressEvent(event);
    else
        m_lastPosition= pos;
}

void Overlay::mouseMoveEvent(QMouseEvent* event)
{
    auto pos= event->pos();
    auto deplace= pos - m_lastPosition;
    auto oldRect= m_selectedRect;

    switch(m_currentCorner)
    {
    case First:
        m_selectedRect.setTopLeft(pos);
        break;
    case Second:
        m_selectedRect.setTopRight(pos);
        break;
    case Third:
        m_selectedRect.setBottomRight(pos);
        break;
    case Fourth:
        m_selectedRect.setBottomLeft(pos);
        break;
    case Center:
        m_selectedRect.translate(deplace);
        break;
    default:
        break;
    }
    m_selectedRect= computeRectGivenRatio(m_selectedRect, m_ratio);

    if(!rect().contains(m_selectedRect))
        m_selectedRect= oldRect;

    m_lastPosition= event->pos();
    update();
    emit selectedRectChanged(m_selectedRect);
}
