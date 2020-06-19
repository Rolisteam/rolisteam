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

#include <QQmlContext>
#include <QQmlEngine>
#include <QUrl>
#include <QVBoxLayout>

#include "avatarprovider.h"
#include "controller/instantmessagingcontroller.h"

InstantMessagingView::InstantMessagingView(InstantMessagingController* ctrl, QWidget* parent)
    : QWidget(parent), m_ui(new Ui::InstantMessagingView), m_qmlViewer(new QQuickWidget()), m_ctrl(ctrl)
{
    m_ui->setupUi(this);

    auto engine= m_qmlViewer->engine();
    engine->rootContext()->setContextProperty("_ctrl", m_ctrl);
    engine->addImageProvider("avatar", new AvatarProvider(m_ctrl->playerModel()));
    setMinimumWidth(200);

    m_qmlViewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlViewer->setSource(QUrl("qrc:/qmlchat/chatview.qml"));

    auto layout= new QVBoxLayout();
    layout->setMargin(0);
    setLayout(layout);

    layout->addWidget(m_qmlViewer.get());

    setWindowTitle(tr("Instant Messaging"));
}

InstantMessagingView::~InstantMessagingView()
{
    delete m_ui;
}
