#ifndef CHARACTER_H
#define CHARACTER_H
#include "person.h"
#include <QMetaType>
class Character : public Person
{
public:
    Character(QString name,QColor color);
    Character();
    Character(const Character& p);
    bool hasChildren() const;

private:
    Person* m_parent;
};
Q_DECLARE_METATYPE(Character)
#endif // CHARACTER_H
