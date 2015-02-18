/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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



#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>
#include <QImage>
#include <QScrollArea>
#include <QFile>
#include <QString>
#include <QLabel>

#include "submdiwindows.h"



class QScrollArea;
class QLabel;
class QSpinBox;
class QSlider;
class ImprovedWorkspace;
/**
* @brief Image loads and  displays images into the ImprovedWorkspace, it herits from submdiWindows
* It provides useful features such as zoom level and size calculation to fit the window size
*
*/
class Image : public SubMdiWindows //public QScrollArea
{
    Q_OBJECT

public :
    Image(ImprovedWorkspace *parent = 0);
	/**
	*  @brief Constructor
        *  @param CleverURI uri to the image file
	*  @param parent pointer to the parent widget
	*/
    Image(CleverURI* uri, ImprovedWorkspace *parent = 0);
	/**
	* @brief destructor
	*
	*
	*/
    ~Image();

	/**
	* @brief implementation of abstract function which set all menu items
	* @param fill up the menu associated with this kind instance of Image
	* @return bool either if there is menu or not
	*/


    virtual bool defineMenu(QMenu* menu);
	/**
	* @brief should be the only way to save the image content
	* @param QString file : uri to the local file.
	*/
    void saveFile(const QString & file);

	/**
	* @brief should be the only way to open an Image 
	* @param QString file : uri to the local file 
	*/
    void openFile(const QString& file);


    virtual bool hasDockWidget() const ;
    virtual QDockWidget* getDockWidget() ;

public slots :

	/**
	* @brief is called when the mouse pointer must be the hand 
	*/
    void pointeurMain();
	/**
	* @brief should be the only way to open an Image 
	*/
    void pointeurNormal();

protected :
	/**
	* @brief is used for handle the close event and hide the subwindows (not close it) 
	* @param event : event context 
	*/
    virtual void closeEvent(QCloseEvent *event);
	/**
	* @brief is used for handle any event for other widget 
	* @param obj : original event recipient 
	* @param e : event context,type... 
	*/
    bool  eventFilter(QObject *obj,QEvent *e);
	/**
	* @brief get together all functions calls to set the user interface 
	*/
    void setUi();

	/**
	* @brief called when the user asks a contextual menu 
	* @param event : describe the context of the event (position, mouse button ...)
	*/
    void contextMenuEvent ( QContextMenuEvent * event );
	/**
	* @brief draw the widget 
	* @param event : define few details about the painting (size ..)
	*/
    virtual void paintEvent ( QPaintEvent * event );
	/**
	* @brief called when users roll the wheel of their mouse
	* @param event : define few parameters about the action (way,..) 
	*/
    void wheelEvent(QWheelEvent *event);

private slots:
	/**
	* @brief generic function to set the zoom level and refresh the picture
	* @param zoomlevel : new zoomlevel value
	*/
    void setZoomLevel(double zoomlevel);
	/**
	* @brief default function to zoomIn, Add 0.2 to the zoom level and then refresh the picture
	*/
    void zoomIn();
	/**
	* @brief default function to zoomOut, substract 0.2 to the zoom level and then refresh the picture
	*/
    void zoomOut();
	/**
	* @brief refresh the size of the label (which embeds the picture) 
	*/
    void resizeLabel();
	/**
	* @brief sets the zoomlevel to 0.2
	*/
    void zoomLittle();
	/**
	* @brief sets the zoom level to 1 
	*/
    void zoomNormal();
	/**
	* @brief sets the zoom level to 4 
	*/
    void zoomBig();
	/**
	* @brief resize the window and sets current size as zoomlevel 1.
	*/
    void onFitWindow();

private :
	/**
	* @brief adapt the size window to fit the MdiArea size and no scrollbar (if possible)
	*/
    void fitWindow();
	/**
	* @brief create and define alls action for this picture, those actions will be used for context menu or active window menu
	*/
    void createActions();




    QLabel * m_labelImage;
    //QImage* m_image;

    QScrollArea* m_scrollArea;
    double m_zoomLevel;
    QPixmap  m_pixMap;

    QAction* m_actionZoomIn;
    QAction* m_actionZoomOut;
    QAction* m_actionfitWorkspace;
    QAction* m_actionNormalZoom; // *1
    QAction* m_actionBigZoom;// * 4
    QAction* m_actionlittleZoom;// * 0.2

    ImprovedWorkspace *m_parent;

    QSize m_NormalSize;
    QSize m_windowSize;
};

#endif
