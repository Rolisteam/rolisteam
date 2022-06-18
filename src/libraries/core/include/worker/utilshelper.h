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
#ifndef UTILSHELPER_H
#define UTILSHELPER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <functional>

#include "media/mediatype.h"
#include "network/network_type.h"
#include <core_global.h>

namespace helper
{
namespace utils
{
CORE_EXPORT QString allSupportedImageFormatFilter();
CORE_EXPORT QRectF computerBiggerRectInside(const QRect& rect, qreal ratio);
CORE_EXPORT QPixmap roundCornerImage(const QPixmap& source, int size= 80, int radius= 8);
CORE_EXPORT bool isSquareImage(const QByteArray& array);
CORE_EXPORT bool hasValidCharacter(const std::vector<connection::CharacterData>& characters, bool isGameMaster);

CORE_EXPORT QStringList extentionPerType(Core::ContentType type, bool save, bool wildcard= false);
CORE_EXPORT QString filterForType(Core::ContentType, bool save);
CORE_EXPORT QString typeToIconPath(Core::ContentType);
CORE_EXPORT QString typeToString(Core::ContentType);
CORE_EXPORT Core::ContentType mediaTypeToContentType(Core::MediaType type);
CORE_EXPORT Core::ContentType extensionToContentType(const QString& filename);

template <typename T>
void setContinuation(QFuture<T> future, QObject* obj, std::function<void(T)> callback)
{
    auto watcher= new QFutureWatcher<T>();
    QObject::connect(watcher, &QFutureWatcher<T>::finished, obj, [watcher, callback]() {
        auto result= watcher->result();
        callback(result);
        delete watcher;
    });
    watcher->setFuture(future);
}

template <typename T>
void setParamIfAny(const QString& key, const std::map<QString, QVariant>& params, std::function<void(T)> setter)
{
    auto it= params.find(key);
    if(params.end() != it)
    {
        setter(it->second.value<T>());
    }
}

} // namespace utils
} // namespace helper

#endif // UTILSHELPER_H
