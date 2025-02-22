#ifndef CUSTOMRULEMODEL_H
#define CUSTOMRULEMODEL_H

#include "convertoroperator.h"
#include "rwidgets_global.h"
#include "unit.h"
#include "unitmodel.h"
#include <QAbstractTableModel>
#include <vector>
namespace GMTOOL
{
class RWIDGET_EXPORT CustomRuleModel : public CategoryModel
{
    Q_OBJECT

public:
    explicit CustomRuleModel(QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
                       int role= Qt::EditRole) override;

    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;

    // Add data:
    bool insertUnit();

    // Remove data:
    bool removeUnit(const QModelIndex& index);

    qreal convert(QPair<const Unit*, const Unit*> pair, qreal value) const;

    QPair<const Unit*, const Unit*> makePair(const QModelIndex& index) const;

    CategoryModel* units() const;
    void setUnits(CategoryModel* units);

    void setCurrentCategoryId(const QString& cat, int categoryId);

    QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*>* convertionRules() const;
    void setConvertionRules(QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*>* convertionRules);

    QModelIndex buddy(const QModelIndex& index) const override;

private:
    QHash<QPair<const Unit*, const Unit*>, ConvertorOperator*>* m_convertionRules;
    int m_currentCatId= 0;
};

} // namespace GMTOOL
#endif // CUSTOMRULEMODEL_H
