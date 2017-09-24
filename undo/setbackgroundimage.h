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
#ifndef SETBACKGROUNDCOMMAND_H
#define SETBACKGROUNDCOMMAND_H

#include <QUndoCommand>
#include <QPixmap>
#include <QUrl>

#include "canvas.h"

class SetBackgroundCommand : public QUndoCommand
{
public:
  SetBackgroundCommand(QGraphicsPixmapItem*& bg,Canvas* canvas,const QUrl& url, QUndoCommand *parent = 0);
  SetBackgroundCommand(QGraphicsPixmapItem*& bg,Canvas* canvas,QPixmap* pix, QUndoCommand *parent = 0);


  void undo() override;
  void redo() override;

protected:
  void init();
private:
   QPixmap* m_image;
   Canvas* m_canvas;
   QGraphicsPixmapItem* m_bgItem;
   QRectF m_previousRect;
};

#endif // SETBACKGROUNDCOMMAND_H
