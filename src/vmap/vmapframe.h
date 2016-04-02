/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                                 *
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



#ifndef MAP_FRAME_H
#define MAP_FRAME_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>

#include "rgraphicsview.h"

#include "vmap.h"
#include "data/mediacontainer.h"
#include "vmap/vtoolbar.h"
//#include "MainWindow.h"

/**
    * @brief displays, stores and manages a map and its items
    *
    */
class VMapFrame : public MediaContainer
{
    Q_OBJECT
    
    
public :
	/**
	 * @brief The PermissionMode enum
	 */
    enum PermissionMode{GM_ONLY, PC_MOVE,PC_ALL };
	/**
	 * @brief VMapFrame
	 */
    VMapFrame();
    /**
    * @brief constructor
    * @todo default constructor missing
    */
    VMapFrame(CleverURI* uri,VMap *vmap);
    /**
    * @brief destructor
    */
    ~VMapFrame();
    /**
    * @brief accessor to get the map.
    */
    VMap* getMap();
    /**
    * @brief get the current editing mode.
    */
    int editingMode();
    
    /**
    * @brief fills up the current window menu
    */
    bool defineMenu(QMenu* /*menu*/);
    
    /**
    * @brief reads the map into the given file
    * @param uri of the file
    */
    void openFile(const QString& file);
	/**
	 * @brief hasDockWidget
	 * @return
	 */
    virtual bool hasDockWidget() const ;
	/**
	 * @brief getDockWidget
	 * @return
	 */
    virtual QDockWidget* getDockWidget() ;
	/**
	 * @brief createMap
	 * @return
	 */
    bool createMap();
    /**
     * @brief readFile
     * @return
     */
    virtual bool readFileFromUri();
    /**
     * @brief openMedia
     * @return
     */
    virtual bool openMedia();
	/**
	 * @brief processAddItemMessage should add items from Network
	 * @param msg
	 */
    void processAddItemMessage(NetworkMessageReader* msg);
	/**
	 * @brief processDelItemMessage deletes items from network order
	 * @param msg
	 */
    void processDelItemMessage(NetworkMessageReader* msg);
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
	 * @brief processMapPropertyChange
	 * @param msg
	 */
    void processMapPropertyChange(NetworkMessageReader* msg);
    /**
     * @brief processSetParentItem
     * @param msg
     */
    void processSetParentItem(NetworkMessageReader* msg);
	/**
	 * @brief saveMedia
	 */
    void saveMedia();
    /**
     * @brief processGeometryViewChange
     * @param msg
     */
    void processGeometryViewChange(NetworkMessageReader* msg);
    /**
     * @brief processOpacityMessage
     * @param msg
     */
    void processOpacityMessage(NetworkMessageReader *msg);
    /**
     * @brief getMediaId
     * @return
     */
    virtual QString getMediaId();
public slots :
	/**
	 * @brief setCleverURI
	 * @param uri
	 */
    virtual void setCleverURI(CleverURI* uri);
    /**
    *  @brief change the current mice cursor
    *  @param new selected QCursor
    */
    virtual void currentCursorChanged(QCursor*);
    /**
    *  @brief change the current drawing tool
    *  @param  new selected tool
    */
    virtual void currentToolChanged(VToolsBar::SelectableTool);
    /**
    *  @brief change the current color
    *  @param  new color
    */
    virtual void currentColorChanged(QColor& );
    
    /**
    *  @brief change the pen size
    *  @param  new size
    */
    virtual void currentPenSizeChanged(int);
    /**
    *  @brief change the current editing  behavior to MaskMode.
    */
    virtual void setEditingMode(int);
	/**
	 * @brief setCurrentNpcNameChanged
	 */
    virtual void setCurrentNpcNameChanged(QString);
	/**
	 * @brief setCurrentNpcNumberChanged
	 */
    virtual void setCurrentNpcNumberChanged(int);
    
signals:
    /**
     * @brief defineCurrentTool
     * @param tool
     */
    void defineCurrentTool(VToolsBar::SelectableTool tool);
protected :
    /**
    *  @brief catches the closeEvent to hide itself (not delete)
    *  @param event discribe the context of the event
    */
    void closeEvent(QCloseEvent *event);
    /**
    *  @brief called when painting the widget is required
    *  @param event discribe the context of the event
    */
    //virtual void paintEvent(QPaintEvent* event);
    virtual void keyPressEvent ( QKeyEvent * event ) ;
    
    /**
    *  @brief catches the mousePressEvent to active the selection and edition
    *  @param event discribe the context of the event
    */
    virtual void mousePressEvent(QMouseEvent* event);
    
private slots:
	/**
	 * @brief updateTitle
	 */
	void updateTitle();
    
private: //functions
	/**
	 * @brief createView
	 */
    void createView();
	/**
	 * @brief updateMap
	 */
    void updateMap();
	/**
	 * @brief visibilityModeToText
	 * @return
	 */
	QString visibilityModeToText(VMap::VisibilityMode);
	/**
	 * @brief permissionModeToText
	 * @return
	 */
	QString permissionModeToText(Map::PermissionMode);
	/**
	 * @brief layerToText
	 * @return
	 */
	QString layerToText(VisualItem::Layer);


private: //members
    /**
    *  pointer to the map, the place where all items are added and displayed
    */
    VMap* m_vmap;    
    /**
    * Pointer to the graphicView, the widget (viewport) which displays the scene
    */
    RGraphicsView* m_graphicView;    
    /**
    * current edition mode
    */
    int m_currentEditingMode;


};

#endif
