#ifndef CONVERTOROPERATOR_H
#define CONVERTOROPERATOR_H

#include <Qt>


class ConvertorOperator
{
public:
    ConvertorOperator(double a,double b=0, bool fraction=false);

    qreal convert(qreal convert);
private:
    double m_a;
    double m_b;
    bool m_fraction;
};

#endif // CONVERTOROPERATOR_H
