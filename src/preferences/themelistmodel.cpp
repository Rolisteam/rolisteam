#include "themelistmodel.h"
#include "theme.h"
ThemeListModel::ThemeListModel(QObject *parent) :
    QAbstractListModel(parent)
{
        m_themeList=new QList<Theme*>();
}
void ThemeListModel::addTheme()
{
    beginInsertRows (QModelIndex(),  m_themeList->size(), m_themeList->size()+1 );
    Theme* tmp=new Theme();
    tmp->setName(tr("Theme %1").arg(m_themeList->size()));
    m_themeList->append(tmp);
    endInsertRows ();
    //reset();
    //insertRows(m_themeList->size(),1);
    //tmpitem->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    //ui->m_themeList->addItem(tmpitem);

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
    if(role==Qt::DisplayRole)
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
void ThemeListModel::removeSelectedTheme()
{
   /* int index = ui->m_themeList->currentRow();
    m_themeList->removeAt(index);
    ui->m_themeList->removeItemWidget(ui->m_themeList->currentItem());*/

}
