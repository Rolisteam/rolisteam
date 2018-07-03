#include "unit.h"

namespace GMTOOL
{

Unit::Unit()
{

}
Unit::Unit(QString n,QString s,Category c)
    : m_name(n),m_symbol(s),m_currentCat(c)
{

}
Unit::Unit(const Unit &copy)
    : m_name(copy.name()),m_symbol(copy.symbol()),
      m_currentCat(copy.currentCat())
{

}

Unit::~Unit()
{

}
/*Unit::Unit(const Unit&& copy)
    : m_name(std::move(copy.name())),
      m_symbol(std::move(copy.symbol())),
      m_currentCat(std::move(copy.currentCat()))
{

}*/

Unit &Unit::operator=(const Unit & b)
{
    m_name = b.name();
    m_symbol = b.symbol();
    m_currentCat = b.currentCat();

    return *this;
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

bool Unit::readOnly() const
{
    return m_readOnly;
}

void Unit::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}

}



