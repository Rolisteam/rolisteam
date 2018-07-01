#include "convertoroperator.h"
namespace GMTOOL
{
ConvertorOperator::ConvertorOperator(double a,double b,bool fraction,bool readOnly)
    : m_a(a),m_b(b),m_fraction(fraction),m_readOnly(readOnly)
{

}

qreal ConvertorOperator::convert(qreal convert) const
{
    if(m_fraction)
    {
        return (convert+m_b)/m_a;
    }
    return m_a*convert+m_b;
}

double ConvertorOperator::a() const
{
    return m_a;
}

void ConvertorOperator::setA(double a)
{
    if(m_readOnly)
        return;
    m_a = a;
}

bool ConvertorOperator::isReadOnly()const
{
    return m_readOnly;
}

double ConvertorOperator::b() const
{
    return m_b;
}

void ConvertorOperator::setB(double b)
{
    if(m_readOnly)
        return;
    m_b = b;
}

bool ConvertorOperator::fraction() const
{
    return m_fraction;
}

void ConvertorOperator::setFraction(bool fraction)
{
    m_fraction = fraction;
}

}
