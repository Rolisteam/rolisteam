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
#ifndef IOWORKER_H
#define IOWORKER_H

#include <charactersheet/charactersheet_global.h>
#include <charactersheet/imagemodel.h>
#include <charactersheet/charactersheetmodel.h>

#include <QJsonArray>

namespace IOWorker
{
// image model
CHARACTERSHEET_EXPORT QJsonArray saveImageModel(charactersheet::ImageModel* model);
CHARACTERSHEET_EXPORT void fetchImageModel(charactersheet::ImageModel* model, const QJsonArray& array);

CHARACTERSHEET_EXPORT void saveFile(const QByteArray& data, const QString& filepath);
CHARACTERSHEET_EXPORT QJsonObject readFileToObject(const QString& file);

CHARACTERSHEET_EXPORT QJsonObject saveCharaterSheetModel(CharacterSheetModel* model);
CHARACTERSHEET_EXPORT void fetchCharacterSheetModel(CharacterSheetModel* model, const QJsonObject& object);


}; // namespace IOWorker

#endif // IOWORKER_H
