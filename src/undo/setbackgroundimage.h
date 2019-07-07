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

#include <QPixmap>
#include <QUndoCommand>
#include <QUrl>

class QGraphicsPixmapItem;
class EditorController;
class SetBackgroundCommand : public QUndoCommand
{
public:
    SetBackgroundCommand(int i, EditorController* ctrl, const QUrl& url, QUndoCommand* parent= nullptr);
    SetBackgroundCommand(int i, EditorController* ctrl, const QPixmap& pix, const QString& fileName,
                         QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

protected:
    void init();

private:
    QPixmap m_image;
    int m_idx= -1;
    QGraphicsPixmapItem* m_bgItem= nullptr;
    QRectF m_previousRect;
    EditorController* m_ctrl= nullptr;
    QString m_filename;
    QUndoCommand* m_subCommand= nullptr;
};

#endif // SETBACKGROUNDCOMMAND_H
