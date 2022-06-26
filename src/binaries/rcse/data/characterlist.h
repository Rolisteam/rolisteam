#ifndef CHARACTERLIST_H
#define CHARACTERLIST_H

#include <QAbstractListModel>
#include <QPointer>
#include <QSortFilterProxyModel>

class CharacterSheetModel;
class CharacterList : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        NameRole= Qt::DisplayRole,
        UuidRole= Qt::UserRole
    };
    explicit CharacterList(CharacterSheetModel* sourceModel, QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QPointer<CharacterSheetModel> m_source;
};

#endif // CHARACTERLIST_H
