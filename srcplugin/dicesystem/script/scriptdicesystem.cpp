/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "scriptdicesystem.h"

ScriptDiceSystem::ScriptDiceSystem()
    : m_regexp(".*:\\(.+\\).*")
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
void ScriptDiceSystem::readSettings(QSettings& setting)
{

}

void ScriptDiceSystem::writeSettings(QSettings& setting)
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
