/***************************************************************************
 * Copyright (C) 2019 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#ifndef EDITORCONTROLLER_H
#define EDITORCONTROLLER_H

#include "canvas.h"
#include <QAction>
#include <QObject>
#include <QUndoStack>
#include <memory>

class ItemEditor;
class EditorController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(std::size_t pageCount READ pageCount NOTIFY pageCountChanged)
public:
    EditorController(QUndoStack& undoStack, ItemEditor* view, QObject* parent= nullptr);

    const std::vector<Canvas*>& pageList() const;
    std::size_t pageCount() const;
    int currentPage() const;

    void clearData(bool defaulCanvas);

    void load(QJsonObject& obj);
    void save(QJsonObject& obj);
    void setImageBackground(int idx, const QPixmap& pix, const QString& filepath);

    void addItem(int idx, QGraphicsItem* item);
public slots:
    int addPage();
    Canvas* removePage(int idx);
    void insertPage(int idx, Canvas* canvas);
    void setFitInView();
    void setCurrentPage(int currentPage);
    void setCurrentTool(Canvas::Tool tool);
    void loadImageFromUrl(const QUrl&);

signals:
    void currentPageChanged(int currentPage);
    void pageAdded(Canvas* canvas);
    void pageCountChanged();
    void canvasBackgroundChanged(int idx, const QPixmap& pix, const QString& str, const QString& uuid);
    void dataChanged();

protected:
    void updateView();
protected slots:
    void alignOn();
    void menuRequestedFromView(const QPoint& pos);
    void sameGeometry();
    void spreadItemEqualy();
    void lockItem();

private:
    QAction* m_lockItem= nullptr;
    QAction* m_fitInView= nullptr;
    QAction* m_alignOnY= nullptr;
    QAction* m_alignOnX= nullptr;
    QAction* m_sameWidth= nullptr;
    QAction* m_sameHeight= nullptr;
    QAction* m_dupplicate= nullptr;
    QAction* m_verticalEquaDistance= nullptr;
    QAction* m_horizontalEquaDistance= nullptr;
    QPoint m_posMenu;
    ItemEditor* m_view= nullptr;

    std::vector<std::unique_ptr<Canvas>> m_canvasList;
    int m_currentPage;

    QUndoStack& m_undoStack;
};

#endif
