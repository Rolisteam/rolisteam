#ifndef OVERLAY_H
#define OVERLAY_H

#include "rwidgets_global.h"
#include <QRect>
#include <QWidget>

class RWIDGET_EXPORT Overlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QRect selectedRect READ selectedRect WRITE setSelectedRect NOTIFY selectedRectChanged)
    Q_PROPERTY(Ratio ratio READ ratio WRITE setRatio NOTIFY ratioChanged)
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

    enum class Ratio
    {
        Ratio_Unconstrained,
        Ratio_Square,
        Ratio_Tv,
        Ratio_Film
    };
    Q_ENUM(Ratio)
    Overlay(QWidget* parent= nullptr); // QRect rect, Ratio ratio= Ratio::Ratio_Unconstrained,

    // Overlay(QRect rect, Ratio ratio= Ratio::Ratio_Unconstrained, QWidget* parent= nullptr);

    Ratio ratio() const;
    void setRatio(Ratio ratio);

    QRect selectedRect() const;
    void setSelectedRect(const QRect& rect);

    void initRect();

signals:
    void selectedRectChanged(const QRect& rect);
    void ratioChanged();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QRect m_selectedRect{0, 0, 100, 100};
    Position m_currentCorner= None;
    QPoint m_lastPosition;
    Ratio m_ratio= Ratio::Ratio_Unconstrained;
    QRect m_centerRect;
};

#endif // OVERLAY_H
