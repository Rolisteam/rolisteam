/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "chatlistmodel.h"
#include "person.h"

ChatListModel::ChatListModel(QObject *parent) :
    QAbstractListModel(parent),m_dataList(NULL)
{
   // m_dataList = new QList<Person*>();
}
int ChatListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(!m_dataList)
        return 0;
    else
        return m_dataList->size();
}
QVariant ChatListModel::data(const QModelIndex &index, int role) const
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
void ChatListModel::setClients(QList<Person*>* tmp)
{
    m_dataList = tmp;
}
