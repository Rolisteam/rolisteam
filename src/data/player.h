#ifndef PLAYER_H
#define PLAYER_H
#include <QList>
#include <QMetaType>
#include "person.h"
#include "character.h"

/**
  * @brief represents a player, herits from person, stores any fields
  */
class Player : public Person
{
public:
    /**
    * @brief constructor with parameters
    */
    Player(QString name,QColor color,QString uri);
    /**
    * @brief default constructor
    */
    Player();
    /**
    * @brief copy constructor
    */
    Player(const Player& p);
    /**
    * @brief if the player has some characters, He has children.
    */
    bool isLeaf() const;
    /**
    * @brief adds character to this player
    */
    void addCharacter(Character* );
    /**
    * @brief remove character at the given row
    */
    void removeCharacter(int row );
    /**
    * @brief child accessor given row.
    */
    Character* child(int row) const;
    /**
    * @brief const accessor to the list of children
    */
    QList<Character*>* children() const;

    int childrenCount() const;
private:
    /**
    * @brief stores the children's list
    */
    QList<Character*>* m_children;
    /**
    * @brief writing serialization operator
    */
    friend QDataStream& operator<<(QDataStream& os,const Player&);
    /**
    * @brief reading serialization operator
    */
    friend QDataStream& operator>>(QDataStream& is,Player&);
};
Q_DECLARE_METATYPE(Player)
#endif // PLAYER_H
