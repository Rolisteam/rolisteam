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
#ifndef FILTERINSTANTMESSAGINGMODEL_H
#define FILTERINSTANTMESSAGINGMODEL_H

#include <QSortFilterProxyModel>

namespace InstantMessaging
{
class ChatRoom;
class FilterInstantMessagingModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
public:
    explicit FilterInstantMessagingModel(QObject* parent= nullptr);

    QString uuid() const;
    bool all() const;

    void setFilterParameter(bool b, QStringList data);

    Q_INVOKABLE QVariant get(int index);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    bool m_allBut= true;
    QStringList m_filteredId;
    QString m_uuid;
};

} // namespace InstantMessaging
#endif // FILTERINSTANTMESSAGINGMODEL_H
