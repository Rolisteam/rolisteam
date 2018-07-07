/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
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
#ifndef CHANGECOLORITEM_H
#define CHANGECOLORITEM_H

#include <QUndoCommand>
#include <QColor>

class VisualItem;

class ChangeColorItemCmd : public QUndoCommand
{
public:
    ChangeColorItemCmd(VisualItem* item, QColor newColor, QString mapId, QUndoCommand* parent = nullptr);

    void redo() override;
    void undo() override;

    void sendOffColor();


private:
    VisualItem* m_item;
    QColor m_oldColor;
    QColor m_newColor;
    QString m_mapId;
};

#endif // CHANGECOLORITEM_H
