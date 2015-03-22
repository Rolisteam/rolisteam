/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
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


#ifndef LOCAL_PERSON_MODEL_H
#define LOCAL_PERSON_MODEL_H

#include "userlist/playerslistproxy.h"
#include "userlist/playersList.h"

/**
 * @brief The LocalPersonModel class
 */
class LocalPersonModel : public PlayersListProxyModel
{
    Q_OBJECT

    public:
        static LocalPersonModel & instance();
        
        QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
        QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

        QVariant data(const QModelIndex &index, int role) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

    protected:
        bool filterChangingRows(QModelIndex & parent, int & start, int & end);

    private:
        LocalPersonModel();
        PlayersList* m_playersList;
};

#endif
