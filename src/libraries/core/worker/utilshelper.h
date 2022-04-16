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

#include "core/media/mediatype.h"
#include "core/network/network_type.h"

namespace helper
{
namespace utils
{
QString allSupportedImageFormatFilter();
QRectF computerBiggerRectInside(const QRect& rect, qreal ratio);
QPixmap roundCornerImage(const QPixmap& source, int size= 80, int radius= 8);
bool isSquareImage(const QByteArray& array);
bool hasValidCharacter(const std::vector<connection::CharacterData>& characters, bool isGameMaster);

QStringList extentionPerType(Core::ContentType type, bool save, bool wildcard= false);
QString filterForType(Core::ContentType, bool save);
QString typeToIconPath(Core::ContentType);
QString typeToString(Core::ContentType);
Core::ContentType mediaTypeToContentType(Core::MediaType type);
Core::ContentType extensionToContentType(const QString& filename);

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
} // namespace utils
} // namespace helper

#endif // UTILSHELPER_H
