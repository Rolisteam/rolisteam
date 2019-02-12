#ifndef UNIT_H
#define UNIT_H
#include <QMetaType>
#include <QString>

namespace GMTOOL
{
    class Unit
    {
    public:
        enum Category
        {
            TEMPERATURE,
            DISTANCE,
            CURRENCY,
            VOLUME,
            MASS,
            CUSTOM
        };
        Unit();
        Unit(QString name, QString symbol, Category c);
        Unit(const Unit& copy);
        ~Unit();
        // Unit(const Unit&& copy);
        Unit& operator=(const Unit&);
        QString symbol() const;
        void setSymbol(const QString& symbol);

        QString name() const;
        void setName(const QString& name);

        Unit::Category currentCat() const;
        void setCurrentCat(const Unit::Category& currentCat);

        bool readOnly() const;
        void setReadOnly(bool readOnly);

    private:
        QString m_name;
        QString m_symbol;
        Unit::Category m_currentCat;
        bool m_readOnly= true;
    };
} // namespace GMTOOL
Q_DECLARE_METATYPE(GMTOOL::Unit)
Q_DECLARE_METATYPE(GMTOOL::Unit*)
#endif // UNIT_H
