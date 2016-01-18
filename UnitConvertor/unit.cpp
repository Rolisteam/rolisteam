#include "unit.h"

Unit::Unit(QString n,QString s,Category c)
    : m_name(name),m_symbol(s),m_currentCat(c)
{

}
QString Unit::symbol() const
{
    return m_symbol;
}

void Unit::setSymbol(const QString &symbol)
{
    m_symbol = symbol;
}
QString Unit::name() const
{
    return m_name;
}

void Unit::setName(const QString &name)
{
    m_name = name;
}
Unit::Category Unit::currentCat() const
{
    return m_currentCat;
}

void Unit::setCurrentCat(const Unit::Category &currentCat)
{
    m_currentCat = currentCat;
}




