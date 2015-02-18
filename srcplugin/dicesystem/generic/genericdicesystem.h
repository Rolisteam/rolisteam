#ifndef GENERICDICESYSTEM_H
#define GENERICDICESYSTEM_H
#include "dicesysteminterface.h"
#include <QObject>

class GenericDiceSystem : public QObject,public DiceSystemInterface
{
    Q_OBJECT

    Q_INTERFACES(DiceSystemInterface)

public:
    GenericDiceSystem();
    ~GenericDiceSystem();
    virtual QString getName() const;
    virtual QString rollDice(QString diceCmd) const;
};


#endif // GENERICDICESYSTEM_H
