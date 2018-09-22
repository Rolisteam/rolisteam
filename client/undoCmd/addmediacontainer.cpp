/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
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
#include "addmediacontainer.h"
#include "improvedworkspace.h"
#include "network/networkmessagewriter.h"
#include <QDebug>
AddMediaContainer::AddMediaContainer(MediaContainer* mediac,
                                     SessionManager* manager,
                                     QMenu* menu,
                                     MainWindow* main,
                                     ImprovedWorkspace* workspace,
                                     bool gm,
                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      m_media(mediac),
      m_manager(manager),
      m_menu(menu),
      m_main(main),
      m_mdiArea(workspace),
      m_gm(gm)
{
    setText(QObject::tr("Show %1").arg(mediac->getUriName()));
}

void AddMediaContainer::redo()
{
    qInfo() << QStringLiteral("Redo command AddMediaContainer: %1 ").arg(text());
    if(nullptr != m_media)
    {
        //add in workspace + add action and add into ressources manager.
        CleverURI* uri = m_media->getCleverUri();
        if(nullptr!=uri)
        {
            m_main->setLatestFile(uri);
            m_manager->addRessource(m_media->getCleverUri());
            uri->setDisplayed(true);
        }
        QAction* action= m_media->getAction();
        if(action == nullptr)
        {
            action = m_menu->addAction(m_media->getUriName());
            action->setCheckable(true);
            action->setChecked(true);
        }
        action->setVisible(true);
        m_media->setAction(action);
        m_mdiArea->addContainerMedia(m_media);
        m_media->setVisible(true);
        m_media->setFocus();

        if(sendAtOpening())
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
            msg.uint8(static_cast<quint8>(uri->getType()));
            m_media->fill(msg);
            msg.sendToServer();
        }
    }
}

void AddMediaContainer::undo()
{
    qInfo() << QStringLiteral("Undo command AddMediaContainer: %1 ").arg(text());
    //remove from workspace, action in menu and from resources manager.
    if(nullptr != m_media)
    {
        auto act = m_media->getAction();
        if(act)
            act->setVisible(false);
        m_manager->removeResource(m_media->getCleverUri());
        m_mdiArea->removeSubWindow(m_media);
        if(m_gm)
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::closeMedia);
            msg.string8(m_media->getMediaId());
            msg.sendToServer();
        }
    }
}
bool AddMediaContainer::sendAtOpening()
{
    if(m_media->isRemote())
    {
       return false;
    }
    auto uri = m_media->getCleverUri();
    if(nullptr == uri)
    {
        return false;
    }
    bool result = false;
    switch(uri->getType())
    {
    case CleverURI::PICTURE:
    case CleverURI::VMAP:
    case CleverURI::MAP:
    case CleverURI::ONLINEPICTURE:
        result = true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}
