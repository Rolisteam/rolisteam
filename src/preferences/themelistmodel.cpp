#include "themelistmodel.h"
#include "theme.h"
ThemeListModel::ThemeListModel(QObject *parent) :
    QAbstractListModel(parent)
{
        m_themeList=new ThemeList();
}
void ThemeListModel::addTheme(Theme* tmp)
{
    beginInsertRows (QModelIndex(),  m_themeList->size(), m_themeList->size()+1 );

    m_themeList->append(tmp);
    endInsertRows ();
}
int ThemeListModel::rowCount ( const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_themeList->size();

}
Qt::ItemFlags ThemeListModel::flags ( const QModelIndex & index ) const
{
    return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled;
}
bool ThemeListModel::setData ( const QModelIndex & index, const QVariant & value, int role)
{
    if(!index.isValid())
        return false;
    if(role==Qt::EditRole)
    {
        m_themeList->at(index.row())->setName(value.toString());
        return true;
    }

    return false;
}

QVariant ThemeListModel::data ( const QModelIndex & index, int role ) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row()>=m_themeList->size())
        return  QVariant();

    if((Qt::DisplayRole == role)||(role==Qt::EditRole))
    {
        return m_themeList->at(index.row())->name();
    }
    return QVariant();
}
void ThemeListModel::removeSelectedTheme(int row)
{
    beginRemoveRows(QModelIndex(),row,row+1);
    m_themeList->removeAt(row);
    endRemoveRows();
}
Theme* ThemeListModel::getTheme(int row)
{
    if(row>=m_themeList->size())
        return NULL;

    return m_themeList->at(row);
}
