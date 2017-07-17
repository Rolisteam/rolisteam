/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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

#include "sharednotecontainer.h"

SharedNoteContainer::SharedNoteContainer()
    : m_edit(new SharedNote())
{
    m_edit->setId(getMediaId());
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    if(NULL!=m_edit)
    {
        m_title = m_edit->windowTitle();
    }

    setCleverUriType(CleverURI::TEXT);
    setWidget(m_edit);
    setWindowIcon(QIcon(":/notes.png"));
    m_edit->displaySharingPanel();
    connect(m_edit,SIGNAL(windowTitleChanged(QString)),this,SLOT(setFileName(QString)));


}
void SharedNoteContainer::setOwner(Player* player)
{
    m_edit->setOwner(player);
}

void SharedNoteContainer::updateNoteToAll()
{
    NetworkMessageWriter msg(NetMsg::SharedNoteCategory,NetMsg::updateTextAndPermission);
    msg.string8(m_mediaId);
    m_edit->updateDocumentToAll(&msg);
    msg.sendAll();
}

void SharedNoteContainer::setFileName(QString str)
{
    if(NULL!=m_uri)
    {
        m_uri->setUri(str);
    }
}

bool SharedNoteContainer::readFileFromUri()
{
    if((NULL==m_uri)||(NULL==m_edit))
    {
        return false;
    }
    if(m_uri->getUri().isEmpty())
    {
        QByteArray array =m_uri->getData();
        QDataStream in(&array,QIODevice::ReadOnly);
        readFromFile(in);
        return true;
    }
    else
    {
        QString uri = m_uri->getUri();
        return m_edit->loadFile(uri);
    }
    return false;
}

void SharedNoteContainer::saveMedia()
{
    if(NULL!=m_edit)
    {
       //m_edit->fileSave();
        QString uri = m_edit->windowTitle();
        m_uri->setUri(uri);
    }
}
void SharedNoteContainer::putDataIntoCleverUri()
{
    if(NULL!=m_edit)
    {
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        //m_edit->saveFileAsBinary(out);
        if(NULL!=m_uri)
        {
            m_uri->setData(data);
        }
    }
}
void SharedNoteContainer::readFromFile(QDataStream& data)
{
    if(NULL!=m_edit)
    {
        //m_edit->readFromBinary(data);
        m_title = m_edit->windowTitle();
    }
}

void SharedNoteContainer::saveInto(QDataStream &out)
{
    if(NULL!=m_edit)
    {
       // m_edit->saveFileAsBinary(out);
    }
}

