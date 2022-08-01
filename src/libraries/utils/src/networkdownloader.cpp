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
#include "utils/networkdownloader.h"

#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>

NetworkDownloader::NetworkDownloader(const QUrl& url, QObject* parent) : QObject(parent), m_url(url)
{
    m_manager.reset(new QNetworkAccessManager());
    connect(m_manager.get(), &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        QByteArray data= reply->readAll();
        bool isImage= false;

        auto v= reply->header(QNetworkRequest::ContentTypeHeader).toString();
        if(v.startsWith("image"))
            isImage= true;

        emit finished(data, isImage);
    });
}

void NetworkDownloader::download()
{
    m_manager->get(QNetworkRequest(m_url));
}
