#include "userlistdelegate.h"
#include "userlistmodel.h"
#include <QPainter>
#include <QDebug>
UserListDelegate::UserListDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}
void UserListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    if(p!=NULL)
    {
        painter->drawText(0,option.fontMetrics.height(),p->getPerson()->getName());
    }

}
QSize UserListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize returnValue;
    PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    if(p!=NULL)
    {
        returnValue.setWidth(option.fontMetrics.width(p->getPerson()->getName()));
        returnValue.setHeight(option.fontMetrics.height());

    }
    qDebug() << " height" << returnValue.height();
    return returnValue;
}
