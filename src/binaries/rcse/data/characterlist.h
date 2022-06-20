#ifndef CHARACTERLIST_H
#define CHARACTERLIST_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class CharacterList : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum Roles {
      NameRole=Qt::DisplayRole,
      UuidRole = Qt::UserRole
    };
    explicit CharacterList(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
};

#endif // CHARACTERLIST_H
