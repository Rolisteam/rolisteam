#ifndef CUSTOMRULEMODEL_H
#define CUSTOMRULEMODEL_H

#include <QAbstractTableModel>
#include <vector>
#include "unit.h"
#include "convertoroperator.h"
#include "unitmodel.h"

namespace GMTOOL {


class CustomRuleModel : public CategoryModel
{
    Q_OBJECT

public:
    explicit CustomRuleModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    //int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

   /* QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;*/
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;


    // Add data:
    bool insertUnit();


    // Remove data:
    bool removeUnit(const QModelIndex &index);

    qreal convert(QPair<const Unit*,const Unit*> pair, qreal value) const;

    QPair<const Unit *, const Unit *> makePair(const QModelIndex &index) const;

    CategoryModel *units() const;
    void setUnits(CategoryModel* units);

    void setCurrentCategory(const QString &cat, int categoryId);

    QHash<QPair<const Unit *, const Unit *>, ConvertorOperator *> *convertionRules() const;
    void setConvertionRules(QHash<QPair<const Unit *, const Unit *>, ConvertorOperator *> *convertionRules);

    QModelIndex buddy(const QModelIndex &index) const;
private:
    //CategoryModel* m_units = nullptr;
    QHash<QPair<const Unit*, const Unit*>,ConvertorOperator*>* m_convertionRules;
    int m_currentCatId = 0;
};

}
#endif // CUSTOMRULEMODEL_H
