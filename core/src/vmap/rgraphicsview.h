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

#include "preferences/preferencesmanager.h"
#include "vmap.h"
#include "vmapwizzarddialog.h"

class VectorialMapController;
class VisualItemController;
/**
 * @brief RGraphicsView is custom graphicsview to fit rolisteam needs. It will implement some important feature
 */
class RGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief constructor with parameters
     * @param Map address which it will be displayed by the graphicsview
     */
    RGraphicsView(VectorialMapController* ctrl, QWidget* parent= nullptr);

    void currentToolChanged(Core::SelectableTool selectedtool);
    void readMessage(NetworkMessageReader* msg);

    void deleteItem(const QList<vmap::VisualItemController*>& list);
    void setItemLayer(const QList<vmap::VisualItemController*>& list, Core::Layer layer);
    void setRotation(const QList<vmap::VisualItemController*>& list, int value);
    void changeZValue(const QList<vmap::VisualItemController*>& list, VectorialMapController::StackOrder order);

public slots:
    void addImageToMap();
    void centerOnItem();

protected:
    // void keyPressEvent ( QKeyEvent * event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void focusInEvent(QFocusEvent* event);
    /* void dragEnterEvent ( QDragEnterEvent * event );
     void dropEvent ( QDropEvent * event );
     void dragMoveEvent( QDragMoveEvent * event );*/
    void wheelEvent(QWheelEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void resizeEvent(QResizeEvent* event);

    void createAction();

    void mouseMoveEvent(QMouseEvent* event);
    void lockItems(const QList<vmap::VisualItemController*>& list);

private slots:
    void rollInit();
    void cleanInit();
    void setZoomFactor();
    void showMapProperties();
    void changeLayer();
    void changeVisibility();
    void sendOffMapChange();
    void updateSizeToController();

private:
    QPointer<VectorialMapController> m_ctrl;
    // VMap* m_vmap= nullptr;

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

    QAction* m_allVisibility= nullptr;
    QAction* m_hiddenVisibility= nullptr;
    QAction* m_characterVisibility= nullptr;

    QAction* m_putGroundLayer= nullptr;
    QAction* m_putObjectLayer= nullptr;
    QAction* m_putCharacterLayer= nullptr;

    // z order action
    QAction* m_backOrderAction= nullptr;
    QAction* m_frontOrderAction= nullptr;
    QAction* m_lowerAction= nullptr;
    QAction* m_raiseAction= nullptr;

    QAction* m_importImage= nullptr;

    Core::SelectableTool m_currentTool;
    PreferencesManager* m_preferences= nullptr;
    QPoint m_lastPoint;
    QPoint m_menuPoint;
    QGraphicsItem* m_centerOnItem= nullptr;
};

#endif // RGRAPHICSVIEW_H
