/***************************************************************************
    *	 Copyright (C) 2017 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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
#include "setbackgroundimage.h"

SetBackgroundCommand::SetBackgroundCommand(Canvas* canvas,const QUrl& url,QUndoCommand *parent)
  : QUndoCommand(parent),m_image(new QPixmap(url.toLocalFile())),m_canvas(canvas)
{
    init();
}
SetBackgroundCommand::SetBackgroundCommand(Canvas* canvas,QPixmap* pix, QUndoCommand *parent)
 : QUndoCommand(parent),m_image(pix),m_canvas(canvas)
{
   init();
}
void SetBackgroundCommand::init()
{
    m_previousRect = m_canvas->sceneRect();
    m_bgItem = m_canvas->getBg();
    if(nullptr == m_bgItem)
    {
        m_bgItem = new QGraphicsPixmapItem();
        m_bgItem->setFlag(QGraphicsItem::ItemIsSelectable,false);
        m_bgItem->setFlag(QGraphicsItem::ItemSendsGeometryChanges,false);
        m_bgItem->setFlag(QGraphicsItem::ItemIsMovable,false);
        m_bgItem->setFlag(QGraphicsItem::ItemIsFocusable,false);
        m_bgItem->setAcceptedMouseButtons(Qt::NoButton);
        m_canvas->setBg(m_bgItem);
    }
    setText(QObject::tr("Set background on Page #%1").arg(m_canvas->currentPage()+1));
}

void SetBackgroundCommand::undo()
{
    if(nullptr != m_bgItem)
    {
        QPixmap map;
        m_bgItem->setPixmap(map);
        m_canvas->setPixmap(nullptr);
        m_canvas->removeItem(m_bgItem);
        m_canvas->setSceneRect(m_previousRect);
    }
}

void SetBackgroundCommand::redo()
{
   m_bgItem->setPixmap(*m_image);
   m_canvas->addItem(m_bgItem);
   m_bgItem->setZValue(-1);
   m_canvas->setPixmap(m_image);
   m_canvas->setSceneRect(m_bgItem->boundingRect());
}
