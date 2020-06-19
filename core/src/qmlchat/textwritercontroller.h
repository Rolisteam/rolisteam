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
#ifndef TEXTWRITERCONTROLLER_H
#define TEXTWRITERCONTROLLER_H

#include <QObject>

namespace InstantMessaging
{
class TextWriterController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    explicit TextWriterController(QObject *parent = nullptr);

    QString text() const;
    Q_INVOKABLE QString interpretedText() const;

public slots:
    void setText(const QString& text);
    void up();
    void down();
    void send();

signals:
    void textChanged(const QString& text);

private:
    QString m_text;
    QStringList m_history;
    int m_historicPostion = 0;
};
}

#endif // TEXTWRITERCONTROLLER_H
