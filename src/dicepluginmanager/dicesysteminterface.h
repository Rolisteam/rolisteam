/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef DICESYSTEMINTERFACE_H
#define DICESYSTEMINTERFACE_H

//#include <QString>

#include <QSettings>

class QString;
#include <QtPlugin>
/**
    * @brief plugin interface used for adding dice system.
    */
class DiceSystemInterface
{
    
public:
    /**
    * @brief virtual destructor
    */
    virtual ~DiceSystemInterface(){}
    /**
    * @brief virtual pure accessor to get the name
    */
    virtual QString getName() const =0;
    /**
    * @brief virtual pure function to roll dice according with this system.
    */
    virtual QString rollDice(QString diceCmd) const=0;
    
    /**
    * @brief read setting (typically: configuration file)
    */
    virtual void readSettings(QSettings& setting) =0;
    /**
    * @brief write setting to save any changes
    */
    virtual void writeSettings(QSettings& setting) =0;
    /**
    * @brief asks if the plugin provides GUI for its settings
    */
    virtual bool hasUiSettings() const= 0;
    /**
    * @brief Displays the GUI for configuring the plugin.
    */
    virtual void showUiSettings() = 0;
    
    virtual const QString& catchRegExp() const = 0;
    /*protected:
    QString m_regexp;*/
};
Q_DECLARE_INTERFACE(DiceSystemInterface,"org.rolisteam.DiceSystemInterface/1.0")
#endif // DICESYSTEMINTERFACE_H
