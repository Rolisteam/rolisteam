#include "character.h"
Character::Character()
    : Person()
{

}

Character::Character(QString name,QColor color)
    : Person(name,color)
{

}
Character::Character(const Character& p)
                : Person()
{
    Person::m_name = p.getName();
    Person::m_color = p.getColor();
}

bool Character::hasChildren() const
{
    return false;
}
