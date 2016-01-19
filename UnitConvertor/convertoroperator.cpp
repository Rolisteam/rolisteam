#include "convertoroperator.h"

ConvertorOperator::ConvertorOperator(double a,double b)
    : m_a(a),m_b(b)
{

}

qreal ConvertorOperator::convert(qreal convert)
{
    return m_a*convert+m_b;
}

