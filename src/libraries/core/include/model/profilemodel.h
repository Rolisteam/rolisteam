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
#ifndef CONNECTIONPROFILEMODEL_H
#define CONNECTIONPROFILEMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <memory>
#include <core_global.h>
class ConnectionProfile;
class CORE_EXPORT  ProfileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CustomRole
    {
        NameRole= Qt::UserRole + 1,
        IndexRole
    };
    Q_ENUM(CustomRole)
    /**
     * @brief ProfileModel
     */
    ProfileModel();
    /**
     * @brief ~ProfileModel
     */
    virtual ~ProfileModel();
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index, int role) const override;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void removeProfile(int idx);

    ConnectionProfile* getProfile(const QModelIndex&);

    int cloneProfile(int index);

    int indexOf(ConnectionProfile* tmp);
    ConnectionProfile* getProfile(int index);

    QHash<int, QByteArray> roleNames() const override;
public slots:
    /**
     * @brief ProfileModel::appendProfile
     */
    void appendProfile();
    /**
     * @brief append profile with param
     * @param profile
     */
    void appendProfile(ConnectionProfile* profile);

private:
    std::vector<std::unique_ptr<ConnectionProfile>> m_connectionProfileList;
};

#endif // CONNECTIONPROFILEMODEL_H
