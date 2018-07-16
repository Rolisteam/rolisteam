/***************************************************************************
    *   Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
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
#include <QHeaderView>



#include "sessionitemmodel.h"
#include "preferences/preferencesmanager.h"

SessionManager::SessionManager(QWidget* parent)
    : QWidget(parent), m_sessionName(tr("Unknown"))
{
    setObjectName("SessionManager");
    m_options = PreferencesManager::getInstance();
    m_view =new SessionView(this);
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->addWidget(m_view);
    //m_internal = new QWidget(this);
    //m_layout->setParent(m_internal);
    //m_internal->setLayout(m_layout);
    //setWidget(m_internal);
    setLayout(m_layout);
    
    setWindowTitle(tr("Resources Explorer"));
    
    m_model = new SessionItemModel();
    //CleverURI::setListener(m_model);
    
    m_view->setModel(m_model);


    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDropIndicatorShown(true);
    m_view->setDefaultDropAction(Qt::MoveAction);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_view->setAlternatingRowColors(true);

    QHeaderView* hHeader = m_view->header();//new QHeaderView(Qt::Vertical,this);
    hHeader->setSectionResizeMode(SessionItemModel::Name,QHeaderView::Stretch);
    hHeader->setSectionResizeMode(SessionItemModel::LoadingMode,QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Displayed,QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Path,QHeaderView::ResizeToContents);
    m_view->setHeader(hHeader);

    m_view->setColumnHidden(SessionItemModel::LoadingMode,true);
    m_view->setColumnHidden(SessionItemModel::Displayed,true);
    m_view->setColumnHidden(SessionItemModel::Path,true);


    connect(m_view,SIGNAL(onDoubleClick(QModelIndex&)),this,SLOT(openResources(QModelIndex&)));
    connect(m_view,SIGNAL(addChapter(QModelIndex&)),this,SLOT(addChapter(QModelIndex&)));
    connect(m_view,SIGNAL(removeSelection()),this,SLOT(removeSelectedItem()));
    connect(m_model,&SessionItemModel::openResource,this,&SessionManager::openResource);
}
SessionManager::~SessionManager()
{
   delete m_model;
}

Chapter* SessionManager::getCurrentChapter()
{
    return m_currentChapter;
}
void SessionManager::addRessource(ResourcesNode* tp)
{
    QModelIndex index = m_view->currentIndex();
    if(tp->getResourcesType() == ResourcesNode::Cleveruri)
    {
        auto cleveruri = dynamic_cast<CleverURI*>(tp);
        if(nullptr != cleveruri)
        {
            cleveruri->setListener(m_model);
        }
    }
    m_model->addResource(tp,index);
    emit sessionChanged(true);
}
void SessionManager::removeResource(CleverURI* uri)
{
    m_model->removeNode(uri);
}

SessionView *SessionManager::getView() const
{
    return m_view;
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
    QWidget::closeEvent(event);
    if(event->isAccepted())
    {
        emit changeVisibility(false);
    }
}

QString SessionManager::getSessionName() const
{
    return m_sessionName;
}

void SessionManager::setSessionName(const QString &sessionName)
{
    m_sessionName = sessionName;
}
SessionItemModel *SessionManager::getModel() const
{
    return m_model;
}

void SessionManager::setModel(SessionItemModel *model)
{
    m_model = model;
}

void SessionManager::openResources(QModelIndex& index)
{
    if(!index.isValid())
        return;

    emit openResource(static_cast<ResourcesNode*>(index.internalPointer()),false);
}
void SessionManager::removeSelectedItem()
{
    QModelIndexList list=m_view->getSelection();
    for(int i = list.size() - 1 ; i >= 0; i--)
    {
        QModelIndex index = list.at(i);
        if(!index.isValid())
            continue;
        m_model->remove(index);
    }
}
void SessionManager::updateCleverUri(CleverURI* uri )
{
    if(nullptr!=m_model)
    {
        m_model->updateNode(uri);
    }
}

void SessionManager::saveSession(QDataStream& out)
{
    if(nullptr!=m_model)
    {
        m_model->saveModel(out);
        emit sessionChanged(false);
    }
}
void SessionManager::loadSession(QDataStream& in)
{
    if(nullptr!=m_model)
    {
        m_model->loadModel(in);
    }
}
void SessionManager::resourceClosed(CleverURI* uri)
{
    if(nullptr!=uri)
    {
        m_view->clearSelection();
        uri->setState(CleverURI::Unloaded);
        if((uri->getUri().isEmpty())||
           (uri->getCurrentMode() != CleverURI::Linked))//new and unsaved document
        {
            m_model->removeNode(uri);
        }
    }
}
