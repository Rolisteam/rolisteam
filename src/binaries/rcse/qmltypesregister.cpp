/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "qmltypesregister.h"

#include <QQmlEngine>
#include <diceparser_qobject/diceroller.h>

#include "controllers/rcseapplicationcontroller.h"

#ifndef UNIT_TEST
#include "charactersheet/controllers/fieldcontroller.h"
#endif

void registerQmlTypes()
{
    qmlRegisterType<DiceRoller>("Rolisteam", 1, 0, "DiceRoller");
#ifndef UNIT_TEST
    qmlRegisterType<FieldController>("Rolisteam", 1, 0, "Field");
#endif
}
