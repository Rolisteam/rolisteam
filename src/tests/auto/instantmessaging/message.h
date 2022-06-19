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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

class Refresh : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool refresh READ refresh WRITE setRefresh NOTIFY refreshChanged)
public:
    bool refresh() const;

public slots:
    void setRefresh(bool refresh);
signals:
    void refreshChanged();

private:
    bool m_refresh= false;
};

class Message : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment CONSTANT)
    Q_PROPERTY(QString command READ command CONSTANT)
    Q_PROPERTY(QString details READ details CONSTANT)
    Q_PROPERTY(QString result READ result CONSTANT)
    Q_PROPERTY(QString time READ time CONSTANT)
    Q_PROPERTY(QString ownerName READ ownerName CONSTANT)
    Q_PROPERTY(QString writerId READ writerId CONSTANT)

public:
    Message(QObject* parent= nullptr);

    QString comment() const;
    QString command() const;
    QString details() const;
    QString result() const;
    QString time() const;
    QString ownerName() const;
    QString writerId() const;

private:
    QString m_comment;
    QString m_command;
    QString m_details;
    QString m_result;
    QString m_time;
    QString m_owner;
    QString m_writerId;
};

class Model : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Message* message READ message CONSTANT)
public:
    Message* message() const { return new Message; }
};
#endif // MESSAGE_H
