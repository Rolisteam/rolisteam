#include "userlistview.h"
#include <QHeaderView>
#include "userlistmodel.h"
#include <userlistdelegate.h>
UserListView::UserListView(QWidget *parent) :
    QTreeView(parent)
{
    setHeaderHidden(true);
    m_delegate = new UserListDelegate(this);
    setItemDelegate(m_delegate);
}
void UserListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    emit currentItemChanged(current);
}
