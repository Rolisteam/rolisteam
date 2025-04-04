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
#include <qml_components/avatarprovider.h>

#include <data/person.h>
#include <model/playermodel.h>

#include "utils/iohelper.h"

AvatarProvider::AvatarProvider(PlayerModel* model)
    : QQuickImageProvider(QQmlImageProviderBase::Image)
    , m_playerModel(model)
    , m_default(":/resources/rolistheme/contact.svg")
{
}

QImage AvatarProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(size)
    auto resize= [requestedSize](const QImage& source)
    { return source.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation); };

    if(id.isEmpty() || m_playerModel.isNull())
        return resize(m_default);

    auto person= m_playerModel->personById(id);

    if(!person)
    {
        auto idUrl= QUrl::fromPercentEncoding(id.toLatin1());
        person= m_playerModel->personById(idUrl);
        if(!person)
            return resize(m_default);
    }

    auto img= utils::IOHelper::dataToImage(person->avatar());

    if(img.isNull())
        img= m_default;

    return requestedSize.isValid() ? resize(img) : img;
}
