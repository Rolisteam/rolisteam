#ifndef UNITMODEL_H
#define UNITMODEL_H

#include "rwidgets_global.h"
#include "unit.h"
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace GMTOOL
{
class RWIDGET_EXPORT CategoryModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CategoryModel(QObject* parent= nullptr);

    QString currentCategory() const;
    virtual void setCurrentCategory(const QString& currentCategory);

    void addUnit(Unit*);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const Q_DECL_OVERRIDE;
    QString m_currentCategory;
};

class RWIDGET_EXPORT UnitModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Category= Qt::UserRole,
        UnitRole
    };
    UnitModel(QObject* parent= nullptr);

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent= QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    Unit* insertData(Unit* unit);
    Unit* getUnitFromIndex(const QModelIndex& i, int currentCat);

    QHash<Unit::Category, QString> cat2Text() const;
    void setCat2Text(const QHash<Unit::Category, QString>& cat2Text);

    QString getCatNameFromId(Unit::Category) const;

    void readSettings();
    void writeSettings();

    int getIndex(Unit* unit);
    Unit* getUnitByIndex(int i) const;

    // Add data:
    bool insertUnit(Unit::Category cat);

    // Remove data:
    bool removeUnit(Unit* unit);

signals:
    void modelChanged();

protected:
    Unit* indexToUnit(const QModelIndex& index) const;

private:
    QMap<Unit::Category, QList<Unit*>> m_data;
    static QHash<Unit::Category, QString> m_cat2Text;
};
} // namespace GMTOOL
#endif // UNITMODEL_H
