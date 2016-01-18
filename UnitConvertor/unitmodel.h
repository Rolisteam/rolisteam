#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <QAbstractListModel>
#include "unit.h"

class UnitModel : public QAbstractListModel
{
public:
    UnitModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    void insertData(Unit* unit);

private:
    QMap<Unit::Category,Unit*> m_data;

};

#endif // UNITMODEL_H
