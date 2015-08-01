/***************************************************************************
    *   Copyright (C) 2010 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#ifndef VMAP_H
#define VMAP_H

#include <QGraphicsScene>
#include <QColor>
#include "vtoolbar.h"
#include "map/map.h"

#include "data/person.h"

class VisualItem;
/**
    * @brief allows users to draw a map on the fly. It manages several kinds of items (VisualItem): rect, line...
    * It is using the QGraphicsScene from Qt.
    */
class VMap : public QGraphicsScene
{
    Q_OBJECT
public:


    enum SCALE_UNIT{CM,M,INCH,FEET,PX};
    /**
    * @brief default constructor
    */
    VMap(QObject * parent = 0);
    /**
    * @brief constructor with parameters
    * @param witdh of the map
    * @param height of the map
    * @param title of the subwindow, will be used for saving the map into file.
    * @param bgcolor is the backgound color of the map (commonly white).
    */
    VMap(int width,int height,QString& title,QColor& m_bgColor,QObject * parent = 0);
    
    /**
    * @brief defines the width
    */
    void setWidth(int width);
    /**
    * @brief defines the height
    */
    void setHeight(int height);
    /**
    * @brief defines the title
    */
    void setTitle(QString title);
    /**
    * @brief defines the background color
    */
    void setBackGroundColor(QColor& bgcolor);
    /**
    * @brief  unused ?
    * @todo check the relevance of this function
    */
    void setSceneRect();
    /**
    * @brief accessor to width
    */
    int mapWidth() const;
    /**
    * @brief accessor to height
    */
    int mapHeight() const;
    /**
    * @brief accessor to the title
    */
    const QString& mapTitle() const;
    /**
    * @brief accessor to the background color
    */
    const QColor& mapColor() const;
    /**
     * @brief saveFile
     */
    void saveFile(QDataStream& );
    /**
     * @brief openFile
     */
    void openFile(QDataStream& );
    /**
     * @brief addCharacter
     * @param p
     * @param pos
     */
    void addCharacter(const Character* p, QPointF pos);

    QString getId() const;
    int getNpcSize() const;

    //network management
    /**
     * @brief processAddItemMessage
     * @param msg
     */
    void processAddItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processMoveItemMessage
     * @param msg
     */
    void processMoveItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processGeometryChangeItem
     * @param msg
     */
    void processGeometryChangeItem(NetworkMessageReader* msg);
	/**
	 * @brief setPermissionMode
	 * @param mode
	 */
	void setPermissionMode(Map::PermissionMode mode);
    /**
     * @brief processDelItemMessage
     * @param msg
     */
    void processDelItemMessage(NetworkMessageReader* msg);
	/**
	 * @brief getPermissionMode
	 * @return
	 */
	Map::PermissionMode getPermissionMode();
	/**
	 * @brief setLocalIsGM
	 */
	void setLocalIsGM(bool);
public slots:
    /**
    * @brief defines the current tools
    * @param new tool
    */
    void setCurrentTool(VToolsBar::SelectableTool selectedtool);
    /**
    * @brief defines the current color for painting
    * @param new color
    */
    void setCurrentChosenColor(QColor&);
    /**
    * @brief defines the pen size (sent off by toolbar).
    */
    void setPenSize(int);
    /**
    * @brief defines NPC size.
    */
    void setNPCSize(int);
    
    
    void setPatternSize(int);
    void setPattern(QPixmap);
    void setScale(int);
    void setScaleUnit(int);
    void setId(QString id);
    void removeItemFromScene(QString);
private slots:
    void sendItemToAll(VisualItem*);
    
protected:
    /**
    * @brief catches move event with the mouse, useful for allowing move of item
    */
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /**
    * @brief catches mouse Press event, defines the first point of the next added item.
    */
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /**
    * @brief  catches release event, defines the last point of the next added item.
    */
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    
    /**
    * @brief adds item depending of the current tool.
    */
    void addItem();
    /*private slots:
    void editingFinished();*/
    
    void generateBackground();

    void addNewItem(VisualItem* item);
    
    
private:
    /**
    * @brief width of the map
    */
    int m_width;
    /**
    * @brief height of the map
    */
    int m_height;
    /**
    * @brief title of the map
    */
    QString m_title;
    /**
    * @brief background color
    */
    QColor m_bgColor;
    /**
    * @brief current tool
    */
    VToolsBar::SelectableTool m_selectedtool;
    /**
    * @brief first point of the next item
    */
    QPointF m_first;
    /**
    * @brief last point of the next item
    */
    QPointF m_end;
    /**
    * @brief current selected item
    */
    VisualItem* m_currentItem;
    /**
    * @brief color for items
    */
    QColor m_itemColor;
    /**
    * @brief pen size
    */
    int m_penSize;
    /**
    * @brief npc size
    */
    int m_npcSize;
    /**
    * @brief Items list which are part of the map.
    */
    QMap<QString,VisualItem*>* m_itemMap;
    /**
    * @brief Pattern Of grid, pattern must be square shaped.
    */
    QPixmap m_gridPattern;
    /**
    * @brief size of the pattern edge.
    */
    int m_sizePattern;
    /**
    * @brief scale of on Pattern edge.
    */
    int m_patternScale;
    /**
    * @brief unit of Pattern scale.
    */
    SCALE_UNIT m_patternUnit;

    QString m_id;
	bool m_localIsGM;
	bool m_hasPermissionMode;
	Map::PermissionMode m_currentMode;

    friend QDataStream& operator<<(QDataStream& os,const VMap&);
    friend QDataStream& operator>>(QDataStream& is,VMap&);
};

#endif // VMAP_H
