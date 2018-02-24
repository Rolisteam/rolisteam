#ifndef UNIT_H
#define UNIT_H
#include <QString>
namespace GMTOOL
{
class Unit
{
public:
    enum Category { TEMPERATURE, DISTANCE,CURRENCY,VOLUME,MASS};
    Unit(QString name,QString symbol, Category c);
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
#endif // UNIT_H
