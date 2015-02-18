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

#include "pluginmanager.h"
#include "dicesysteminterface.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QtGui>

DicePlugInManager::DicePlugInManager(QObject *parent) :
    QObject(parent)
{
    m_interfaceList = new QList<DiceSystemInterface*>;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin)
        {
            DiceSystemInterface * op = qobject_cast<DiceSystemInterface *>(plugin);
            if (op)
            {
                m_interfaceList->append(op);
            }
        }
        else
            qDebug() << loader.errorString();
    }
    
}
DicePlugInManager::~DicePlugInManager()
{
    
}
DicePlugInManager* DicePlugInManager::m_singleton = NULL;

DicePlugInManager* DicePlugInManager::instance()
{
    if(!m_singleton)
        m_singleton = new DicePlugInManager();
    
    return m_singleton;
}

DiceSystemInterface* DicePlugInManager::getInterface(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(tmp->getName()==name)
            return tmp;
    }
    return NULL;
}
QStringList DicePlugInManager::getInterfaceList()
{
    QStringList interfaceNames;
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        interfaceNames << tmp->getName();
    }
    return interfaceNames;
}
void DicePlugInManager::readSettings(QSettings& setting)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        tmp->readSettings(setting);
    }
}

void DicePlugInManager::writeSettings(QSettings& setting)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        tmp->writeSettings(setting);
    }
}
void DicePlugInManager::showUISettings(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(name==tmp->getName())
            tmp->showUiSettings();
    }
}

bool DicePlugInManager::hasUISettings(QString& name)
{
    foreach(DiceSystemInterface* tmp,*m_interfaceList)
    {
        if(name==tmp->getName())
            return tmp->hasUiSettings();
    }
    return false;
}
