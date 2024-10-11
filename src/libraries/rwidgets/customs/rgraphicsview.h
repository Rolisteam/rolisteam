/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef RGRAPHICSVIEW_H
#define RGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPointer>

#include "commands/changestackordervmapcommand.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets_global.h"

class VectorialMapController;

/**
 * @brief RGraphicsView is custom graphicsview to fit rolisteam needs. It will implement some important feature
 */
class RWIDGET_EXPORT RGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief constructor with parameters
     * @param Map address which it will be displayed by the graphicsview
     */
    RGraphicsView(VectorialMapController* ctrl, QWidget* parent= nullptr);

    void currentToolChanged(Core::SelectableTool selectedtool);

    void deleteItem(const QList<vmap::VisualItemController*>& list);
    void setItemLayer(const QList<vmap::VisualItemController*>& list, Core::Layer layer);
    void setRotation(const QList<vmap::VisualItemController*>& list, int value);

public slots:
    void addImageToMap();
    void centerOnItem();

protected:
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void resizeEvent(QResizeEvent* event);

    void createAction();
    void lockItems(const QList<vmap::VisualItemController*>& list);

    QList<ItemToControllerInfo> selection();

signals:
    void updateVisualZone();

private slots:
    void rollInit();
    void cleanInit();
    void setZoomFactor();
    void changeLayer();
    void changeVisibility();
    void updateSizeToController();
    void stackBefore(const QList<ItemToControllerInfo>& first, const QList<ItemToControllerInfo>& second);

private:
    QPointer<VectorialMapController> m_ctrl;

    int m_counterZoom;

    QAction* m_zoomNormal= nullptr;
    QAction* m_zoomInMax= nullptr;
    QAction* m_zoomOutMax= nullptr;
    QAction* m_zoomIn= nullptr;
    QAction* m_zoomOut= nullptr;
    QAction* m_zoomCenterOnItem= nullptr;
    QAction* m_properties= nullptr;
    QAction* m_editGroundLayer= nullptr;
    QAction* m_editObjectLayer= nullptr;
    QAction* m_editCharacterLayer= nullptr;
    QAction* m_editGameMasterLayer= nullptr;

    // Global action
    QAction* m_rollInitOnAllNpc= nullptr;
    QAction* m_rollInitOnSelection= nullptr;
    QAction* m_rollInitOnAllCharacter= nullptr;
    QAction* m_cleanInitOnAllNpc= nullptr;
    QAction* m_cleanInitOnSelection= nullptr;
    QAction* m_cleanInitOnAllCharacter= nullptr;

    QAction* m_normalizeSizeAverage= nullptr;
    QAction* m_normalizeSizeUnderMouse= nullptr;
    QAction* m_normalizeSizeBigger= nullptr;
    QAction* m_normalizeSizeSmaller= nullptr;

    QAction* m_lockSize= nullptr;
    QAction* m_dupplicate{nullptr};

    QAction* m_allVisibility= nullptr;
    QAction* m_hiddenVisibility= nullptr;
    QAction* m_characterVisibility= nullptr;

    QAction* m_putGroundLayer= nullptr;
    QAction* m_putObjectLayer= nullptr;
    QAction* m_putCharacterLayer= nullptr;
    QAction* m_putGMLayer= nullptr;

    // z order action
    QAction* m_backOrderAction= nullptr;
    QAction* m_frontOrderAction= nullptr;
    QAction* m_lowerAction= nullptr;
    QAction* m_raiseAction= nullptr;

    QAction* m_removeSelection= nullptr;

    QAction* m_importImage= nullptr;

    Core::SelectableTool m_currentTool;
    QPointer<PreferencesManager> m_preferences;
    QPointF m_lastPoint;
    QPoint m_menuPoint;
    QGraphicsItem* m_centerOnItem= nullptr;
};

#endif // RGRAPHICSVIEW_H
