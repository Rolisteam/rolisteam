/***************************************************************************
 *	Copyright (C) 2010 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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
#include "preferences/preferencesmanager.h"
#include "preferences/preferenceslistener.h"
#include <QDebug>
#include <QDir>
#include <QMetaType>
#include <QSettings>

PreferencesManager::PreferencesManager(const QString& applicationName, const QString& subname)
    : m_optionDictionary(nullptr), m_applicationName(applicationName), m_subname(subname), m_ready(false)
{
    m_optionDictionary= new QMap<QString, QVariant>;

    // Default value
    m_optionDictionary->insert("MusicDirectory", QDir::homePath());
    m_optionDictionary->insert("ImageDirectory", QDir::homePath());
    m_optionDictionary->insert("MapDirectory", QDir::homePath());
    m_optionDictionary->insert("SessionDirectory", QDir::homePath());
    m_optionDictionary->insert("MinutesDirectory", QDir::homePath());
    m_optionDictionary->insert("ChatDirectory", QDir::homePath());
    m_optionDictionary->insert("DataDirectory", QDir::homePath());
}

PreferencesManager::~PreferencesManager()
{
    delete m_optionDictionary;
}

bool PreferencesManager::registerValue(const QString& key, QVariant value, bool overwrite)
{
    if((overwrite) || (!m_optionDictionary->contains(key)))
    {
        QVariant oldValue;
        if(m_optionDictionary->contains(key))
        {
            oldValue= m_optionDictionary->value(key);
        }
        m_optionDictionary->insert(key, value);
        if(oldValue != value)
        {
            writeSettings();
            emit dataChanged(key, value);
            notifyListener(key, value);
        }
        return true;
    }
    else
        return false;
}
const QVariant PreferencesManager::value(const QString& key, const QVariant& defaultValue)
{
    return m_optionDictionary->contains(key) ? m_optionDictionary->value(key) : defaultValue;
}
void PreferencesManager::readSettings()
{
    QSettings settings(m_applicationName, m_subname);
    settings.beginGroup("rolisteam/preferences");
    int size= settings.beginReadArray("preferenceMap");
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString key= settings.value("key").toString();
        QVariant value= settings.value("value");
        m_optionDictionary->insert(key, value);
    }
    settings.endArray();
    settings.endGroup();

    notifyAllListener();
    setReady(true);
}
void PreferencesManager::writeSettings()
{
    QSettings settings(m_applicationName, m_subname);

    settings.beginGroup("rolisteam/preferences");
    settings.beginWriteArray("preferenceMap");
    settings.clear();
    auto const& keys= m_optionDictionary->keys();
    for(int i= 0; i < m_optionDictionary->size(); ++i)
    {
        settings.setArrayIndex(i);
        QString key= keys.at(i);
        QVariant var= m_optionDictionary->value(key);
        settings.setValue("key", key);
        settings.setValue("value", var);
    }
    settings.endArray();
    settings.endGroup();
}
void PreferencesManager::registerListener(const QString& str, PreferencesListener* listerner)
{
    m_listernerMap.insert(str, listerner);
}
void PreferencesManager::registerLambda(const QString& key, std::function<void(QVariant)> func)
{
    m_lambdaMap.insert({key, func});
    func(value(key, QVariant()));
}

bool PreferencesManager::ready() const
{
    return m_ready;
}

void PreferencesManager::notifyAllListener()
{
    for(const auto& [key, value] : m_optionDictionary->asKeyValueRange())
    {
        notifyListener(key, value);
    }
}

void PreferencesManager::setReady(bool r)
{
    if(r == m_ready)
        return;
    m_ready= r;
    emit readyChanged();
}

void PreferencesManager::notifyListener(const QString& key, const QVariant& value)
{
    PreferencesListener* tmp= m_listernerMap.value(key);
    if(nullptr != tmp)
    {
        tmp->preferencesHasChanged(key);
    }

    auto it= m_lambdaMap.find(key);
    if(it != m_lambdaMap.end())
    {
        auto func= it->second;
        if(nullptr != func)
            func(value);
    }
}
