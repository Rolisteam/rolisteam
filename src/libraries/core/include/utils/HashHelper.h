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
#ifndef HASHHELPER_H
#define HASHHELPER_H

#include "core_global.h"
#include <QColor>

CORE_EXPORT inline size_t qHash(const QColor& color, size_t seed)
{
    QtPrivate::QHashCombine hash;

    seed= hash(static_cast<uint>(color.red()), seed);
    seed= hash(static_cast<uint>(color.green()), seed);
    seed= hash(static_cast<uint>(color.blue()), seed);
    seed= hash(static_cast<uint>(color.alpha()), seed);
    return seed;
}

#endif // HASHHELPER_H
