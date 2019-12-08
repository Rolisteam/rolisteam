/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
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
#include "openmediacontroller.h"

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediacontrollerinterface.h"
#include "data/cleveruri.h"

#include <QDebug>

OpenMediaController::OpenMediaController(CleverURI* uri, MediaControllerInterface* ctrl, ContentController* contentCtrl,
                                         bool gm, const std::map<QString, QVariant>& args, QUndoCommand* parent)
    : QUndoCommand(parent), m_uri(uri), m_ctrl(ctrl), m_contentCtrl(contentCtrl), m_args(args), m_gm(gm)
{
    if(nullptr != uri)
        setText(QObject::tr("Show %1").arg(uri->name()));
}

void OpenMediaController::redo()
{
    qInfo() << QStringLiteral("Redo command OpenMediaController: %1 ").arg(text());
    if(nullptr == m_uri || m_ctrl.isNull() || m_contentCtrl.isNull())
        return;

    m_ctrl->openMedia(m_uri, m_args);
    m_contentCtrl->addContent(m_uri);
    // add in workspace + add action and add into ressources manager.
    /*CleverURI* uri= m_media->getCleverUri();
    if(nullptr != uri)
    {
        //        m_ctrl->addRessource(m_media->getCleverUri());
        uri->setDisplayed(true);
    }
    QAction* action= m_media->getAction();
    if(action == nullptr)
    {
        action= m_menu->addAction(m_media->getUriName());
        action->setCheckable(true);
        action->setChecked(true);
    }
    action->setVisible(true);
    m_media->setAction(action);
    m_mdiArea->addContainerMedia(m_media);
    m_media->setVisible(true);
    m_media->setFocus();*/

    /*    if(sendAtOpening())
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
            msg.uint8(static_cast<quint8>(uri->getType()));
            m_media->fill(msg);
            msg.sendToServer();
        }*/
}

void OpenMediaController::undo()
{
    qInfo() << QStringLiteral("Undo command AddMediaContainer: %1 ").arg(text());
    if(nullptr == m_uri || nullptr == m_ctrl)
        return;

    m_ctrl->closeMedia(m_uri->uuid());
    m_contentCtrl->removeContent(m_uri);
    // remove from workspace, action in menu and from resources manager.
    /*    if(nullptr != m_media)
        {
            auto act= m_media->getAction();
            if(act)
                act->setVisible(false);
            //  m_ctrl->removeResource(m_media->getCleverUri());
            m_mdiArea->removeSubWindow(m_media);
            if(m_gm)
            {
                NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::closeMedia);
                msg.string8(m_media->getMediaId());
                msg.sendToServer();
            }
        }*/
}

// bool OpenMediaController::sendAtOpening()
//{
/*if(m_media->isRemote())
{
    return false;
}
auto uri= m_media->getCleverUri();
if(nullptr == uri)
{
    return false;
}
bool result= false;
switch(uri->getType())
{
case CleverURI::PICTURE:
case CleverURI::VMAP:
case CleverURI::MAP:
case CleverURI::ONLINEPICTURE:
    result= true;
    break;
default:
    result= false;
    break;
}
return result;*/
//}
