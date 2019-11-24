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
#include "sessiondock.h"
#include <QCloseEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeView>

#include "controller/contentcontroller.h"
#include "preferences/preferencesmanager.h"
#include "sessionitemmodel.h"
#include "ui_sessiondock.h"

SessionDock::SessionDock(ContentController* ctrl, QWidget* parent)
    : QDockWidget(parent), m_ui(new Ui::SessionDock), m_ctrl(ctrl)
{
    m_ui->setupUi(this);
    setObjectName("SessionManager");

    setWindowTitle(tr("Resources Explorer"));

    m_ui->m_view->setModel(m_ctrl->model());

    m_ui->m_view->setDragEnabled(true);
    m_ui->m_view->setAcceptDrops(true);
    m_ui->m_view->setDropIndicatorShown(true);
    m_ui->m_view->setDefaultDropAction(Qt::MoveAction);
    m_ui->m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_ui->m_view->setAlternatingRowColors(true);

    QHeaderView* hHeader= m_ui->m_view->header(); // new QHeaderView(Qt::Vertical,this);
    /*hHeader->setSectionResizeMode(SessionItemModel::Name, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(SessionItemModel::LoadingMode, QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Displayed, QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Path, QHeaderView::ResizeToContents);
    m_ui->m_view->setHeader(hHeader);*/

    // m_view->setColumnHidden(SessionItemModel::LoadingMode,true);
    m_ui->m_view->setColumnHidden(SessionItemModel::Displayed, true);
    m_ui->m_view->setColumnHidden(SessionItemModel::Path, true);

    connect(m_ui->m_view, &SessionView::onDoubleClick, m_ctrl, &ContentController::openResources);
    connect(m_ui->m_view, &SessionView::addChapter, m_ctrl, &ContentController::addChapter);
    connect(m_ui->m_view, &SessionView::removeSelection, this,
            [this]() { m_ctrl->removeSelectedItems(m_ui->m_view->getSelection()); });
    // connect(m_model, &SessionItemModel::openResource, this, &SessionManager::openResource);
}
SessionDock::~SessionDock() {}

/*void SessionDock::addRessource(ResourcesNode* tp)
{
    if(nullptr == tp || nullptr == m_view)
        return;

    QModelIndex index= m_view->currentIndex();
    if(tp->getResourcesType() == ResourcesNode::Cleveruri)
    {
        auto cleveruri= dynamic_cast<CleverURI*>(tp);
        if(nullptr != cleveruri)
        {
            cleveruri->setListener(m_model);
        }
    }
    m_model->addResource(tp, index);
    emit sessionChanged(true);
}
void SessionDock::removeResource(CleverURI* uri)
{
    m_model->removeNode(uri);
}

void SessionDock::addChapter(QModelIndex& index)
{
    QString tmp= tr("Chapter %1").arg(m_model->rowCount(index) + 1);
    Chapter* chapter= new Chapter();
    chapter->setName(tmp);
    m_model->addResource(chapter, index);
}*/
void SessionDock::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    if(event->isAccepted())
    {
        emit changeVisibility(false);
    }
}

/*void SessionManager::openResources(QModelIndex& index)
{
    if(!index.isValid())
        return;

    emit openResource(static_cast<ResourcesNode*>(index.internalPointer()), false);
}
void SessionManager::removeSelectedItem()
{
    QModelIndexList list= m_view->getSelection();
    for(int i= list.size() - 1; i >= 0; i--)
    {
        QModelIndex index= list.at(i);
        if(!index.isValid())
            continue;
        m_model->remove(index);
    }
}*/

/*void SessionDock::updateCleverUri(CleverURI* uri)
{
    if(nullptr != m_model)
    {
        m_model->updateNode(uri);
    }
}

void SessionDock::saveSession(QDataStream& out)
{
    if(nullptr != m_model)
    {
        m_model->saveModel(out);
        emit sessionChanged(false);
    }
}
void SessionDock::loadSession(QDataStream& in)
{
    if(nullptr != m_model)
    {
        m_model->loadModel(in);
    }
}*/

/*void SessionManager::resourceClosed(CleverURI* uri)
{
    if(nullptr == uri)
        return;

    m_view->clearSelection();
    uri->setState(CleverURI::Unloaded);
    if((uri->getUri().isEmpty() && uri->getCurrentMode() == CleverURI::Linked)) // new and unsaved document
    {
        m_model->removeNode(uri);
    }
}*/
