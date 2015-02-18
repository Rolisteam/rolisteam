#ifndef SCRIPTDICESYSTEM_H
#define SCRIPTDICESYSTEM_H

#include "dicesysteminterface.h"
#include <QObject>

class ScriptDiceSystem : public QObject,public DiceSystemInterface
{
    Q_OBJECT

    Q_INTERFACES(DiceSystemInterface)
public:
    ScriptDiceSystem();
    ~ScriptDiceSystem();
    virtual QString getName() const;
    virtual QString rollDice(QString diceCmd) const;

    virtual void readSettings();
    virtual void writeSettings();

    virtual bool hasUiSettings() const;
    virtual void showUiSettings();

virtual const QString& catchRegExp() const;
private:
    QString m_regexp;
};

#endif // SCRIPTDICESYSTEM_H
