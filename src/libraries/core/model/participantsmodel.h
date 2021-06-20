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
#ifndef PARTICIPANTSMODEL_H
#define PARTICIPANTSMODEL_H

#include <QAbstractProxyModel>
#include <QObject>

#include <QJsonObject>

class Player;
class ParticipantsModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    enum Permission
    {
        ReadWrite,
        ReadOnly,
        Hidden,
        Permission_count
    };
    ParticipantsModel();
    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QModelIndex parent(const QModelIndex& child) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    QString ownerId() const;
    void setOwnerId(const QString& id);

    void saveModel(QJsonObject& root);

    ParticipantsModel::Permission getPermissionFor(Player* player);
    void loadModel(QJsonObject& root);

public slots:
    void promotePlayerToRead(const QModelIndex& index);
    void promotePlayerToReadWrite(const QModelIndex& index);
    void demotePlayerToRead(const QModelIndex& index);
    void demotePlayerToHidden(const QModelIndex& index);

    void promotePlayer(const QModelIndex& index);
    void demotePlayer(const QModelIndex& index);

private:
    QStringList m_readOnly;
    QStringList m_readWrite;
    QStringList m_hidden;

    QStringList m_permissionGroup;

    QString m_ownerId;
};

#endif // PARTICIPANTSMODEL_H
