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
#ifndef ADDPAGECOMMAND_H
#define ADDPAGECOMMAND_H

#include "canvas.h"
#include <QObject>
#include <QStringListModel>
#include <QUndoCommand>

class EditorController;
class AddPageCommand : public QUndoCommand
{
public:
    AddPageCommand(EditorController* ctrl, QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

    // Canvas *canvas() const;

    //  static QStringListModel* getPagesModel();
    //  static void setPagesModel(QStringListModel* pagesModel);

private:
    int m_pageId;
    EditorController* m_ctrl;
    //  Canvas* m_canvas;
};

#endif // ADDPAGECOMMAND_H
