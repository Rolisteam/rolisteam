#ifndef OVERLAY_H
#define OVERLAY_H

#include <QRect>
#include <QWidget>

class Overlay : public QWidget
{
public:
    enum Position
    {
        None,
        First,
        Second,
        Third,
        Fourth,
        Center
    }; // follow clock,
    Overlay(QRect rect, QWidget* parent= nullptr);

    QRect rect() const;
    void setRect(const QRect& rect);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    int computeDistance(QPoint p, QPoint r);

private:
    QRect m_rect;
    QRect m_fullRect;
    Position m_currentCorner;
    QPoint m_lastPosition;
};

#endif // OVERLAY_H
