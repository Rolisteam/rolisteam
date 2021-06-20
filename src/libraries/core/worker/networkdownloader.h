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
#ifndef NETWORKDOWNLOADER_H
#define NETWORKDOWNLOADER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>

class NetworkDownloader : public QObject
{
    Q_OBJECT
public:
    explicit NetworkDownloader(const QUrl& url, QObject *parent = nullptr);

    void download();
signals:
    void finished(QByteArray data);

private:
    QUrl m_url;
    std::unique_ptr<QNetworkAccessManager> m_manager;
};

#endif // NETWORKDOWNLOADER_H
