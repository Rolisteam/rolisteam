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
#ifndef ADDFIELDCOMMAND_H
#define ADDFIELDCOMMAND_H

#include "canvas.h"
#include <QUndoCommand>

namespace charactersheet
{
class ImageModel;
}
class AddFieldCommand : public QUndoCommand
{
public:
    AddFieldCommand(Canvas::Tool tool, Canvas* graphicsScene, FieldModel* model, int currentPage,
                    charactersheet::ImageModel* imageModel, QPointF pos, QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

    FieldController* getField() const;

private:
    Canvas::Tool m_tool;
    FieldController* m_field;
    Canvas* m_canvas;
    QPointF initialPosition;
    FieldModel* m_model;
    int m_currentPage;
    charactersheet::ImageModel* m_imageModel;
    bool m_addImage;
};

#endif // ADDFIELDCOMMAND_H
