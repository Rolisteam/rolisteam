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
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    qmlRegisterType<utils::MappingHelper>("utils", 1, 0, "MappingHelper");
    qmlRegisterUncreatableType<mindmap::MindMapControllerBase>("mindmap", 1, 0, "MindMapController",
                                                               "MindMapController can't be created in qml");
    qmlRegisterUncreatableType<mindmap::MindItem>("mindmap", 1, 0, "MindItem", "Enum only");
    qmlRegisterType<mindmap::SelectionController>("mindmap", 1, 0, "SelectionController");
    qmlRegisterUncreatableType<RemotePlayerModel>("mindmap", 1, 0, "RemotePlayerModel", "property values");
    qmlRegisterType<mindmap::LinkItem>("mindmap", 1, 0, "MindLink");
    qmlRegisterType<mindmap::NodeStyle>("mindmap", 1, 0, "NodeStyle");
    qmlRegisterUncreatableType<mindmap::PositionedItem>("mindmap", 1, 0, "PositionedItem", "Enum only");
    qmlRegisterType<mindmap::SideMenuController>("mindmap", 1, 0, "SideMenuController");
    qmlRegisterUncreatableType<mindmap::MindItemModel>("mindmap", 1, 0, "MindItemModel",
                                                       "MindItemModel can't be created in qml");
}

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    // Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rmindmap);
    // Q_INIT_RESOURCE(resources);
    // Q_INIT_RESOURCE(mindmap);

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

    MainController main;

    registerMindmapType();
    qmlRegisterSingletonInstance<MainController>("mindmap", 1, 0, "MindmapManager", &main);

    QQmlApplicationEngine qmlEngine;
    qmlEngine.addImportPath(QStringLiteral("qrc:/qml"));
    qmlEngine.addImportPath(QStringLiteral("qrc:/qml/rolistyle"));
    auto provider= new mindmap::NodeImageProvider(main.imgModel());
    qmlEngine.addImageProvider("nodeImages", provider);

    qmlEngine.load(QLatin1String("qrc:/resources/qml/main.qml"));

    return app.exec();
}
