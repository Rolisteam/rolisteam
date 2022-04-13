/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "mindmapview.h"

#include "instantmessaging/avatarprovider.h"

#include <QJSValue>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/src/model/nodeimageprovider.h"

MindMapView::MindMapView(MindMapController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::MindMapContainer, parent)
    , m_qmlViewer(new QQuickWidget())
    , m_ctrl(ctrl)
{
    auto format= QSurfaceFormat::defaultFormat();
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    m_qmlViewer->setFormat(format);

    setObjectName("mindmap");
    setWindowIcon(QIcon::fromTheme("mindmap"));
    auto engine= m_qmlViewer->engine();

    qmlRegisterSingletonType<MindmapManager>(
        "org.rolisteam.mindmap", 1, 0, "MindMapManager",
        [ctrl, engine](QQmlEngine* qmlengine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(scriptEngine)
            if(qmlengine != engine)
                return {};
            static MindmapManager manager;
            static bool initialized= false;
            if(!initialized)
            {
                manager.setCtrl(ctrl);
                initialized= true;
            }
            qmlengine->setObjectOwnership(&manager, QQmlEngine::CppOwnership);
            return &manager;
        });

    // engine->rootContext()->setContextProperty("_ctrl", m_ctrl);
    engine->addImageProvider("avatar", new AvatarProvider(m_ctrl->playerModel()));
    engine->addImageProvider("nodeImages", new mindmap::NodeImageProvider(m_ctrl->imageModel()));
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    // qrc:/qml/CustomItems/PermissionSlider.qml

    m_qmlViewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlViewer->setSource(QUrl("qrc:/resources/qml/main.qml"));

    auto wid= new QWidget(this);

    auto layout= new QVBoxLayout(wid);
    layout->setContentsMargins(QMargins());
    wid->setLayout(layout);

    layout->addWidget(m_qmlViewer.get());

    setWindowTitle(tr("%1 - Mindmap").arg(ctrl->name()));

    setWidget(wid);
}

MindmapManager::MindmapManager(QObject* parent) : QObject(parent) {}

MindMapController* MindmapManager::ctrl() const
{
    return m_ctrl;
}

void MindmapManager::setCtrl(MindMapController* ctrl)
{
    if(m_ctrl == ctrl)
        return;
    m_ctrl= ctrl;
    emit ctrlChanged();
}
