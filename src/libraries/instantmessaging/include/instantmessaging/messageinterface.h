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
#ifndef MESSAGEINTERFACE_H
#define MESSAGEINTERFACE_H

#include "im_global.h"

#include <QDateTime>
#include <QObject>
#include <QUrl>

namespace InstantMessaging
{
class IM_EXPORT MessageInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MessageType type READ type CONSTANT)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString owner READ owner CONSTANT)   // Player only
    Q_PROPERTY(QString writer READ writer CONSTANT) // player or one of its character.
    Q_PROPERTY(QDateTime dateTime READ dateTime CONSTANT)
    Q_PROPERTY(QUrl imageLink READ imageLink WRITE setImageLink NOTIFY imageLinkChanged)
public:
    enum MessageType
    {
        Text,
        Dice,
        Command,
        Error,
    };
    Q_ENUM(MessageType)
    explicit MessageInterface(QObject* parent= nullptr);

    virtual MessageType type() const= 0;
    virtual QString text() const= 0;
    virtual QString owner() const= 0;
    virtual QDateTime dateTime() const= 0;
    virtual QString writer() const= 0;
    virtual QUrl imageLink() const= 0;

public slots:
    virtual void setText(const QString& text)= 0;
    virtual void setImageLink(const QUrl& path)= 0;

signals:
    void textChanged();
    void imageLinkChanged();
};

class IM_EXPORT MessageBase : public MessageInterface
{
    Q_OBJECT
public:
    explicit MessageBase(const QString& owner, const QString& writer, const QDateTime& time, MessageType type,
                         QObject* parent= nullptr);
    QString owner() const override;
    QDateTime dateTime() const override;
    MessageType type() const override;
    QString writer() const override;
    QUrl imageLink() const override;

public slots:
    void setImageLink(const QUrl& path) override;

private:
    QString m_ownerId;
    QDateTime m_time;
    MessageType m_type;
    QString m_writer;
    QUrl m_imageLink;
};
} // namespace InstantMessaging
#endif // MESSAGEINTERFACE_H
