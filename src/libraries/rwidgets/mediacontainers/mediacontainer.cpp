/***************************************************************************
 *	Copyright (C) 2015 by Renaud Guezennec                                 *
 *   http://www.renaudguezennec.eu/accueil,3.html                          *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "mediacontainer.h"

#include <QMdiArea>
#include <QMenu>
#include <QMessageBox>

MediaContainer::MediaContainer(MediaControllerBase* ctrl, ContainerType containerType, QWidget* parent)
    : QMdiSubWindow(parent), m_lifeCycleCtrl(ctrl), m_currentCursor(nullptr), m_containerType(containerType)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_detachedDialog= new QAction(tr("Detach the view"), this);
    m_detachedDialog->setCheckable(true);

    connect(m_detachedDialog, &QAction::triggered, this, &MediaContainer::detachView, Qt::QueuedConnection);
    connect(m_lifeCycleCtrl, &MediaControllerBase::closeContainer, this, &MediaContainer::close);
    connect(m_lifeCycleCtrl, &MediaControllerBase::ownerIdChanged, this, &MediaContainer::ownerIdChanged);
    connect(m_lifeCycleCtrl, &MediaControllerBase::nameChanged, this,
            [this]() { setWindowTitle(m_lifeCycleCtrl->name()); });
}

MediaContainer::~MediaContainer() {}

MediaControllerBase* MediaContainer::ctrl() const
{
    return m_lifeCycleCtrl;
}

void MediaContainer::error(QString err, QWidget* parent)
{
    if(nullptr != parent)
    {
        QMessageBox msgBox(parent);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QObject::tr("Loading error"));
        msgBox.move(QPoint(parent->width() / 2, parent->height() / 2) + QPoint(-100, -50));

        Qt::WindowFlags flags= msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        msgBox.setText(err);
        msgBox.exec();
    }
}

void MediaContainer::setVisible(bool b)
{
    if(nullptr != widget())
    {
        widget()->setVisible(b);
    }
    QMdiSubWindow::setVisible(b);
}

QString MediaContainer::mediaId() const
{
    return m_lifeCycleCtrl->uuid();
}

void MediaContainer::addActionToMenu(QMenu& menu)
{
    menu.addAction(m_detachedDialog);
}

void MediaContainer::hideEvent(QHideEvent*)
{
    emit visibleChanged(false);
}

void MediaContainer::showEvent(QShowEvent*)
{
    emit visibleChanged(true);
}

void MediaContainer::detachView(bool b)
{
    static QMdiArea* parent= mdiArea();
    if(b)
    {
        setParent(nullptr);
        setVisible(true);
    }
    else
    {
        // m_window->setParent(parent);
        if(nullptr != parent)
        {
            parent->addSubWindow(this);
        }
        setVisible(true);
    }
}
MediaContainer::ContainerType MediaContainer::getContainerType() const
{
    return m_containerType;
}

void MediaContainer::closeEvent(QCloseEvent* event)
{
    hide();
    event->accept();
}

void MediaContainer::setContainerType(const ContainerType& containerType)
{
    m_containerType= containerType;
}
QString MediaContainer::ownerId() const
{
    return m_lifeCycleCtrl ? m_lifeCycleCtrl->ownerId() : QString();
}

void MediaContainer::setOwnerId(const QString& ownerId)
{
    if(m_lifeCycleCtrl)
        m_lifeCycleCtrl->setOwnerId(ownerId);
}
