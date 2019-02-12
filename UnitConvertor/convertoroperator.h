#ifndef CONVERTOROPERATOR_H
#define CONVERTOROPERATOR_H

#include <Qt>

namespace GMTOOL
{
    /**
     * @brief The ConvertorOperator class manages an afine function to make any convertions.
     *
     */
    class ConvertorOperator
    {
    public:
        ConvertorOperator(double a, double b= 0, bool fraction= false, bool readOnly= true);

        qreal convert(qreal convert) const;
        double a() const;
        void setA(double a);

        double b() const;
        void setB(double b);

        bool fraction() const;
        void setFraction(bool fraction);

        bool isReadOnly() const;

    private:
        double m_a;
        double m_b;
        bool m_fraction;
        bool m_readOnly;
    };

} // namespace GMTOOL
#endif // CONVERTOROPERATOR_H
