#include "person.h"
Person::Person()
{

}

Person::Person(QString name,QColor color)
    : m_name(name),m_color(color)
{
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


