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
#include <QMessageBox>


MediaContainer::MediaContainer(QWidget* parent)
    : QMdiSubWindow(parent),m_uri(nullptr),m_preferences(PreferencesManager::getInstance()),m_action(nullptr),m_currentCursor(nullptr),m_mediaId(QUuid::createUuid().toString()),m_remote(false)
{
    //m_preferences = ;
    setAttribute(Qt::WA_DeleteOnClose,false);
    m_detachedDialog  = new QAction(tr("Detach the view"),this);
    m_detachedDialog->setCheckable(true);

    connect(m_detachedDialog,SIGNAL(triggered(bool)),this, SLOT(detachView(bool)));

}
MediaContainer::~MediaContainer()
{
    if(nullptr != m_uri)
    {
        delete m_uri;
    }
}

void MediaContainer::setLocalPlayerId(QString id)
{
    m_localPlayerId = id;
}

QString MediaContainer::getLocalPlayerId()
{
    return m_localPlayerId;
}
void MediaContainer::setCleverUri(CleverURI* uri)
{
	m_uri = uri;
}
QString MediaContainer::getTitle() const
{
	return m_title;
}
CleverURI*  MediaContainer::getCleverUri() const
{
    return  m_uri;
}
bool MediaContainer::openMedia()
{
    return false;
}
void MediaContainer::error(QString err,QWidget* parent)
{
    if(nullptr!=parent)
    {
        QMessageBox msgBox(parent);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QObject::tr("Loading error"));
        msgBox.move(QPoint(parent->width()/2, parent->height()/2) + QPoint(-100, -50));

        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        msgBox.setText(err);
        msgBox.exec();
    }
}
bool MediaContainer::isUriEndWith(QString str)
{
    if(nullptr!=m_uri)
    {
        if(m_uri->getUri().endsWith(str))
        {
            return true;
        }
    }
    return false;
}

void MediaContainer::setTitle(QString str)
{
  m_title = str;
  setWindowTitle(m_title);
}
void MediaContainer::setVisible(bool b)
{
    if(nullptr!=widget())
    {
        widget()->setVisible(b);
        if(nullptr!=m_action)
        {
            m_action->setChecked(b);

        }
    }
    if(nullptr != m_uri)
    {
        m_uri->setDisplayed(b);
    }
    QMdiSubWindow::setVisible(b);
}
void MediaContainer::setAction(QAction* act)
{
    m_action = act;
    connect(m_action, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));
}
QAction* MediaContainer::getAction()
{
    return m_action;
}
void MediaContainer::setCleverUriType(CleverURI::ContentType type)
{
    m_uri = new CleverURI(m_title,"",type);
}
void MediaContainer::currentColorChanged(QColor& penColor)
{
    m_penColor = penColor;
}

QString MediaContainer::getMediaId()
{
    return m_mediaId;
}

void MediaContainer::setMediaId(QString str)
{
    m_mediaId = str;
}
void MediaContainer::currentToolChanged(VToolsBar::SelectableTool selectedtool)
{
    m_currentTool = selectedtool;
}
void MediaContainer::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor = cursor;
}
CleverURI::ContentType MediaContainer::getContentType()
{
    if(nullptr!=m_uri)
    {
        return m_uri->getType();
    }
    return CleverURI::NONE;
}
void MediaContainer::addActionToMenu(QMenu& menu)
{
    menu.addAction(m_detachedDialog);
}

QUndoStack* MediaContainer::getUndoStack() const
{
    return nullptr;
}

void MediaContainer::setUndoStack(QUndoStack* undoStack)
{

}

void MediaContainer::fill(NetworkMessageWriter &msg)
{

}

void MediaContainer::readMessage(NetworkMessageReader &msg)
{

}

void MediaContainer::detachView(bool b)
{
    static QMdiArea* parent = mdiArea();
    if(b)
    {
        setParent(nullptr);
        setVisible(true);
    }
    else
    {
        //m_window->setParent(parent);
        if(nullptr!=parent)
        {
            parent->addSubWindow(this);
        }
        setVisible(true);
    }
}

bool MediaContainer::isRemote() const
{
    return m_remote;
}

void MediaContainer::setRemote(bool remote)
{
    m_remote = remote;
}
