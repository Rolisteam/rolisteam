#ifndef REALSLIDER_H
#define REALSLIDER_H

#include <QSlider>

class RealSlider : public QSlider
{
    Q_OBJECT
public:
    explicit RealSlider(QWidget* parent= nullptr);

    qreal start() const;
    void setStart(const qreal& start);

    qreal end() const;
    void setEnd(const qreal& end);

    qreal step() const;
    void setStep(const qreal& step);

    void updateRange();
public slots:
    void setRealValue(qreal v);
private slots:
    void internalValueChanged(int);
signals:
    void valueChanged(qreal);

private:
    qreal m_start;
    qreal m_end;
    qreal m_step;
};

#endif // REALSLIDER_H
