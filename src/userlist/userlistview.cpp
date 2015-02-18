#include "userlistview.h"
#include <QHeaderView>
#include "userlistmodel.h"
#include <userlistdelegate.h>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>

UserListView::UserListView(QWidget *parent) :
    QTreeView(parent)
{
    setHeaderHidden(true);
    m_delegate = new UserListDelegate(this);
    setItemDelegate(m_delegate);
    connect(this,SIGNAL(editCurrentItemColor()),this,SLOT(onEditCurrentItemColor()));
}
void UserListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    emit currentItemChanged(current);
}
void  UserListView::mouseDoubleClickEvent ( QMouseEvent * event)
{
    QModelIndex tmp = indexAt(event->pos());

    if(tmp.isValid())
    {
        event->pos();
        int depth = 1;
        while(tmp.parent().isValid())
        {
            depth++;

            tmp=tmp.parent();
        }
        if((depth*20<event->pos().x())&&((depth+1)*20>event->pos().x()))
        {
            emit editCurrentItemColor();
        }
        else
            QTreeView::mouseDoubleClickEvent(event);

    }
    else
        QTreeView::mouseDoubleClickEvent(event);
}
void UserListView::onEditCurrentItemColor()
{
    QModelIndex index= currentIndex();
    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());



    QColor color= QColorDialog::getColor(childItem->getPerson()->getColor(),this);
    qDebug() << color;
    childItem->getPerson()->setColor(color);

}
