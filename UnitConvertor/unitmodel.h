#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "unit.h"

namespace GMTOOL
{


class CategoryModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
       CategoryModel(QObject* parent);

       QString currentCategory() const;
       void setCurrentCategory(const QString &currentCategory);
protected:
       bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;
private:
       QString m_currentCategory;
};


class UnitModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {Category=Qt::UserRole};
    UnitModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    Unit* insertData(Unit* unit);
    Unit* getUnitFromIndex(const QModelIndex& i);

    QHash<Unit::Category, QString> cat2Text() const;
    void setCat2Text(const QHash<Unit::Category, QString> &cat2Text);

    QString getCatNameFromId(Unit::Category) const;

private:
    QMap<Unit::Category,Unit*> m_data;
    static QHash<Unit::Category,QString> m_cat2Text;

};
}
#endif // UNITMODEL_H
