/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

NoteContainer::NoteContainer()
    : m_edit(new TextEdit())
{
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    if(NULL!=m_edit)
    {
        m_title = m_edit->getShowName();
    }
    setCleverUriType(CleverURI::TEXT);
    setWidget(m_edit);
    setWindowIcon(QIcon(":/notes.png"));
}

void NoteContainer::setFileName(QString str)
{
    if(NULL!=m_uri)
    {
        m_uri->setUri(str);
    }
}

bool NoteContainer::readFileFromUri()
{
    if((NULL==m_uri)||(NULL==m_edit))
    {
        return false;
    }
    return m_edit->load(m_uri->getUri());
}

void NoteContainer::saveMedia()
{
    if(NULL!=m_edit)
    {
        m_edit->fileSave();
        QString uri = m_edit->getShowName();
        m_uri->setUri(uri);
    }
}
void NoteContainer::readFromFile(QDataStream& data)
{
    if(NULL!=m_edit)
    {
        m_edit->readFromBinary(data);
        m_title = m_edit->windowTitle();
    }
}

void NoteContainer::saveInto(QDataStream &out)
{
    if(NULL!=m_edit)
    {
        m_edit->saveFileAsBinary(out);
    }
}

