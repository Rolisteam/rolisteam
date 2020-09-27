/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef NODESTYLEMODEL_H
#define NODESTYLEMODEL_H

#include <QAbstractListModel>
class NodeStyle;

class NodeStyleModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        ColorOne= Qt::UserRole + 1,
        ColorTwo,
        TextColor
    };
    explicit NodeStyleModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    NodeStyle* getStyle(int index) const;
private slots:
    void initStyles();

private:
    std::vector<NodeStyle*> m_styles;
};

#endif // NODESTYLEMODEL_H
