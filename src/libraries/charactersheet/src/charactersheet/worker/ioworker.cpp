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
#include "charactersheet/worker/ioworker.h"

#include "charactersheet/include/charactersheet/controllers/section.h"

#include <QBuffer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

namespace IOWorker
{

QJsonObject saveCharaterSheetModel(CharacterSheetModel* model)
{
    auto section = model->getRootSection();
    if(!section)
        return {};

    QJsonObject res;
    section->save(res);
    return res;
}

void fetchCharacterSheetModel(CharacterSheetModel *model, const QJsonObject &object)
{
    auto section = model->getRootSection();
    if(!section)
        return;
    section->load(object);
}

QJsonArray saveImageModel(charactersheet::ImageModel* model)
{
    auto const& imgs= model->imageInfos();
    QJsonArray array;
    for(const auto& info : imgs)
    {
        QJsonObject oj;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        info.pixmap.save(&buffer, "PNG");
        oj["bin"]= QString(buffer.data().toBase64());
        oj["key"]= info.key;
        oj["isBg"]= info.isBackground;
        oj["filename"]= info.filename;
        array.append(oj);
    }
    return array;
}

void fetchImageModel(charactersheet::ImageModel* model, const QJsonArray& array)
{
    for(const auto& imgVal : array)
    {
        auto imgInfo= imgVal.toObject();
        auto imgData= imgInfo["bin"].toString();
        auto imgKey= imgInfo["key"].toString();
        auto imgIsBg= imgInfo["isBg"].toBool();
        auto filename= imgInfo["filename"].toString();

        auto data= QByteArray::fromBase64(imgData.toLocal8Bit());
        QPixmap map;
        map.loadFromData(data, "PNG");

        model->insertImage(map, imgKey, filename, imgIsBg);
    }
}

void saveFile(const QByteArray& data, const QString& filepath)
{
    if(data.isEmpty())
        return;
    QSaveFile file(filepath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.commit();
    }
}

QJsonObject readFileToObject(const QString& filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return {};

    QJsonDocument json= QJsonDocument::fromJson(file.readAll());
    return json.object();
}



} // namespace IOWorker
