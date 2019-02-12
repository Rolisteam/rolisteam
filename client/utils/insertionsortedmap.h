#ifndef INSERTIONSORTEDLIST_H
#define INSERTIONSORTEDLIST_H

#include <QHash>
#include <QList>

// template<typename Key,typename Value> class InsertionSortedList;

template <class Key, class Value>
class InsertionSortedMap
{
public:
    InsertionSortedMap();

    void append(const Key& key, const Value& value);
    void insert(int pos, const Key& key, const Value& value);
    void prepend(const Key& key, const Value& value);

    const Value getValue(const Key& key) const;
    const Value getValueFromIndex(int index) const;

    int size();

private:
    QList<Key> m_keys;
    QHash<Key, Value> m_data;
};

template <class Key, class Value>
InsertionSortedMap<Key, Value>::InsertionSortedMap()
{
}

template <class Key, class Value>
void InsertionSortedMap<Key, Value>::append(const Key& key, const Value& value)
{
    m_keys.append(key);
    m_data.insert(key, value);
}

template <class Key, class Value>
void InsertionSortedMap<Key, Value>::insert(int pos, const Key& key, const Value& value)
{
    m_keys.insert(pos, key);
    m_data.insert(key, value);
}

template <class Key, class Value>
void InsertionSortedMap<Key, Value>::prepend(const Key& key, const Value& value)
{
    m_keys.prepend(key);
    m_data.insert(key, value);
}

template <class Key, class Value>
const Value InsertionSortedMap<Key, Value>::getValue(const Key& key) const
{
    return m_data.value(key);
}

template <class Key, class Value>
const Value InsertionSortedMap<Key, Value>::getValueFromIndex(int index) const
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_keys.size());

    return m_data.value(m_keys.at(index));
}

template <class Key, class Value>
int InsertionSortedMap<Key, Value>::size()
{
    return m_keys.size();
}
#endif // INSERTIONSORTEDLIST_H
