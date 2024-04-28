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
#include "maincontroller.h"

#include "utils/iohelper.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QStyle>
#include <QtConcurrent>

QJsonObject openFile(const QUrl& url, MainController* ctrl)
{
    auto array= utils::IOHelper::loadFile(url.toLocalFile());
    if(!ctrl || array.isEmpty())
        return {};

    return IOHelper::textByteArrayToJsonObj(array);
}

bool writeFile(const QUrl& url, const QByteArray& array)
{
    return IOHelper::writeByteArrayIntoFile(url.toLocalFile(), array);
}

MainController::MainController(QObject* parent) : mindmap::MindMapControllerBase{false, {"mindmap"}, parent} {}

void MainController::openFile(const QUrl& file)
{
    setUrl(file);

    helper::utils::setContinuation<QJsonObject>(
        QtConcurrent::run([this, file]() { return ::openFile(file, this); }), this,
        [this](const QJsonObject& obj) { IOHelper::readMindmapControllerBase(this, obj); });
}

void MainController::saveFile()
{
    auto data = IOHelper::saveController(this);
    auto fileName = url();
    helper::utils::setContinuation<bool>(QtConcurrent::run([data, fileName]() { return ::writeFile(fileName, data); }), this, [](bool){});
}
