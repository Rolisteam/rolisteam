#ifndef GENERICDICESYSTEM_H
#define GENERICDICESYSTEM_H

#include <QObject>
#include "dicesysteminterface.h"


class GenericDiceSystem : public QObject,public DiceSystemInterface
{
    Q_OBJECT

    Q_INTERFACES(DiceSystemInterface)

public:
    GenericDiceSystem();
    ~GenericDiceSystem();
    virtual QString getName() const;
    virtual QString rollDice(QString diceCmd) const;

    virtual void readSettings();
    virtual void writeSettings();

    virtual bool hasUiSettings() const;
    virtual void showUiSettings();
};


#endif // GENERICDICESYSTEM_H
