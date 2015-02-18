#ifndef PLAYER_H
#define PLAYER_H
#include <QList>
#include <QMetaType>
#include "person.h"
#include "character.h"
class Player : public Person
{
public:
    Player(QString name,QColor color);
    Player();
    Player(const Player& p);

    bool hasChildren() const;
    void addCharacter(Character* );
    void removeCharacter(int row );

    Character* child(int row);
    QList<Character*>* children() const;
private:
    QList<Character*>* m_children;

    friend QDataStream& operator<<(QDataStream& os,const Player&);
    friend QDataStream& operator>>(QDataStream& is,Player&);
};
Q_DECLARE_METATYPE(Player)
#endif // PLAYER_H
