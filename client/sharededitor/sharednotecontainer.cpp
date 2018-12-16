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

SharedNoteContainer::SharedNoteContainer(bool localIsGM,QWidget* parent)
    : MediaContainer(localIsGM,parent), m_edit(new SharedNote())
{
    m_edit->setId(getMediaId());
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    setCleverUriType(CleverURI::SHAREDNOTE);
    setWidget(m_edit);
    setWindowIcon(QIcon(":/resources/icons/sharedEditor.png"));
    m_edit->setFileName(getUriName());

}
void SharedNoteContainer::readMessage(NetworkMessageReader& msg)
{
    if(nullptr!=m_edit)
    {
        m_edit->readFromMsg(&msg);
    }
}

void SharedNoteContainer::setMediaId(QString str)
{
    MediaContainer::setMediaId(str);
    m_edit->setId(getMediaId());
}
void SharedNoteContainer::runUpdateCmd(QString msg)
{
    if(nullptr!=m_edit)
    {
        m_edit->runUpdateCmd(msg);
    }
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
    msg.sendToServer();
}

bool SharedNoteContainer::readFileFromUri()
{
    if((nullptr==m_uri)||(nullptr==m_edit))
        return false;

    bool val = false;
    if(!m_uri->exists())
    {
        QByteArray array =m_uri->getData();
        QDataStream in(&array,QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_7);
        readFromFile(in);
        val = true;
    }
    else
    {
        QString uri = m_uri->getUri();
        QFile file(uri);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            val = m_edit->loadFileAsText(in);
        }
    }
    updateTitle();
    return val;
}

void SharedNoteContainer::saveMedia()
{
    if(nullptr!=m_edit)
    {
        if(nullptr!=m_uri)
        {
            QString filter = CleverURI::getFilterForType(m_uri->getType());
            filter = filter.remove(0,filter.indexOf("(")+1);
            filter = filter.remove(filter.indexOf(")"),1);
            filter = filter.replace('*',' ');
            QStringList list = filter.split(' ',QString::SkipEmptyParts);

            bool hasEnd=false;
            for(auto end : list)
            {
                if(m_uri->getUri().endsWith(end))
                {
                    hasEnd = true;
                }
            }
            if(!hasEnd)
            {
                QString str = m_uri->getUri()+".rsn";
                m_uri->setUri(str);
            }
            updateTitle();

            QFile file(m_uri->getUri());
            if (!file.open(QIODevice::WriteOnly))
            {
                return;
            }
            QTextStream out(&file);
            m_edit->saveFileAsText(out);
            file.close();
        }
    }
}

void SharedNoteContainer::updateTitle()
{
    m_edit->setFileName(getUriName());// update the title too
}

void SharedNoteContainer::putDataIntoCleverUri()
{
    if(nullptr!=m_edit)
    {
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_7);
        m_edit->saveFile(out);
        if(nullptr!=m_uri)
        {
            m_uri->setData(data);
        }
    }
}
void SharedNoteContainer::readFromFile(QDataStream& data)
{
    if(nullptr!=m_edit)
    {
        m_edit->loadFile(data);
    }
}

void SharedNoteContainer::saveInto(QDataStream &out)
{
    if(nullptr!=m_edit)
    {
        m_edit->saveFile(out);
    }
}
