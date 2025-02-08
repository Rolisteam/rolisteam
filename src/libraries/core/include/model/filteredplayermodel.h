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
#ifndef FILTEREDPLAYERMODEL_H
#define FILTEREDPLAYERMODEL_H

#include <QQmlEngine>
#include <QSortFilterProxyModel>
#include <core_global.h>

namespace InstantMessaging
{
class CORE_EXPORT FilteredPlayerModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool allow READ allow WRITE setAllow NOTIFY allowChanged FINAL)
    Q_PROPERTY(QStringList recipiants READ recipiantIds WRITE setRecipiants NOTIFY recipiantsChanged FINAL)
public:
    explicit FilteredPlayerModel(const QStringList& list= QStringList(), QObject* parent= nullptr);

    QStringList recipiantIds() const;

    bool hasRecipiant(const QString& uuid);
    QString recipiantName(const QString& uuid);

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

    bool allow() const;
    void setAllow(bool newAllow);
    void setRecipiants(const QStringList& list);

public slots:
    void addRecipiants(const QString& id);
    void removeRecipiants(const QString& id);

signals:
    void allowChanged();
    void recipiantsChanged();
    void recipiantAdded(const QString& id);
    void recipiantRemoved(const QString& id);

private:
    QStringList m_participants;
    bool m_allow{true};
};
} // namespace InstantMessaging
#endif // FILTEREDPLAYERMODEL_H
