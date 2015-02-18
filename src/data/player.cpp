#include "player.h"
#include <QDebug>
#include <QVariant>
Player::Player()
    : Person()
{
    m_color.setBlue(255);
    m_color.setGreen(0);
    m_color.setRed(0);
    m_children = new QList<Character*>;
}
Player::Player(QString name,QColor color,QString uri)
    : Person(name, color,uri)
{
    m_children = new QList<Character*>;
}
Player::Player(const Player& p)
        : Person()
{
    m_children = new QList<Character*>;

    *m_children = *p.children();
    Person::m_name = p.getName();
    Person::m_color = p.getColor();
    Person::m_avatar = p.getAvatar();
}

bool Player::hasChildren() const
{
    return true;
}
void Player::addCharacter(Character* c)
{
    m_children->append(c);
}

void Player::removeCharacter(int row )
{
    m_children->removeAt(row);
}
Character* Player::child(int row)
{
    if(row >= m_children->size())
        return NULL;

    return m_children->at(row);
}

QList<Character*>* Player::children() const
{
 return m_children;
}
QDataStream& operator<<(QDataStream& out, const Player& con)
{
  out << con.getName();
  out << con.getColor();
  out << con.getAvatar();
  return out;
}

QDataStream& operator>>(QDataStream& is,Player& con)
{
  is >>(con.m_name);
  is >>(con.m_color);
  is >>(con.m_avatar);
  return is;
}
