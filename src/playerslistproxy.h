/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#ifndef PLAYERS_LIST_PROXY_MODEL_H
#define PLAYERS_LIST_PROXY_MODEL_H

#include <QAbstractProxyModel>

/**
 * @brief You onbly need to implement :
 * Qt::ItemFlags flags(const QModelIndex &index) const;
 * QVariant data(const QModelIndex &index, int role) const;
 * bool setData(const QModelIndex &index, const QVariant &value, int role);
 */
class PlayersListProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

    public:
        PlayersListProxyModel(QObject * parent = 0);

        QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
        QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

    protected:
        virtual bool filterChangingRows(QModelIndex & parent, int & start, int & end);

    private:
        bool m_rowsAboutToBeInserted;
        bool m_rowsAboutToBeRemoved;

    private slots:
        void p_rowsAboutToBeInserted(const QModelIndex & parent, int start, int end);
        void p_rowsInserted();
        void p_rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
        void p_rowsRemoved();
        void p_dataChanged(const QModelIndex & from, const QModelIndex & to);
};

#endif
