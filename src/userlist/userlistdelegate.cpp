#include "userlistdelegate.h"
#include "userlistmodel.h"
#include <QPainter>
#include <QDebug>
UserListDelegate::UserListDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}
void UserListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    /*PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    if(p!=NULL)
    {
        QRect tmp = option.rect;
        tmp.setLeft(20);
        painter->drawText(tmp,p->getPerson()->getName());
        QBrush brush(p->getPerson()->getColor());
        painter->setBrush(brush);
        painter->fillRect(option.rect.x(),option.rect.y(),20,option.rect.height(),Qt::SolidPattern);

    }
    qDebug() << option.decorationSize << option.rect;*/
    QItemDelegate::paint(painter,option,index);

}
QSize UserListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    /*QSize returnValue;
    PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    if(p!=NULL)
    {
        returnValue.setWidth(option.fontMetrics.width(p->getPerson()->getName()));
        returnValue.setHeight(option.fontMetrics.height());

    }
    qDebug() << " height" << returnValue.height();*/
   // qDebug() << QItemDelegate::sizeHint(option,index);
    return QItemDelegate::sizeHint(option,index);
}
