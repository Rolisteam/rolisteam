/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                                 *
 *  renaud@rolisteam.org                                                   *
 *                                                                         *
 *   rmindmap is free software; you can redistribute it and/or modify      *
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

#include <QAbstractItemModel>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QSurfaceFormat>

#include "common_qml/theme.h"
#include "controller/view_controller/mindmapcontrollerbase.h"
#include "controller/view_controller/sidemenucontroller.h"
#include "maincontroller.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/data/minditem.h"
#include "mindmap/data/nodestyle.h"
#include "mindmap/data/positioneditem.h"
#include "mindmap/model/nodeimageprovider.h"
#include "mindmap/qmlItems/linkitem.h"
#include "utils/mappinghelper.h"

void registerMindmapType()
{
    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterUncreatableType<mindmap::MindMapControllerBase>("mindmapcpp", 1, 0, "MindMapController",
                                                               "MindMapController can't be created in qml");
    qmlRegisterUncreatableType<mindmap::MindItem>("mindmapcpp", 1, 0, "MindItem", "Enum only");
    qmlRegisterUncreatableType<RemotePlayerModel>("mindmapcpp", 1, 0, "RemotePlayerModel", "property values");
    qmlRegisterType<mindmap::LinkItem>("mindmapcpp", 1, 0, "MindLink");
    qmlRegisterType<mindmap::NodeStyle>("mindmapcpp", 1, 0, "NodeStyle");
    qmlRegisterType<mindmap::MindNode>("mindmapcpp", 1, 0, "MindNode");
    qmlRegisterType<mindmap::SideMenuController>("mindmapcpp", 1, 0, "SideMenuController");
}

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(rmindmap);
    Q_INIT_RESOURCE(rolisteam);
    // Q_INIT_RESOURCE(resources);

    app.setApplicationName(QStringLiteral("RMindMap"));
    app.setOrganizationName(QStringLiteral("Rolisteam"));
    app.setOrganizationDomain(QStringLiteral("org.rolisteam"));
    auto format= QSurfaceFormat::defaultFormat();
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");
    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption file(QStringList() << "f"
                                          << "file",
                            QObject::tr("open <file>."), "file");

    parser.addOption(file);
    parser.process(app.arguments());

    MainController main;

    registerMindmapType();
    qmlRegisterSingletonInstance<MainController>("mindmap", 1, 0, "MindmapManager", &main);

    QQmlApplicationEngine qmlEngine;
    QObject::connect(&qmlEngine, &QQmlApplicationEngine::objectCreated, &qmlEngine,
                     [](QObject* obj, const QUrl& url) { qDebug() << "object created: " << obj->objectName() << obj; });
    qmlEngine.addImportPath(QStringLiteral("qrc:/qml"));
    qmlEngine.addImportPath(QStringLiteral("qrc:/qml/rolistyle"));
    auto provider= new mindmap::NodeImageProvider(main.imgModel());
    qmlEngine.addImageProvider("nodeImages", provider);

    qmlEngine.loadFromModule("mindmapmod", "Main");

    if(parser.isSet(file))
    {
        main.openFile(QUrl::fromUserInput(parser.value(file)));
    }

    return app.exec();
}
