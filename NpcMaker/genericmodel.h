#ifndef GENERICMODEL_H
#define GENERICMODEL_H

#include <QAbstractTableModel>
class CharacterField;
class GenericModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit GenericModel(QStringList cols, QVector<int> readOnlyCols= QVector<int>(), QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addData(CharacterField* type);
    void removeData(const QModelIndex&);

    QList<CharacterField*>::iterator begin();
    QList<CharacterField*>::iterator end();

private:
    QList<CharacterField*> m_data;
    QStringList m_columnList;
    QVector<int> m_readOnlyCols;
};

#endif // GENERICMODEL_H
