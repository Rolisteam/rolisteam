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
#ifndef DELETEPAGECOMMAND_H
#define DELETEPAGECOMMAND_H

#include <QUndoCommand>
#include "canvas.h"
#include <QObject>
#include <QStringListModel>
#include "fieldmodel.h"

#include "deletefieldcommand.h"

class DeletePageCommand : public QUndoCommand
{
public:
  DeletePageCommand(int currentPage, QList<Canvas*>& list,FieldModel* model, QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

  Canvas *canvas() const;

  static QStringListModel* getPagesModel();
  static void setPagesModel(QStringListModel* pagesModel);

private:
  int m_currentPage;
  Canvas* m_canvas;
  QList<Canvas*>& m_list;
  static QStringListModel* m_pagesModel;
  FieldModel* m_model;
  QList<CharacterSheetItem*> m_subField;
  QList<Canvas*> m_subCanvas;
  QList<int> m_subCurrentPage;
  DeleteFieldCommand* m_deleteField;
};

#endif // DELETEPAGECOMMAND_H
