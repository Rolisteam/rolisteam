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

MediaContainer::MediaContainer(AbstractMediaContainerController* ctrl, ContainerType containerType, bool localIsGM,
                               QWidget* parent)
    : QMdiSubWindow(parent)
    , m_lifeCycleCtrl(ctrl)
    , m_preferences(PreferencesManager::getInstance())
    , m_action(nullptr)
    , m_currentCursor(nullptr)
    , m_mediaId(QUuid::createUuid().toString())
    , m_remote(false)
    , m_localIsGM(localIsGM)
    , m_containerType(containerType)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_detachedDialog= new QAction(tr("Detach the view"), this);
    m_detachedDialog->setCheckable(true);

    connect(m_detachedDialog, SIGNAL(triggered(bool)), this, SLOT(detachView(bool)));
    connect(ctrl, &AbstractMediaContainerController::closeContainer, this, &MediaContainer::close);
    connect(m_lifeCycleCtrl, &AbstractMediaContainerController::titleChanged, this,
            [this]() { setWindowTitle(m_lifeCycleCtrl->title()); });
}

MediaContainer::~MediaContainer() {}

void MediaContainer::setLocalPlayerId(QString id)
{
    //  m_localPlayerId= id;
}

QString MediaContainer::getLocalPlayerId()
{
    return {}; // m_localPlayerId;
}
/*void MediaContainer::setCleverUri(CleverURI* uri)
{
    if(m_uri == uri)
        return;

    if(m_uri != nullptr)
    {
        delete m_uri;
        m_uri= nullptr;
    }
    m_uri= uri;
    if(nullptr != m_uri)
    {
        m_uri->setListener(this);
    }
    updateTitle();
}

CleverURI* MediaContainer::getCleverUri() const
{
    return m_uri;
}*/

AbstractMediaContainerController* MediaContainer::ctrl() const
{
    return m_lifeCycleCtrl;
}

bool MediaContainer::openMedia()
{
    return false;
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
bool MediaContainer::isUriEndWith(QString str)
{
    /* if(nullptr != m_uri)
     {
         if(m_uri->getUri().endsWith(str))
         {
             return true;
         }
     }*/
    return false;
}

void MediaContainer::setVisible(bool b)
{
    if(nullptr != widget())
    {
        widget()->setVisible(b);
        if(nullptr != m_action)
        {
            m_action->setChecked(b);
        }
    }
    /*  if(nullptr != m_uri)
      {
          m_uri->setDisplayed(b);
      }*/
    QMdiSubWindow::setVisible(b);
}
void MediaContainer::setAction(QAction* act)
{
    m_action= act;
    connect(m_action, &QAction::triggered, this, &MediaContainer::setVisible);
}
QAction* MediaContainer::getAction()
{
    return m_action;
}

void MediaContainer::currentColorChanged(QColor& penColor)
{
    m_penColor= penColor;
}

QString MediaContainer::getMediaId() const
{
    return m_mediaId;
}

QString MediaContainer::getUriName() const
{
    /* if(nullptr == m_uri)
         return m_name;

     return m_uri->name();*/
}

void MediaContainer::setUriName(const QString& name)
{
    /* if(nullptr != m_uri)
         m_uri->setName(name);*/
    m_name= name;
    if(nullptr != m_action)
        m_action->setText(getUriName());
    updateTitle();
}

void MediaContainer::setMediaId(QString str)
{
    m_mediaId= str;
}

void MediaContainer::cleverURIHasChanged(CleverURI* uri, CleverURI::DataValue field)
{
    /* if(uri != m_uri)
         return;

     if(field == CleverURI::NAME)
     {
         if(nullptr != m_action)
             m_action->setText(getUriName());
         updateTitle();
     }*/
}
void MediaContainer::currentToolChanged(Core::SelectableTool selectedtool)
{
    m_currentTool= selectedtool;
}
void MediaContainer::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor= cursor;
}

void MediaContainer::addActionToMenu(QMenu& menu)
{
    menu.addAction(m_detachedDialog);
}

void MediaContainer::fill(NetworkMessageWriter&) {}

void MediaContainer::readMessage(NetworkMessageReader&) {}

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
    return m_ownerId;
}

void MediaContainer::setOwnerId(const QString& ownerId)
{
    if(ownerId == m_ownerId)
        return;
    m_ownerId= ownerId;
    emit ownerIdChanged();
}

bool MediaContainer::getLocalIsGM() const
{
    return m_localIsGM;
}

void MediaContainer::setLocalIsGM(bool localIsGM)
{
    m_localIsGM= localIsGM;
}

bool MediaContainer::isRemote() const
{
    return m_remote;
}

void MediaContainer::setRemote(bool remote)
{
    m_remote= remote;
}
