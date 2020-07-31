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

#include "controller/view_controller/notecontroller.h"

NoteContainer::NoteContainer(NoteController* note, QWidget* parent)
    : MediaContainer(note, MediaContainer::ContainerType::NoteContainer, parent)
    , m_noteCtrl(note)
    , m_edit(new TextEdit(note))
{
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    setWidget(m_edit);
    setWindowIcon(QIcon::fromTheme("notes"));
    connect(m_edit, &TextEdit::fileNameChanged, this, &NoteContainer::setFileName);

    auto func= [this]() { setWindowTitle(tr("%1 - Note").arg(m_noteCtrl->name())); };
    connect(m_noteCtrl, &NoteController::nameChanged, this, func);
    func();
}

void NoteContainer::setFileName(QString str)
{
    /*if(nullptr != m_uri)
    {
        m_uri->setUri(str);
    }*/
    setWindowModified(false);
    updateTitle();
}

void NoteContainer::updateTitle()
{
    /*QString showName= getUriName();
    if(showName.isEmpty())
        showName= "untitled.txt";
    setWindowTitle(tr("%1[*] - (Notes)").arg(showName));*/
}

bool NoteContainer::readFileFromUri()
{
    /* if((nullptr == m_uri) || (nullptr == m_edit))
     {
         return false;
     }
     bool val= false;
     if(!m_uri->exists())
     {
         QByteArray array= m_uri->getData();
         QDataStream in(&array, QIODevice::ReadOnly);
         in.setVersion(QDataStream::Qt_5_7);
         readFromFile(in);
         val= true;
     }
     else
     {
         val= m_edit->load(m_uri->getUri());
     }
     updateTitle();*/
    return false;
}

void NoteContainer::saveMedia()
{
    if(nullptr != m_edit)
    {
        m_edit->fileSave();
        QString uri= m_edit->getFileName();
        // m_uri->setUri(uri);
    }
}
void NoteContainer::putDataIntoCleverUri()
{
    if(nullptr != m_edit)
    {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_7);
        m_edit->saveFileAsBinary(out);
        /*    if(nullptr != m_uri)
            {
                m_uri->setData(data);
            }*/
    }
}

void NoteContainer::setTitle(QString str)
{
    if(nullptr != m_edit)
    {
        m_edit->setCurrentFileName(str);
    }
}
void NoteContainer::readFromFile(QDataStream& data)
{
    if(nullptr != m_edit)
    {
        m_edit->readFromBinary(data);
    }
}

void NoteContainer::saveInto(QDataStream& out)
{
    if(nullptr != m_edit)
    {
        m_edit->saveFileAsBinary(out);
    }
}
