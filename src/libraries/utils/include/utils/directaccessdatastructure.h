/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef DIRECTACCESSDATASTRUCTURE_H
#define DIRECTACCESSDATASTRUCTURE_H

#include "utils/utils_global.h"
#include <QHash>
#include <QList>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QWriteLocker>

template <class T, class Y>
class UTILS_EXPORT DirectAccessDataStructure
{
public:
    DirectAccessDataStructure(){};

    void reserve(qsizetype size)
    {
        QWriteLocker locker(&m_lock);
        m_keys.reserve(size);
        m_data.reserve(size);
    };

    auto begin() const { return m_data.keyValueBegin(); };
    auto end() const { return m_data.keyValueEnd(); };

    Y operator[](qsizetype idx) const
    {
        QReadLocker locker(&m_lock);
        return m_data[m_keys[idx]];
    };

    Y value(const T& key) const
    {
        QReadLocker locker(&m_lock);
        return m_data[key];
    };

    void changeKey(const T& oldkey, const T& newkey, const Y& child)
    {
        QWriteLocker locker(&m_lock);
        m_keys.replace(m_keys.indexOf(oldkey), newkey);
        m_data.remove(oldkey);
        m_data.insert(newkey, child);
    }

    bool isEmpty() const { return m_data.isEmpty(); };

    qsizetype size() const { return m_data.size(); };
    const T key(qsizetype idx) const
    {
        QReadLocker locker(&m_lock);
        return m_keys[idx];
    };

    void append(const T& key, const Y& value)
    {
        QWriteLocker locker(&m_lock);
        m_data.insert(key, value);
        m_keys.append(key);
    };

    void swapItemsAt(qsizetype i, qsizetype j)
    {
        if(i < 0 || i > m_keys.size() || j > m_keys.size() || j < 0)
            return;

        QWriteLocker locker(&m_lock);
        m_keys.swapItemsAt(i, j);
    }

    void remove(const T& key)
    {
        QWriteLocker locker(&m_lock);
        if(m_data.remove(key))
        {
            m_keys.removeAll(key);
        }
    }

    bool contains(const T& key) const { return m_data.contains(key); }
    typename QHash<T, Y>::iterator find(const T& key) { return m_data.find(key); }
    typename QHash<T, Y>::const_iterator find(const T& key) const { return m_data.find(key); }
    typename QHash<T, Y>::iterator keyEnd() { return m_data.end(); }
    typename QHash<T, Y>::const_iterator keyConstEnd() const { return m_data.cend(); }
    typename QHash<T, Y>::iterator keyBegin() { return m_data.begin(); }
    typename QHash<T, Y>::const_iterator keyConstBegin() const { return m_data.cbegin(); }

    typename QList<T>::iterator orderedBegin() { return m_keys.begin(); }
    typename QList<T>::const_iterator orderedConstBegin() const { return m_keys.cbegin(); }
    typename QList<T>::iterator orderedEnd() { return m_keys.end(); }
    typename QList<T>::const_iterator orderedConstEnd() const { return m_keys.cend(); }

    qsizetype index(const T& key) const { return m_keys.indexOf(key); }

    template <typename Container, std::enable_if_t<std::is_same_v<QList<T>, Container>, bool> = true>
    Container keys() const
    {
        return m_keys;
    }

    template <typename Container, std::enable_if_t<std::is_same_v<QList<Y>, Container>, bool> = true>
    Container values() const
    {
        return m_data.values();
    }

    template <typename Container, std::enable_if_t<!std::is_same_v<QList<T>, Container>, bool> = true>
    Container keys() const
    {
        return {m_keys.cbegin(), m_keys.cend()};
    }

    void clear()
    {
        QWriteLocker locker(&m_lock);
        m_data.clear();
        m_keys.clear();
    };

    bool isValid() const { return m_keys.size() == m_data.size(); }

private:
    QList<T> m_keys;
    QHash<T, Y> m_data;
    mutable QReadWriteLock m_lock;
};

#endif // DIRECTACCESSDATASTRUCTURE_H
