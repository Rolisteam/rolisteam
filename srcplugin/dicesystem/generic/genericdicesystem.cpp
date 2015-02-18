#include "genericdicesystem.h"
#include <QtGui>

GenericDiceSystem::GenericDiceSystem()
{
}
GenericDiceSystem::~GenericDiceSystem()
{

}

QString GenericDiceSystem::getName() const
{
    return QString("Generic");
}

QString GenericDiceSystem::rollDice(QString diceCmd) const
{
    return tr("not implemented yet");
}
Q_EXPORT_PLUGIN2(generic,GenericDiceSystem);
