#include "convertoroperator.h"

ConvertorOperator::ConvertorOperator(double a,double b,bool fraction)
    : m_a(a),m_b(b),m_fraction(fraction)
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

