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
#include "session.h"
#include "preferencesmanager.h"

SessionManager::SessionManager()
    : m_recentfileCount(5)
{
    m_currentSession = new Session();
    setObjectName("SessionManager");
    m_options = PreferencesManager::getInstance();
    m_recentfileCount = m_options->value("session/recentfilesCount",m_recentfileCount).toInt();
    m_view =new SessionView;
    m_layout = new QHBoxLayout;
    m_layout->setMargin(0);
    m_layout->addWidget(m_view);
    m_internal = new QWidget();
    m_internal->setLayout(m_layout);
    setWidget(m_internal);

    setWindowTitle(tr("Resources Explorer"));

    m_model = new SessionItemModel;

    m_view->setModel(m_model);
    connect(m_view,SIGNAL(onDoubleClick(QModelIndex&)),this,SLOT(openResources(QModelIndex&)));
    connect(m_view,SIGNAL(addChapter(QModelIndex&)),this,SLOT(addChapter(QModelIndex&)));
    connect(m_view,SIGNAL(removeSelection()),this,SLOT(removeSelectedItem()));
}
Chapter* SessionManager::getCurrentChapter()
{
    return m_currentChapter;
}

void SessionManager::setCurrentSession(Session* s)
{
    m_currentSession=s;
    m_model->setSession(m_currentSession);
}
CleverURI* SessionManager::addRessource(CleverURI* tp)
{
    QModelIndex index = m_view->currentIndex();
    m_model->addRessources(tp,index);

    m_recentlist.prepend(tp);

    if(m_recentlist.size()>m_recentfileCount)
    {
        m_recentlist.removeLast();
    }
    return  tp;
}

void SessionManager::readSettings(QSettings & m)
{
    m.beginGroup("Session");
    QVariant r;
    r.setValue<Session>(*m_currentSession);
    *m_currentSession=m.value("Session",r).value<Session>();

    // if no number that means no recenfile
    int number = m.value("numberRecentFile",0).toInt();
    qDebug()<< "number of recent file session manager" << number;
    for(int i=0 ; i<number ; i++)
    {
        CleverURI* cleverURI = new CleverURI;
        r.setValue<CleverURI>(*cleverURI);
        *cleverURI=m.value(QString("recentfile%1").arg(i)).value<CleverURI>();
        m_recentlist.prepend(cleverURI);
        m.setValue(QString("recentfile%1").arg(i),r);
    }


    m.endGroup();

    m_model->setSession(m_currentSession);
}
const QList<CleverURI*>& SessionManager::getRecentFiles()
{
    return m_recentlist;
}

void SessionManager::addChapter(QModelIndex& index)
{
    QString tmp = tr("Chapter %1").arg(m_currentSession->chapterCount());
    //m_currentChapter = m_model->addChapter(tmp,m_view->currentIndex());
    m_currentChapter =m_model->addChapter(tmp,index);

}

void SessionManager::writeSettings(QSettings & m)
{
    m.beginGroup("Session");
    QVariant r;
    r.setValue<Session>(*m_currentSession);
    m.setValue("Session",r);


    m.setValue("numberRecentFile",m_recentlist.size());
    for(int i=0 ; i<m_recentlist.size() ; i++)
    {
        CleverURI* cleverURI =m_recentlist.at(i);
        r.setValue<CleverURI>(*cleverURI);
        m.setValue(QString("recentfile%1").arg(i),r);
        //con.m_recentlist.append(cleverURI);
    }

    m.endGroup();
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
    m_currentChapter = m_model->getChapter(p);
}

void SessionManager::openResources(QModelIndex& index)
{
      ResourcesItem* item = static_cast<ResourcesItem*>(index.internalPointer());
      if(item!=NULL)
      {
          CleverURI* uri = dynamic_cast<CleverURI*>(item->getData());
          if(uri!=NULL)
          {
              emit openFile(uri);
          }
      }
}
void SessionManager::removeSelectedItem()
{

    QModelIndexList list=m_view->getSelection();
//    if(list!=NULL)
//    {
        qDebug() << "size of the list= " <<list.size();
        for(int i = 0 ; i < list.size(); i++)
        {
            QModelIndex index = list.at(i);
            qDebug() << "coord= " <<index.column()<<index.row();
            if(!index.isValid())
                continue;
            m_model->remove(index);
        }
//    }
}

