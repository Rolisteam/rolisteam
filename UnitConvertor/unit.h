#ifndef UNIT_H
#define UNIT_H
#include <QString>
#include <QMetaType>

namespace GMTOOL
{
class Unit
{
public:
    enum Category { TEMPERATURE, DISTANCE,CURRENCY,VOLUME,MASS,CUSTOM};
    Unit();
    Unit(QString name,QString symbol, Category c);
    Unit(const Unit &copy);
    ~Unit();
    //Unit(const Unit&& copy);
    Unit& operator=(const Unit&);
    QString symbol() const;
    void setSymbol(const QString &symbol);

    QString name() const;
    void setName(const QString &name);

    Unit::Category currentCat() const;
    void setCurrentCat(const Unit::Category &currentCat);


private:
    QString m_name;
    QString m_symbol;
    Unit::Category m_currentCat;
};
}
Q_DECLARE_METATYPE(GMTOOL::Unit)
Q_DECLARE_METATYPE(GMTOOL::Unit*)
#endif // UNIT_H
