#include "characterlist.h"

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"

#include <QDebug>

CharacterList::CharacterList(CharacterSheetModel* sourceModel, QObject* parent)
    : QAbstractListModel(parent), m_source(sourceModel)
{
}

int CharacterList::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || !m_source)
        return 0;
    return m_source->columnCount(QModelIndex());
}

QHash<int, QByteArray> CharacterList::roleNames() const
{
    return QHash<int, QByteArray>({{NameRole, "name"}, {UuidRole, "uuid"}});
}

Qt::ItemFlags CharacterList::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CharacterList::data(const QModelIndex& index, int role) const
{
    QVector<int> roles({NameRole, UuidRole, Qt::DisplayRole, Qt::EditRole});
    if(!index.isValid() || !roles.contains(role))
        return QVariant();

    auto r= index.row();

    if(r == 0)
    {
        return (role == Qt::DisplayRole) ? tr("Mock Data") : "";
    }

    auto sheet= m_source->getCharacterSheet(r - 1);

    QVariant res;

    switch(role)
    {
    case NameRole:
    case Qt::EditRole:
        res= sheet->name();
        break;
    case UuidRole:
        res= sheet->uuid();
        break;
    }
    return res;
}
