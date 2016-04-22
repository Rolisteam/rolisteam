/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
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
#include "vmap.h"
#include "vmapwizzarddialog.h"
#include "preferences/preferencesmanager.h"
/**
    * @brief RGraphicsView is custom graphicsview to fit rolisteam needs. It will implement some important feature
    */
class RGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    enum Method {Bigger, Smaller, UnderMouse, Average };
    /**
    * @brief constructor with parameters
    * @param Map address which it will be displayed by the graphicsview
    */
    RGraphicsView(VMap* vmap);
    
	void currentToolChanged(VToolsBar::SelectableTool selectedtool);
    void readMessage(NetworkMessageReader* msg);


    void deleteItem(QList<QGraphicsItem*> list);
    void setItemLayer(QList<QGraphicsItem*> list,VisualItem::Layer layer);
    void setRotation(QList<QGraphicsItem*> list, int value);
    void changeZValue(QList<QGraphicsItem *> list, VisualItem::StackOrder order);
    void normalizeSize(QList<QGraphicsItem *> list,Method method, QPoint point);

public slots:
    void addImageToMap();
protected:
    void keyPressEvent ( QKeyEvent * event);
    void mousePressEvent ( QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void focusInEvent ( QFocusEvent * event );
   /* void dragEnterEvent ( QDragEnterEvent * event );
    void dropEvent ( QDropEvent * event );
	void dragMoveEvent( QDragMoveEvent * event );*/
    void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void resizeEvent(QResizeEvent* event);

    void createAction();



    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void setZoomFactor();
    void showMapProperties();
	void rubberBandGeometry(QRect viewportRect, QPointF fromScenePoint, QPointF toScenePoint);

    void changeLayer();
    void changeVisibility();
    void sendOffMapChange();

private:
    VMap* m_vmap;

    int m_counterZoom;

    QAction* m_zoomNormal;
    QAction* m_zoomInMax;
    QAction* m_zoomOutMax;
    QAction* m_properties;
    QAction* m_editGroundLayer;
    QAction* m_editObjectLayer;
    QAction* m_editCharacterLayer;

    QAction* m_normalizeSizeAverage;
    QAction* m_normalizeSizeUnderMouse;
    QAction* m_normalizeSizeBigger;
    QAction* m_normalizeSizeSmaller;

    QAction* m_lockSize;

    QAction* m_allVisibility;
    QAction* m_hiddenVisibility;
    QAction* m_characterVisibility;


    QAction* m_putGroundLayer;
    QAction* m_putObjectLayer;
    QAction* m_putCharacterLayer;

    QAction* m_importImage;

	VToolsBar::SelectableTool m_currentTool;
    PreferencesManager* m_preferences;
    QPoint m_lastPoint;

};

#endif // RGRAPHICSVIEW_H
