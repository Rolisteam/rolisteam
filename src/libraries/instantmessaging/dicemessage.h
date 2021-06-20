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
#ifndef DICEMESSAGE_H
#define DICEMESSAGE_H

#include <QJsonObject>
#include <QObject>

#include "messageinterface.h"

namespace InstantMessaging
{
class DiceMessage : public MessageBase
{
    Q_OBJECT
    Q_PROPERTY(QString result READ result CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QString command READ command CONSTANT)
    Q_PROPERTY(QString details READ details CONSTANT)
public:
    DiceMessage(const QString& ownerId, const QString& writer, const QDateTime& time, QObject* parent= nullptr);

    QString text() const override;
    QString command() const;
    QString details() const;
    QString result() const;
    QString comment() const;

public slots:
    void setText(const QString& text) override;

private:
    void computeResult();

private:
    QString m_text;
    QString m_comment;
    QString m_command;
    QString m_details;
    QString m_result;
};

} // namespace InstantMessaging
#endif // DICEMESSAGE_H
