#include "dicecontroller.h"

#include "dicehelper.h"

DiceController::DiceController(DiceController::DiceType type, QObject* parent)
    : QObject{parent}, m_face(type), m_timer(new QTimer)
{
    m_timer->setInterval(1000);
    auto updateValue= [this]()
    {
        if(!stable())
            return;
        using dd= DiceController::DiceType;
        if(m_face == dd::FOURSIDE)
            setValue(DiceHelper::computeValue<dd::FOURSIDE>(m_rotation));
        else if(m_face == dd::SIXSIDE)
            setValue(DiceHelper::computeValue<dd::SIXSIDE>(m_rotation));
        else if(m_face == dd::OCTOSIDE)
            setValue(DiceHelper::computeValue<dd::OCTOSIDE>(m_rotation));
        else if(m_face == dd::TENSIDE)
            setValue(DiceHelper::computeValue<dd::TENSIDE>(m_rotation));
        else if(m_face == dd::TWELVESIDE)
            setValue(DiceHelper::computeValue<dd::TWELVESIDE>(m_rotation));
        else if(m_face == dd::TWENTYSIDE)
            setValue(DiceHelper::computeValue<dd::TWENTYSIDE>(m_rotation));
        else if(m_face == dd::ONEHUNDREDSIDE)
            setValue(DiceHelper::computeValue<dd::ONEHUNDREDSIDE>(m_rotation));
    };
    connect(this, &DiceController::rotationChanged, this, updateValue);
    connect(this, &DiceController::stableChanged, this, updateValue);

    connect(m_timer.get(), &QTimer::timeout, this,
            [this]()
            {
                auto t= m_count.elapsed();
                if(t >= 900)
                {
                    setStable(true);
                    m_timer->stop();
                }
                else
                {
                    m_timer->start();
                }
            });

    updateValue();
}

QColor DiceController::color() const
{
    return m_color;
}

void DiceController::setColor(const QColor& newColor)
{
    if(m_color == newColor)
        return;
    m_color= newColor;
    emit colorChanged();
}

DiceController::DiceType DiceController::face() const
{
    return m_face;
}

int DiceController::value() const
{
    return m_value;
}

void DiceController::setValue(int newValue)
{
    if(m_value == newValue)
        return;
    // qDebug() << "setvalue" << newValue;
    m_value= newValue;
    emit valueChanged();
}

QVector3D DiceController::position() const
{
    return m_position;
}

void DiceController::setPosition(const QVector3D& newPosition)
{
    if(m_position == newPosition)
        return;
    m_position= newPosition;
    emit positionChanged();
}

QVector3D DiceController::rotation() const
{
    return m_rotation;
}

void DiceController::setRotation(const QVector3D& newRotation)
{
    if(m_rotation == newRotation)
        return;
    m_rotation= newRotation;
    emit rotationChanged();
}

bool DiceController::stable() const
{
    return m_stable;
}

void DiceController::setStable(bool newStable)
{
    if(m_stable == newStable)
        return;
    m_stable= newStable;

    if(!m_stable)
    {
        m_unstableTime= 0;
        m_unstableDistance= 0;
        m_unstableRotation= 0;
        m_unstablePosition= m_position;
    }
    else
    {
        m_unstableDistance= std::abs(m_unstablePosition.distanceToPoint(m_position));
    }

    emit stableChanged();
}

bool DiceController::selected() const
{
    return m_selected;
}

void DiceController::setSelected(bool newSelected)
{
    if(m_selected == newSelected)
        return;
    m_selected= newSelected;
    emit selectedChanged();
}

std::tuple<quint64, float, float> DiceController::unstableData() const
{
    return std::make_tuple(m_unstableTime / 1000, m_unstableDistance, m_unstableRotation);
}

void DiceController::elapsed()
{
    setStable(false);
    m_timer->start();
    auto t= m_count.restart();
    if(m_unstableTime == 0)
        m_unstableTime+= 1;
    else
        m_unstableTime+= t;
}

void DiceController::addRotationOffset(float r)
{
    m_unstableRotation+= r;
}
