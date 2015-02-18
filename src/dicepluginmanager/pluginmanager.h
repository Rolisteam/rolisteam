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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QObject>
#include <QList>
#include <QStringList>
#include <QSettings>

class DiceSystemInterface;
/**
    * @brief manages dice system plugin, it provides an API to run the dice command, and configure the plugin.
    */
class DicePlugInManager : public QObject
{
    Q_OBJECT
public:
    /**
    * @brief destructor
    */
    ~DicePlugInManager();
    /**
    * @brief instance provider, part of singleton pattern.
    */
    static DicePlugInManager* instance();
    /**
    * @brief accessor to the given interface.
    */
    DiceSystemInterface* getInterface(QString& name);
    /**
    * @brief accessor to the interface list
    */
    QStringList getInterfaceList();
    
    /**
    * @brief calls every interfaces to read their settings
    */
    void readSettings(QSettings& setting);
    
    /**
    * @brief calls every interfaces to write their settings.
    */
    void writeSettings(QSettings& setting);
    /**
    * @brief calls the same function to the given interface.
    */
    void showUISettings(QString& name);
    /**
    * @brief allows to know if the given interface has UI for settings
    */
    bool hasUISettings(QString& name);
private:
    DicePlugInManager(QObject *parent = 0);
    
    static DicePlugInManager* m_singleton;
    
    QList<DiceSystemInterface*>* m_interfaceList;
};

#endif // PLUGINMANAGER_H
