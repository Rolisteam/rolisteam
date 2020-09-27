/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef LINKMODEL_H
#define LINKMODEL_H

#include <QAbstractItemModel>

#include "data/link.h"

class LinkModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles : int
    {
        // Curves
        Direction= Qt::UserRole + 1,
        Position,
        Last,
        Width,
        Height,
        StartBoxRole,
        EndBoxRole,
        LinkRole,
        Label
    };
    explicit LinkModel(QObject* parent= nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    std::vector<Link*>& getDataSet();

    Link* addLink(MindNode* p1, MindNode* p2);
    void openLinkAndChildren(const QString& id, bool status);

    void clear();

    void append(Link* link);
    void removeLink(Link* link);
public slots:
    void linkHasChanged();

private:
    std::vector<Link*> m_data;
};

#endif // LINKMODEL_H
