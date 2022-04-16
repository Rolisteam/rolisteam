/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef CHARACTERDATAMODEL_H
#define CHARACTERDATAMODEL_H

#include "core/network/network_type.h"
#include <QAbstractTableModel>
#include <QPointer>

class ConnectionProfile;
class CharacterDataModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role
    {
        None= Qt::UserRole + 1,
        Avatar,
        Color,
        AvatarData,
        Name
    };
    explicit CharacterDataModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data
    connection::CharacterData character(int i) const;

    QHash<int, QByteArray> roleNames() const override;

public slots:
    bool insertCharacter();
    void setProfile(ConnectionProfile* profile);
    bool addCharacter(const connection::CharacterData& data);
    bool removeCharacter(int index);

    void setAvatar(int i, const QByteArray& img);
    void setName(int i, const QString& string);
    void setColor(int i, const QColor& color);

private:
    QPointer<ConnectionProfile> m_profile;
};

#endif // CHARACTERDATAMODEL_H
