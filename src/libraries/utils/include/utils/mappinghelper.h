/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef MAPPINGHELPER_H
#define MAPPINGHELPER_H

#include "utils/utils_global.h"
#include <QObject>
#include <QRectF>
#include <QSizeF>
#include <QQmlEngine>

namespace utils
{
class UTILS_EXPORT MappingHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit MappingHelper(QObject* parent= nullptr);

    ///
    /// \brief mapSizeTo compute a size that fits into maxSize but have the ratio of currentSize.
    /// \param maxSize
    /// \param currentSize
    ///
    Q_INVOKABLE QSizeF mapSizeTo(const QSizeF& maxSize, const QSizeF& currentSize);
    Q_INVOKABLE QRectF maxRect(const QRectF& parentRect, const QRectF& childrenRect);

    Q_INVOKABLE QRectF mapRectInto(const QRectF& maxRect, const QRectF& viewRect, const QRectF& imageRect);
};
} // namespace utils
#endif // MAPPINGHELPER_H
