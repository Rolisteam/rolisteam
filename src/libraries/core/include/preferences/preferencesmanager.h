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
#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QMap>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <functional>

class PreferencesListener;
#include <core_global.h>
/**
 * @index <h2>How To use the Preference Manager.</h2>
 *  <p>It is always painful to manage of data between two sessions of Rolisteam.
 * We implemented a powerful tool which helps contributors to make it easier.
 * We will see all required steps to load/save and update data.
 * </p>
 * <h3>First thing at all</h3> You need to get pointer to this class by including preferencesmanager.h. Don't forget to
 * add pointer to PreferencesManager into your class. Now, it's better if you get the address of the unique instance of
 * the preferences manager, this can be done by calling the static function PreferencesManager::getInstance();
 *
 */
/**
 * @brief Store options and manage access to their value.
 * Save/load values in/from QSetting instance.
 */
class PreferencesManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged FINAL)
public:
    /**
     * @brief Private constructor to make sure there is only one instance of this.
     */
    PreferencesManager(const QString& applicationName, const QString& subname);

    /**
     * @brief  desturctor
     */
    ~PreferencesManager();

    /**
     * @brief register a couple key/value in the map.
     *
     * @param key unique string to identify the value
     * @param value current value for the couple
     * @param overwrite replace or not the value associated to the key if there is already stored.
     * @return whether or not the insertion was successfully done
     */
    bool registerValue(const QString& key, QVariant value, bool overwrite= true);

    /**
     * @brief accessor to the map
     *
     * @param key unique string to identify the value
     * @param defaultValue this value is returned if the key is notin the dictionary
     * @return const reference to the value
     */
    const QVariant value(const QString& key, const QVariant& defaultValue);
    void readSettings();
    void writeSettings();
    /**
     * @brief registerListener
     */
    void registerListener(const QString&, PreferencesListener*);
    void registerLambda(const QString& key, std::function<void(QVariant)> func);

    bool ready() const;

private:
    /**
     * @brief notifyListener
     */
    void notifyListener(const QString& key, const QVariant& value);
    void notifyAllListener();

    void setReady(bool r);

signals:
    void dataChanged(const QString& key, const QVariant& value);
    void readyChanged();

private:
    /**
     * data structure to store any element.
     * The key is a QString, the value is a QVariant.
     */
    QMap<QString, QVariant>* m_optionDictionary;
    /**
     * @brief m_listernerMap
     */
    QMap<QString, PreferencesListener*> m_listernerMap;
    /**
     * @brief m_lambdaMap
     */
    std::map<QString, std::function<void(QVariant)>> m_lambdaMap;
    QString m_applicationName;
    QString m_subname;
    bool m_ready;
};

#endif // PREFERENCESMANAGER_H
