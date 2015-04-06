#ifndef DICEALIASMODEL_H
#define DICEALIASMODEL_H

#include <QAbstractListModel>

#include "dicealias.h"

//typedef QPair<QString,QString> DiceAlias;

class DiceAliasModel : public QAbstractListModel
{
public:
    enum COLUMN_TYPE {PATTERN,VALUE,METHOD};
    DiceAliasModel();
    ~DiceAliasModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);


	///new methods
    void setAliases(QList<DiceAlias*>* map);
	void appendAlias();
private:
    QList<DiceAlias*>* m_diceAliasList;
    QStringList m_header;
};

#endif // DICEALIASMODEL_H
