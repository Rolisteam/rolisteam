/***************************************************************************
    *   Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "sessionmanager.h"
#include <QTreeView>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QDebug>

#include "sessionitemmodel.h"
#include "preferences/preferencesmanager.h"

SessionManager::SessionManager()
{

    setObjectName("SessionManager");
    m_options = PreferencesManager::getInstance();
    m_view =new SessionView;
    m_layout = new QHBoxLayout;
    m_layout->setMargin(0);
    m_layout->addWidget(m_view);
    m_internal = new QWidget();
    m_internal->setLayout(m_layout);
    setWidget(m_internal);
    
    setWindowTitle(tr("Resources Explorer"));
    
    m_model = new SessionItemModel();
    
    m_view->setModel(m_model);
    connect(m_view,SIGNAL(onDoubleClick(QModelIndex&)),this,SLOT(openResources(QModelIndex&)));
    connect(m_view,SIGNAL(addChapter(QModelIndex&)),this,SLOT(addChapter(QModelIndex&)));
    connect(m_view,SIGNAL(removeSelection()),this,SLOT(removeSelectedItem()));
}
Chapter* SessionManager::getCurrentChapter()
{
    return m_currentChapter;
}
CleverURI* SessionManager::addRessource(CleverURI* tp)
{
    QModelIndex index = m_view->currentIndex();
    m_model->addResource(tp,index);
    return  tp;
}


void SessionManager::addChapter(QModelIndex& index)
{
    QString tmp = tr("Chapter %1").arg(m_model->rowCount(index));
    Chapter* chapter = new Chapter();
    chapter->setName(tmp);
    m_model->addResource(chapter,index);
}
void SessionManager::closeEvent ( QCloseEvent * event )
{
    QDockWidget::closeEvent(event);
    if(event->isAccepted())
    {
        emit changeVisibility(false);
    }
}

void SessionManager::setCurrentChapter()
{
    QModelIndex p = m_view->currentIndex();
   // m_currentChapter = m_model->getChapter(p);
}

void SessionManager::openResources(QModelIndex& index)
{
    if(!index.isValid())
        return;

    CleverURI* item = static_cast<CleverURI*>(index.internalPointer());

    if(item!=NULL)
    { 
       emit openFile(item);
    }
}
void SessionManager::removeSelectedItem()
{
    QModelIndexList list=m_view->getSelection();
    for(int i = 0 ; i < list.size(); i++)
    {
        QModelIndex index = list.at(i);
        if(!index.isValid())
            continue;
        m_model->remove(index);
    }
}
void SessionManager::saveSession(QDataStream& out)
{
    if(NULL!=m_model)
    {
        m_model->saveModel(out);
    }
}
void SessionManager::loadSession(QDataStream& in)
{
    if(NULL!=m_model)
    {
        m_model->loadModel(in);
    }
}
