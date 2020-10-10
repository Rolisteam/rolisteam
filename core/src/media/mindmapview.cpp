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

#include "qmlchat/avatarprovider.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>

#include "controller/view_controller/mindmapcontroller.h"

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
    engine->rootContext()->setContextProperty("_ctrl", m_ctrl);
    engine->addImageProvider("avatar", new AvatarProvider(m_ctrl->playerModel()));
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    // qrc:/qml/CustomItems/PermissionSlider.qml

    m_qmlViewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlViewer->setSource(QUrl("qrc:/resources/qml/main.qml"));

    auto wid= new QWidget(this);

    auto layout= new QVBoxLayout(wid);
    layout->setMargin(0);
    wid->setLayout(layout);

    layout->addWidget(m_qmlViewer.get());

    setWindowTitle(tr("%1 - Mindmap").arg(ctrl->name()));

    setWidget(wid);
}
