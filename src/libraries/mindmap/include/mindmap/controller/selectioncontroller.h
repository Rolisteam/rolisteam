/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H

#include "mindmap/mindmap_global.h"
#include <QObject>
#include <QPointF>
#include <QUndoStack>
#include <vector>

namespace mindmap
{

class MindNode;
class MindItem;
class MINDMAP_EXPORT SelectionController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString lastSelected READ lastSelected NOTIFY lastSelectedChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
public:
    explicit SelectionController(QObject* parent= nullptr);

    void setUndoStack(QUndoStack* stack);

    QString lastSelected() const;

    bool enabled() const;
    bool hasSelection() const;

    const std::vector<mindmap::MindItem*>& selectedNodes() const;
signals:
    void enabledChanged();
    void lastSelectedChanged();
    void hasSelectionChanged();

public slots:
    void setEnabled(bool enable);
    void addToSelection(mindmap::MindItem* node);
    void removeFromSelection(mindmap::MindItem* node);
    void movingNode(const QPointF& motion);
    void clearSelection();

private:
    void setLastSelectedId(const QString& id);

private:
    std::vector<mindmap::MindItem*> m_selection;
    bool m_enabled= false;
    QUndoStack* m_undoStack;
    QString m_lastSelectedId;
};
} // namespace mindmap
#endif // SELECTIONCONTROLLER_H
