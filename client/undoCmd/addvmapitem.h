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
#ifndef ADDVMAPITEMCOMMAND_H
#define ADDVMAPITEMCOMMAND_H

#include <QUndoCommand>
#include "vmap/vmap.h"
#include "vmap/vtoolbar.h"


class AddVmapItemCommand : public QUndoCommand
{
public:
    AddVmapItemCommand(VToolsBar::SelectableTool tool,
                       VMap* canvas,
                       QPointF& pos,
                       QColor& color,
                       int penSize,
                       QUndoCommand *parent= nullptr);

    AddVmapItemCommand(VisualItem* item,VMap* map,QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    template <class T>
    T* getItem() const;
    VisualItem* getItem() const;
    VisualItem* getPath() const;

    QString getLocalUserId() const;
    void setLocalUserId(const QString &localUserId);

    bool hasError() const;
    void setError(bool error);

    bool isNpc()const;

    bool isUndoable() const;
    void setUndoable(bool undoable);

    bool getInitPoint() const;
    void setInitPoint(bool initPoint);

protected:
    bool isVisible();
    bool isEditable();
    void initItem();
private:
    VMap* m_vmap=nullptr;
    QPointF m_pos;
    QColor m_color;
    int m_penSize;
    VisualItem* m_currentPath = nullptr;
    bool m_error = false;
    VToolsBar::SelectableTool m_tool;
    bool m_first;
    bool m_undoable;
    bool m_initPoint = true;
    VisualItem* m_currentItem = nullptr;
};

#endif // ADDFIELDCOMMAND_H
