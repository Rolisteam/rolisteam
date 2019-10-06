#include <QDebug>

#include "realslider.h"

RealSlider::RealSlider(QWidget* parent) : QSlider(parent), m_start(0.0), m_end(1.0), m_step(0.01)
{
    updateRange();
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(internalValueChanged(int)));
}

qreal RealSlider::start() const
{
    return m_start;
}

void RealSlider::setStart(const qreal& start)
{
    m_start= start;
    updateRange();
}

qreal RealSlider::end() const
{
    return m_end;
}

void RealSlider::setEnd(const qreal& end)
{
    m_end= end;
    updateRange();
}

qreal RealSlider::step() const
{
    return m_step;
}

void RealSlider::setStep(const qreal& step)
{
    m_step= step;
    updateRange();
}

void RealSlider::updateRange()
{
    qreal val= m_end - m_start;
    qreal num= val / m_step;

    setMaximum(num);
    setMinimum(0);
    setSingleStep(1);
}

void RealSlider::setRealValue(qreal v)
{
    setValue(v * maximum());
}

void RealSlider::internalValueChanged(int x)
{
    if(0 != maximum())
    {
        qreal y= static_cast<qreal>(x) / static_cast<qreal>(maximum());
        emit valueChanged(y);
    }
}
