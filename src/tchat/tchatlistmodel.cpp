#include "tchatlistmodel.h"
#include "person.h"

TchatListModel::TchatListModel(QObject *parent) :
    QAbstractListModel(parent),m_dataList(NULL)
{
   // m_dataList = new QList<Person*>();
}
int TchatListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(!m_dataList)
        return 0;
    else
        return m_dataList->size();
}
QVariant TchatListModel::data(const QModelIndex &index, int role) const
{

    if((!index.isValid())||(!m_dataList))
        return QVariant();

    if(index.row()<m_dataList->size())
    {
        switch(role)
        {
            case Qt::DisplayRole :
                return m_dataList->at(index.row())->getName();
            default:
                return QVariant();
        }
    }
    return QVariant();
}
void TchatListModel::setClients(QList<Person*>* tmp)
{
    m_dataList = tmp;
}
