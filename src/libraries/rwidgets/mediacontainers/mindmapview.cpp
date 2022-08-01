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

#include "qml_components/avatarprovider.h"

#include <QJSValue>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQuickStyle>

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/model/nodeimageprovider.h"
#include <common_qml/theme.h>

MindMapView::MindMapView(MindMapController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::MindMapContainer, parent)
    , m_qmlViewer(new QQuickWidget())
    , m_ctrl(ctrl)
{
    // auto format= QSurfaceFormat::defaultFormat();
    auto format= m_qmlViewer->format();
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(4, 6);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    // format.setSamples(2);
    m_qmlViewer->setFormat(format);

    setObjectName("mindmap");
    setWindowIcon(QIcon::fromTheme("mindmap"));
    auto engine= m_qmlViewer->engine();

    auto selector= new QQmlFileSelector(engine, this);
    auto instance= customization::Theme::instance();
    connect(instance, &customization::Theme::folderChanged, this, [instance, selector]() {
        qDebug() << "change prefix" << instance->folder();
        selector->setExtraSelectors({instance->folder()});
    });

    engine->setOutputWarningsToStandardError(true);

    qmlRegisterSingletonType<MindmapManager>(
        "mindmap", 1, 0, "MindmapManager", [ctrl, engine](QQmlEngine* qmlengine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(scriptEngine)
            if(qmlengine != engine)
                return {};
            static QHash<QQmlEngine*, MindmapManager*> hash;

            if(!hash.contains(qmlengine))
            {
                auto m= new MindmapManager(ctrl);
                m->setCtrl(ctrl);
                qmlengine->setObjectOwnership(m, QQmlEngine::CppOwnership);
                hash.insert(qmlengine, m);
            }

            return hash.value(qmlengine);
        });
    if(!m_ctrl)
        return;
    engine->addImageProvider("avatar", new AvatarProvider(m_ctrl->playerModel()));
    engine->addImageProvider("nodeImages", new mindmap::NodeImageProvider(m_ctrl->imgModel()));
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    engine->addImportPath(QStringLiteral("qrc:/qml/rolistyle"));

    connect(engine, &QQmlEngine::warnings, this, [](const QList<QQmlError>& errors) {
        for(const auto& error : errors)
            qDebug() << "warnings: " << error;
    });
    connect(m_qmlViewer.get(), &QQuickWidget::sceneGraphError, this,
            [](QQuickWindow::SceneGraphError error, const QString& message) { qDebug() << message << error; });

    m_qmlViewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlViewer->setSource(QUrl("qrc:/resources/qml/main.qml"));

    auto const& errors= m_qmlViewer->errors();
    std::for_each(std::begin(errors), std::end(errors), [](const QQmlError& error) { qDebug() << error; });

    // m_qmlViewer.

    auto wid= new QWidget(this);

    auto layout= new QVBoxLayout(wid);
    layout->setContentsMargins(QMargins());
    wid->setLayout(layout);

    layout->addWidget(m_qmlViewer.get());

    connect(ctrl, &MindMapController::nameChanged, this,
            [this]() { setWindowTitle(tr("%1 - Mindmap").arg(m_ctrl->name())); });

    setWindowTitle(tr("%1 - Mindmap").arg(m_ctrl->name()));

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
