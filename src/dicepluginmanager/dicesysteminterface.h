#ifndef DICESYSTEMINTERFACE_H
#define DICESYSTEMINTERFACE_H

//#include <QString>
class QString;
#include <QtPlugin>
class DiceSystemInterface
{

public:
    virtual ~DiceSystemInterface(){}
    virtual QString getName() const =0;
    virtual QString rollDice(QString diceCmd) const=0;

    virtual void readSettings() =0;
    virtual void writeSettings() =0;

    virtual bool hasUiSettings() const= 0;
    virtual void showUiSettings() = 0;

};
Q_DECLARE_INTERFACE(DiceSystemInterface,"org.rolisteam.DiceSystemInterface/1.0")
#endif // DICESYSTEMINTERFACE_H
