/***************************************************************************
    *	Copyright (C) 2010 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
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
#include "preferenceslistener.h"
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QMetaType>

#include "data/cleveruri.h"

PreferencesManager::PreferencesManager()
    : m_optionDictionary(nullptr)
{
    m_optionDictionary = new QMap<QString,QVariant>;
    qRegisterMetaTypeStreamOperators<CleverURI>("CleverURI");
    qRegisterMetaTypeStreamOperators<CleverUriList>("CleverUriList");

    //Default value
    m_optionDictionary->insert("MusicDirectory",QDir::homePath());
    m_optionDictionary->insert("ImageDirectory",QDir::homePath());
    m_optionDictionary->insert("MapDirectory",QDir::homePath());
    m_optionDictionary->insert("SessionDirectory",QDir::homePath());
    m_optionDictionary->insert("MinutesDirectory",QDir::homePath());
    m_optionDictionary->insert("ChatDirectory",QDir::homePath());
    m_optionDictionary->insert("DataDirectory",QDir::homePath()); 
}

PreferencesManager::~PreferencesManager()
{
    delete m_optionDictionary;
}

PreferencesManager* PreferencesManager::m_singleton = nullptr;


PreferencesManager* PreferencesManager::getInstance()
{
    if(m_singleton == nullptr)
    {
        m_singleton = new PreferencesManager;
    }
    
    return m_singleton;
}
bool PreferencesManager::registerValue(QString key,QVariant value, bool overwrite)
{
    if((overwrite)||(!m_optionDictionary->contains(key)))
    {
        QVariant oldValue;
        if(m_optionDictionary->contains(key))
        {
            oldValue = m_optionDictionary->value(key);
        }
        m_optionDictionary->insert(key,value);
        if(oldValue != value)
        {
            notifyListener(key);
        }
        return true;
    }
    else
        return false;
}
const QVariant PreferencesManager::value(QString key,QVariant defaultValue)
{
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
    int size = settings.beginReadArray("preferenceMap");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString key = settings.value("key").toString();
        QVariant value = settings.value("value");
        m_optionDictionary->insert(key,value);
    }   
    settings.endArray();
    settings.endGroup();

    for(auto p : m_lambdaMap)
    {
        p.second(p.first);
    }
}
void PreferencesManager::writeSettings(QSettings & settings)
{
    qRegisterMetaTypeStreamOperators<CleverURI>("CleverURI");
    qRegisterMetaTypeStreamOperators<CleverUriList>("CleverUriList");

    settings.beginGroup("rolisteam/preferences");
    settings.beginWriteArray("preferenceMap");
    for (int i = 0; i < m_optionDictionary->size(); ++i)
    {
        settings.setArrayIndex(i);
        QString key = m_optionDictionary->keys().at(i);
        QVariant var = m_optionDictionary->value(key);
        settings.setValue("key", key);
        settings.setValue("value",var);
    }
    settings.endArray();
    settings.endGroup();
}
void PreferencesManager::registerListener(QString str, PreferencesListener* listerner)
{
   m_listernerMap.insert(str,listerner);
}
void PreferencesManager::registerLambda(QString key,std::function<void(QString)> func)
{
    m_lambdaMap.insert({key,func});
}
void PreferencesManager::notifyListener(QString str)
{
    PreferencesListener* tmp = m_listernerMap.value(str);
    if(nullptr!=tmp)
    {
        tmp->preferencesHasChanged(str);
    }

    auto it = m_lambdaMap.find(str);
    if(it!=m_lambdaMap.end())
    {
        auto func = it->second;
        if(nullptr != func)
            func(str);
    }
}
