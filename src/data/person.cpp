#include "person.h"
Person::Person()
{
    m_state = Qt::Unchecked;
}

Person::Person(QString name,QColor color)
    : m_name(name),m_color(color)
{
    m_state = Qt::Unchecked;
}
QString Person::getName() const
{
    return m_name;
}
QColor Person::getColor() const
{
    return m_color;
}
void Person::setName(QString p)
{
    m_name = p;
}

void Person::setColor(QColor p)
{
    m_color=p;
}
void Person::setState(Qt::CheckState m)
{

    m_state = m;

}

Qt::CheckState Person::checkedState()
{
    return m_state;
}

