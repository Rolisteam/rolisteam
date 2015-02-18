#include "scriptdicesystem.h"

ScriptDiceSystem::ScriptDiceSystem()
{

}
ScriptDiceSystem::~ScriptDiceSystem()
{

}

QString ScriptDiceSystem::getName() const
{
    return QString("Script");
}
QString ScriptDiceSystem::rollDice(QString diceCmd) const
{
    return QString("Script plugin not yet implemented");
}
void ScriptDiceSystem::readSettings()
{

}

void ScriptDiceSystem::writeSettings()
{

}

bool ScriptDiceSystem::hasUiSettings() const
{
    return false;
}
void ScriptDiceSystem::showUiSettings()
{

}
const QString& ScriptDiceSystem::catchRegExp() const
{
    return m_regexp;
}
Q_EXPORT_PLUGIN2(dicescripter,ScriptDiceSystem);
