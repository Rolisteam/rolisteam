#ifndef DICESYSTEMINTERFACE_H
#define DICESYSTEMINTERFACE_H

#include <QString>
class DiceSystemInterface
{
public:
    virtual ~DiceSystemInterface();
    virtual QString getName() const =0;
    virtual QString rollDice(QString diceCmd) const=0;

};

#endif // DICESYSTEMINTERFACE_H
