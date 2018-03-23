#include "diceshortcut.h"

DiceShortCut::DiceShortCut()
{

}
QString DiceShortCut::text() const
{
    return m_text;
}

void DiceShortCut::setText(const QString &text)
{
    m_text = text;
}

QString DiceShortCut::command() const
{
    return m_command;
}

void DiceShortCut::setCommand(const QString &command)
{
    m_command = command;
}

bool DiceShortCut::alias() const
{
    return m_alias;
}

void DiceShortCut::setAlias(bool alias)
{
    m_alias = alias;
}

