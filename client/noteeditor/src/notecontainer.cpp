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

#include "notecontainer.h"

NoteContainer::NoteContainer(bool localIsGM,QWidget* parent)
    : MediaContainer(localIsGM,parent), m_edit(new TextEdit())
{
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    setCleverUriType(CleverURI::TEXT);
    setWidget(m_edit);
    setWindowIcon(QIcon(":/notes.png"));
    connect(m_edit,SIGNAL(fileNameChanged(QString)),this,SLOT(setFileName(QString)));
}

void NoteContainer::setFileName(QString str)
{
    if(nullptr!=m_uri)
    {
        m_uri->setUri(str);
    }
    setWindowModified(false);
    updateTitle();
}

void NoteContainer::updateTitle()
{
    QString showName;
    if (nullptr == m_uri || m_uri->name().isEmpty())
        showName = "untitled.txt";
    else
        showName = m_uri->name();
    setWindowTitle(tr("%1[*] - (Notes)").arg(showName));
}

bool NoteContainer::readFileFromUri()
{
    if((nullptr==m_uri)||(nullptr==m_edit))
    {
        return false;
    }
    bool val = false;
    if(!m_uri->exists())
    {
        QByteArray array =m_uri->getData();
        QDataStream in(&array,QIODevice::ReadOnly);
        readFromFile(in);
        val = true;
    }
    else
    {
        val = m_edit->load(m_uri->getUri());
    }
    updateTitle();
    return val;
}

void NoteContainer::saveMedia()
{
    if(nullptr!=m_edit)
    {
        m_edit->fileSave();
        QString uri = m_edit->getFileName();
        m_uri->setUri(uri);
    }
}
void NoteContainer::putDataIntoCleverUri()
{
    if(nullptr!=m_edit)
    {
        QByteArray data;
        QDataStream out(&data,QIODevice::WriteOnly);
        m_edit->saveFileAsBinary(out);
        if(nullptr!=m_uri)
        {
            m_uri->setData(data);
        }
    }
}

void NoteContainer::setTitle(QString str)
{
    if(nullptr!=m_edit)
    {
        m_edit->setCurrentFileName(str);
    }
}
void NoteContainer::readFromFile(QDataStream& data)
{
    if(nullptr!=m_edit)
    {
        m_edit->readFromBinary(data);
    }
}

void NoteContainer::saveInto(QDataStream &out)
{
    if(nullptr!=m_edit)
    {
        m_edit->saveFileAsBinary(out);
    }
}

