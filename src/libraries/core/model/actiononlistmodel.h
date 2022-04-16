/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#ifndef ACTIONONLISTMODEL_H
#define ACTIONONLISTMODEL_H

#include <QAbstractListModel>

struct ActionInfo
{
    QString name;
    QString icon;
};

struct DataInfo
{
    QString data;
    int indexAction= -1;
};

class ActionOnListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Action_Role
    {
        Name= Qt::UserRole + 1,
        Action,
        PossibleAction,
        PossibleIcon,
    };

    explicit ActionOnListModel(const QStringList& data, const QList<ActionInfo>& actions, const QString& root,
                               QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool canValidate() const;

    void setAction(int index, int action);

    const QList<DataInfo>& dataset() const;

private:
    QList<DataInfo> m_data;
    QList<ActionInfo> m_actions;
    QString m_root;
};

#endif // ACTIONONLISTMODEL_H
