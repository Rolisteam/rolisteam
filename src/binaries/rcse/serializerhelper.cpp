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
#include "serializerhelper.h"

#include "controllers/maincontroller.h"
#include "controllers/qmlgeneratorcontroller.h"

#include "charactersheet/worker/ioworker.h"

#include <QFontDatabase>

namespace SerializerHelper
{

QByteArray buildData(rcse::MainController* ctrl, const QString& fileName)
{
    QFile file(fileName);

    QJsonDocument json;
    QJsonObject obj;

    if(file.open(QIODevice::ReadOnly))
    {
        json = QJsonDocument::fromJson(file.readAll());
        obj = json.object();
    }

    obj = saveGeneratorController(obj, ctrl->generatorCtrl());

    obj[keys::pageCount]= static_cast<int>(ctrl->editCtrl()->pageCount());
    obj[keys::uuid]= ctrl->imageCtrl()->uuid();

    // background
    auto img= IOWorker::saveImageModel(ctrl->imageCtrl()->model());
    obj[keys::background]= img;

    ctrl->characterCtrl()->save(obj);
    json.setObject(obj);

    return json.toJson();
}
QJsonObject saveFieldModel(FieldModel* model)
{
    QJsonObject obj;
    auto root= model->getRootSection();
    root->save(obj, false);
    return obj;
}

void fetchGeneratorController(QmlGeneratorController* ctrl, const QJsonObject& obj)
{
    ctrl->setHeadCode(obj["additionnalHeadCode"].toString(""));
    if(ctrl->headCode().isEmpty())
        ctrl->setHeadCode(obj["additionnalCode"].toString(""));
    ctrl->setImportCode(obj["additionnalImport"].toString(""));
    ctrl->setFixedScale(obj["fixedScale"].toDouble(1.0));
    ctrl->setBottomCode(obj["additionnalBottomCode"].toString(""));
    QString qml= obj[keys::qml].toString();
    ctrl->setQmlCode(qml);

    const auto fonts= obj["fonts"].toArray();
    QStringList fontNames;
    for(const auto obj : fonts)
    {
        const auto font= obj.toObject();
        const auto fontData= QByteArray::fromBase64(font["data"].toString("").toLatin1());
        QFontDatabase::addApplicationFontFromData(fontData);

        fontNames << font["name"].toString();
    }
    ctrl->setFonts(fontNames);
    auto model= ctrl->fieldModel();

    model->load(obj["data"].toObject());
}

void fetchMainController(rcse::MainController* ctrl, const QJsonObject& jsonObj)
{
    ctrl->cleanUpData(false);
    QJsonObject data= jsonObj[keys::data].toObject();

    int pageCount= jsonObj[keys::pageCount].toInt();
    ctrl->imageCtrl()->setUuid(jsonObj[keys::uuid].toString(QUuid::createUuid().toString(QUuid::WithoutBraces)));

    // ui->m_codeEdit->setPlainText(qml);

    QJsonArray images= jsonObj["background"].toArray();

    QJsonArray backGround;
    std::copy_if(images.begin(), images.end(), std::back_inserter(backGround), [](const QJsonValue& val) {
        auto obj= val.toObject();
        return obj["isBg"].toBool();
    });

    QJsonArray regularImage;
    std::copy_if(images.begin(), images.end(), std::back_inserter(regularImage), [](const QJsonValue& val) {
        auto obj= val.toObject();
        return !obj["isBg"].toBool();
    });

    for(auto img : qAsConst(regularImage))
    {
        auto oj= img.toObject();
        QPixmap pix;
        QString str= oj["bin"].toString();
        QString path= oj["filename"].toString();
        QByteArray array= QByteArray::fromBase64(str.toUtf8());

        pix.loadFromData(array);
        ctrl->imageCtrl()->addImage(pix, path);
    }

    QPixmap refBgImage;
    for(int i= 0; i < pageCount; ++i)
    {
        QString path;
        QString id;
        QPixmap pix;
        if(i < backGround.size())
        {
            auto oj= backGround[i].toObject();
            QString str= oj["bin"].toString();
            id= oj["key"].toString();
            if(oj.contains("filename"))
                path= oj["filename"].toString();
            QByteArray array= QByteArray::fromBase64(str.toUtf8());

            pix.loadFromData(array);
            if(refBgImage.isNull())
                refBgImage= pix;
        }
        else
        {
            if(refBgImage.isNull())
                refBgImage= QPixmap(800, 600);
            pix= refBgImage;
        }
        auto idx= ctrl->editCtrl()->addPage();
        if(!backGround.isEmpty())
        {
            auto model= ctrl->imageCtrl()->model();
            model->insertImage(pix, id, path, false); //->addBackgroundImage(idx, pix, path, id);
            ctrl->editCtrl()->setImageBackground(idx, pix, path);
        }
    }

    fetchGeneratorController(ctrl->generatorCtrl(), jsonObj);
    ctrl->characterCtrl()->setRootSection(ctrl->generatorCtrl()->fieldModel()->getRootSection());
    ctrl->characterCtrl()->load(jsonObj, false);

    auto model= ctrl->generatorCtrl()->fieldModel();
    auto const& allChildren= model->allChildren();
    for(auto child : allChildren)
    {
        ctrl->editCtrl()->addFieldItem(child);
    }
}

QJsonObject& saveGeneratorController(QJsonObject& obj, QmlGeneratorController* ctrl)
{
    obj[keys::data]= saveFieldModel(ctrl->fieldModel());

    // qml file
    obj[keys::qml]= ctrl->qmlCode();
    obj[keys::headCode]= ctrl->headCode();
    obj[keys::imports]= ctrl->importCode();
    obj[keys::fixedScale]= ctrl->fixedScale();
    obj[keys::bottomCode]= ctrl->bottomCode();

    QJsonArray fonts;
    auto embeddedFont= ctrl->fonts();
    for(const QString& fontUri : embeddedFont)
    {
        QFile file(fontUri);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonObject font;
            font[keys::name]= fontUri;
            QByteArray array= file.readAll();
            font[keys::data]= QString(array.toBase64());
            fonts.append(font);
        }
    }
    obj[keys::fonts]= fonts;
    return obj;
}

} // namespace SerializerHelper
