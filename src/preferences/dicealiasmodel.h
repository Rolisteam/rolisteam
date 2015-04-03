#ifndef DICEALIASMODEL_H
#define DICEALIASMODEL_H

#include <QAbstractListModel>

class DiceAliasModel : public QAbstractListModel
{
public:
    enum COLUMN_TYPE {PATTERN,VALUE};
    DiceAliasModel();
    ~DiceAliasModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


private:
    QMap<QString,QString> m_DiceAliasMap;
    QStringList m_header;
};

#endif // DICEALIASMODEL_H
