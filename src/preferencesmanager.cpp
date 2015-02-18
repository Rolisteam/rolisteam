/***************************************************************************
    *	Copyright (C) 2010 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify  *
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
#include "preferencesmanager.h"
#include <QSettings>
#include <QDir>
#include <QDebug>


PreferencesManager::PreferencesManager()
    : m_optionDictionary(NULL)
{
    m_optionDictionary = new QMap<QString,QVariant>;
    

    
    //Default value
    m_optionDictionary->insert("MusicDirectory",QDir::homePath());
    m_optionDictionary->insert("ImageDirectory",QDir::homePath());
    m_optionDictionary->insert("MapDirectory",QDir::homePath());
    m_optionDictionary->insert("StoryDirectory",QDir::homePath());
    m_optionDictionary->insert("MinutesDirectory",QDir::homePath());
    m_optionDictionary->insert("ChatDirectory",QDir::homePath());
    m_optionDictionary->insert("DataDirectory",QDir::homePath());
    
}

PreferencesManager::~PreferencesManager()
{
    delete m_optionDictionary;
}

PreferencesManager* PreferencesManager::m_singleton = NULL;


PreferencesManager* PreferencesManager::getInstance()
{
    
    if(m_singleton == NULL)
        m_singleton = new PreferencesManager;
    
    return m_singleton;
}
bool PreferencesManager::registerValue(QString key,QVariant value, bool overwrite)
{
    qDebug() << "registervalue" << key << value;
    if((overwrite)||(!m_optionDictionary->contains(key)))
    {
        m_optionDictionary->insert(key,value);
        return true;
    }
    else
        return false;
}
const QVariant PreferencesManager::value(QString key,QVariant defaultValue)
{
    qDebug() << m_optionDictionary;
    if(m_optionDictionary->contains(key))
    {
        return m_optionDictionary->value(key);
    }
    else
    {
        return defaultValue;
    }
}

void PreferencesManager::readSettings(QSettings & settings)
{
    settings.beginGroup("rolisteam/preferences");
    QVariant variant = settings.value("map",*m_optionDictionary);
    if(variant.canConvert<QMap<QString,QVariant> >())
    {
        *m_optionDictionary = variant.value<QMap<QString,QVariant> >();
    }
    
    //   qDebug() << "size reading dico" << m_optionDictionary->size();

    
    settings.endGroup();
    
}
void PreferencesManager::writeSettings(QSettings & settings)
{
    settings.beginGroup("rolisteam/preferences");
    settings.setValue("map",*m_optionDictionary);
    //  qDebug() << "size writing dico" << m_optionDictionary->size();
    for(int i = 0;m_optionDictionary->size()>i;i++)
    {
         qDebug() << "key " << m_optionDictionary->keys().at(i);
         qDebug() << "value " << m_optionDictionary->value(m_optionDictionary->keys().at(i),true).toString();
    }
    settings.endGroup();
}
