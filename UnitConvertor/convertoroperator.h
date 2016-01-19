#ifndef CONVERTOROPERATOR_H
#define CONVERTOROPERATOR_H

#include <Qt>


class ConvertorOperator
{
public:
    ConvertorOperator(double a,double b=0);

    qreal convert(qreal convert);
private:
    double m_a;
    double m_b;
};

#endif // CONVERTOROPERATOR_H
