/***************************************************************************
    *      Copyright (C) 2010 by Renaud Guezennec                             *
    *                                                                         *
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
#ifndef CHARACTERITEM_H
#define CHARACTERITEM_H
#include "visualitem.h"
#include "data/person.h"

#include <QAction>
/**
    * @brief represents any character on map.
    */
class CharacterItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief CharacterItem
     */
    CharacterItem();
    /**
     * @brief CharacterItem
     * @param m
     * @param center
     * @param diameter
     */
    CharacterItem(Character* m,QPointF center,int diameter = 40);
    /**
    * @brief constructor
    */
    //CharacterItem(QColor& penColor,QGraphicsItem * parent = 0);
    /**
    * @brief serialisation function to write data
    */
    virtual void writeData(QDataStream& out) const;
    /**
    * @brief serialisation function to read data.
    */
    virtual void readData(QDataStream& in);
    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType();
    
    /**
    * @brief gives the bounding rect of the ellipse
    */
    virtual QRectF boundingRect() const ;
    /**
    * @brief modifies the ellipse size and shape.
    */
    virtual void setNewEnd(QPointF& nend);
    /**
    * @brief paint the ellipse at the correct position
    */
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg);
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg);
    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    void setGeometryPoint(qreal pointId, QPointF &pos);
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem();
    /**
     * @brief resizeContents
     * @param rect
     * @param keepRatio
     */
    void resizeContents(const QRect& rect, bool keepRatio);
    /**
     * @brief updateChildPosition
     */
    void updateChildPosition();
    /**
     * @brief itemChange
     * @param change
     * @param value
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu*);

public slots:
    void sizeChanged(int m_size);
    /**
     * @brief showNpcName
     */
    void showNpcName(bool);
    /**
     * @brief showNpcNumber
     */
    void showNpcNumber(bool);
    /**
     * @brief showPcName
     */
    void showPcName(bool);
private slots:
    /**
     * @brief generatedThumbnail
     */
    void generatedThumbnail();
    /**
     * @brief createActions
     */
    void createActions();
    /**
     * @brief characterStateChange
     */
    void characterStateChange();
private:
    Character* m_character;
    QPointF m_center;
    int m_diameter;
    QPixmap* m_thumnails;
    QRectF m_rect;
    bool m_showNpcName;
    bool m_showNpcNumber;
    bool m_showPcName;


    //QAction*
    QAction* m_healthyStateAct;
    QAction* m_lightlyStateAct;
    QAction* m_seriouslyStateAct;
    QAction* m_deadStateAct;
    QAction* m_spleepingStateAct;
    QAction* m_bewitchedStateAct;
    
    
};

#endif // CHARACTERITEM_H
