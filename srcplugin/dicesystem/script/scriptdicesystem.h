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

#ifndef SCRIPTDICESYSTEM_H
#define SCRIPTDICESYSTEM_H

#include "dicesysteminterface.h"
#include <QObject>
/**
 * @brief should provide subsystem to implement easy game dice rules.
 * @author Renaud Guezennec
 *
 */
class ScriptDiceSystem : public QObject,public DiceSystemInterface
{
    Q_OBJECT

    Q_INTERFACES(DiceSystemInterface)
public:
    /**
      * @brief default constructor
      */
    ScriptDiceSystem();
    /**
      * @brief destructor
      */
    ~ScriptDiceSystem();
    /**
      * @brief accessor to the plugin's name
      * @return Name
      */
    virtual QString getName() const;
    /**
      * @brief called when the user types the dice syntax.
      * @brief it should perform an analysis  of it and return the appropriate sentences.
      * @param diceCmd : the whole text which triggered syntax check
      * @return the string which should be displayed
      */
    virtual QString rollDice(QString diceCmd) const;
    /**
      * @brief read setting for this particular plugin
      */
    virtual void readSettings();
    /**
      * @brief write setting at the end of each session
      */
    virtual void writeSettings();
    /**
      * @brief either has UI setting or not
      */
    virtual bool hasUiSettings() const;
    /**
      * @brief displays settings windows (can be called only if hasUiSettings returns true.
      */
    virtual void showUiSettings();
    /**
      * @brief defines the regexp which allows the chat to know when it should call this plugin.
      */
    virtual const QString& catchRegExp() const;
private:
    /**
      * @brief stores the regexp
      */
    QString m_regexp;
};

#endif // SCRIPTDICESYSTEM_H
