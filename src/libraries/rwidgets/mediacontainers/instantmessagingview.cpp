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
#include "instantmessagingview.h"
#include "ui_instantmessagingview.h"

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>

#include "qml_components/avatarprovider.h"

InstantMessagingView::InstantMessagingView(InstantMessagingController* ctrl, QWidget* parent)
    : QWidget(parent), m_ui(new Ui::InstantMessagingView), m_qmlViewer(new QQuickWidget()), m_ctrl(ctrl)
{
    m_ui->setupUi(this);

    auto engine= m_qmlViewer->engine();

    connect(engine, &QQmlEngine::warnings, this, [](const QList<QQmlError>& warnings) {
        for(const auto& warn : warnings)
        {
            qDebug() << warn.toString();
        }
    });
    qmlRegisterSingletonType<InstantMessagerManager>(
        "org.rolisteam.InstantMessaging", 1, 0, "InstantMessagerManager",
        [this, engine](QQmlEngine* qmlengine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(scriptEngine)
            if(qmlengine != engine)
                return {};
            static InstantMessagerManager manager;
            qmlengine->setObjectOwnership(&manager, QQmlEngine::CppOwnership);
            static bool initialized= false;
            if(!initialized)
            {
                manager.setCtrl(m_ctrl);
                initialized= true;
            }
            return &manager;
        });

    engine->addImageProvider("avatar", new AvatarProvider(m_ctrl->playerModel()));
    engine->addImportPath(QStringLiteral("qrc:/qml"));
    engine->addImportPath(QStringLiteral("qrc:/qml/rolistyle"));

    connect(m_qmlViewer.get(), &QQuickWidget::sceneGraphError, this,
            [](QQuickWindow::SceneGraphError, const QString& msg) { qDebug() << msg; });
    connect(engine, &QQmlEngine::warnings, this, [](const QList<QQmlError>& warnings) {
        for(const auto& error : warnings)
            qDebug() << error.toString();
    });
    setMinimumWidth(200);

    m_qmlViewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlViewer->setSource(QUrl("qrc:/qml/InstantMessaging/InstantMessagingMain.qml"));

    for(const auto& error : m_qmlViewer->errors())
        qDebug() << error.toString();

    auto layout= new QVBoxLayout();
    layout->setContentsMargins(QMargins());
    setLayout(layout);

    layout->addWidget(m_qmlViewer.get());

    setWindowTitle(tr("Instant Messaging"));
}

InstantMessagingView::~InstantMessagingView()
{
    delete m_ui;
}

void InstantMessagingView::closeEvent(QCloseEvent* event)
{
    hide();
    event->accept();
}

InstantMessagerManager::InstantMessagerManager(QObject* object) : QObject(object) {}

InstantMessagingController* InstantMessagerManager::ctrl() const
{
    return m_ctrl;
}

void InstantMessagerManager::setCtrl(InstantMessagingController* ctrl)
{
    if(ctrl == m_ctrl)
        return;
    m_ctrl= ctrl;
    emit ctrlChanged();
}
