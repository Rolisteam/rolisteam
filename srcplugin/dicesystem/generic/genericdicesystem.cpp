#include "genericdicesystem.h"
#include <QtGui>

GenericDiceSystem::GenericDiceSystem()
    : m_regexp("![1-9]+d[1-9]+")
{
    //m_regexp =QString("![1-9]+d[1-9]+");
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

void GenericDiceSystem::readSettings(QSettings& setting)
{

}
void GenericDiceSystem::writeSettings(QSettings& setting)
{

}
bool GenericDiceSystem::hasUiSettings() const
{
    return false;
}

void GenericDiceSystem::showUiSettings()
{

}
const QString& GenericDiceSystem::catchRegExp() const
{
    return m_regexp;
}
Q_EXPORT_PLUGIN2(generic,GenericDiceSystem);
