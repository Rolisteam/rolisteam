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
#ifndef LOGCATEGORIES_H
#define LOGCATEGORIES_H
#include "utils/utils_global.h"

#include <QLoggingCategory>

namespace logCategory
{
UTILS_EXPORT const QLoggingCategory map("map");
UTILS_EXPORT const QLoggingCategory campaign("campaign");
UTILS_EXPORT const QLoggingCategory pdf("pdf");
UTILS_EXPORT const QLoggingCategory img("image");
UTILS_EXPORT const QLoggingCategory mindmap("mindmap");
UTILS_EXPORT const QLoggingCategory im("instantmessaging");
UTILS_EXPORT const QLoggingCategory sheet("charactersheet");
UTILS_EXPORT const QLoggingCategory server("server");
UTILS_EXPORT const QLoggingCategory rcse("rcse");
UTILS_EXPORT const QLoggingCategory rolisteam("rolisteam");
UTILS_EXPORT const QLoggingCategory network("network");
}; // namespace logCategory

#endif // LOGCATEGORIES_H
