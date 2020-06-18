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
#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include "messageinterface.h"
#include <QAbstractListModel>
#include <memory>

namespace InstantMessaging
{
class MessageInterface;
class MessageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
public:
    enum InnerRole
    {
        MessageTypeRole= Qt::UserRole + 1,
        TextRole,
        TimeRole,
        MessageRole,
        LocalRole,
        OwnerRole
    };
    explicit MessageModel(QObject* parent= nullptr);
    virtual ~MessageModel() override;

    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    QString localId() const;

public slots:
    void setLocalId(const QString& localid);
    void addMessage(const QString& text, const QDateTime& time, const QString& owner,
                    MessageInterface::MessageType type);
    void addMessageInterface(MessageInterface* msg);

signals:
    void messageAdded(MessageInterface* msg);
    void localIdChanged(QString);

private:
    std::vector<std::unique_ptr<MessageInterface>> m_messages;
    QString m_localId;
};
} // namespace InstantMessaging

#endif // MESSAGEMODEL_H
