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
#ifndef SERIALIZERHELPER_H
#define SERIALIZERHELPER_H

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

class MainController;
class QmlGeneratorController;
class FieldModel;
namespace SerializerHelper
{
namespace keys
{
const char* const pageCount{"pageCount"};
const char* const uuid{"uuid"};
const char* const background{"background"};
const char* const name{"name"};
const char* const type{"type"};
const char* const items{"items"};

const char* const data{"data"};
const char* const qml{"qml"};
const char* const headCode{"additionnalHeadCode"};
const char* const imports{"additionnalImport"};
const char* const fixedScale{"fixedScale"};
const char* const bottomCode{"additionnalBottomCode"};
const char* const flickable{"flickable"};

const char* const fonts{"fonts"};

} // namespace keys

QByteArray buildData(MainController* ctrl);
void fetchMainController(MainController* ctrl, const QJsonObject& jsonObj);

void fetchGeneratorController(QmlGeneratorController* ctrl, const QJsonObject& obj);
QJsonObject saveGeneratorController(QmlGeneratorController* ctrl);

QJsonObject saveFieldModel(FieldModel* model);
}; // namespace SerializerHelper

#endif // SERIALIZERHELPER_H
