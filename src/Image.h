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


/********************************************************************/
/*                                                                  */
/* QScrollArea contenant une image, qui ne peut etre fermee que par */
/* le joueur qui l'a ouverte ou par le MJ. Le but est de pouvoir    */
/* afficher des images sans passer par les cartes qui utilisent     */
/* beaucoup de ressources systeme.                                  */
/*                                                                  */
/********************************************************************/	


#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>
#include <QImage>
#include <QScrollArea>
#include <QAction>
#include <QFile>
#include <QString>
#include <QLabel>

#include "WorkspaceAmeliore.h"
#include "mainwindow.h"


class Liaison;
class NetworkMessageWriter;
class QShortcut;
class Image : public QScrollArea
{
Q_OBJECT

public :
    Image(MainWindow* mainWindow,QString identImage, QString identJoueur, QImage *image, QAction *action = 0, WorkspaceAmeliore *parent = 0);
    ~Image();
    void associerAction(QAction *action);
    void fill(NetworkMessageWriter & message) const;
    void sauvegarderImage(QFile &file, QString titre);
    void sauvegarderImage(QDataStream& out, QString titre);
    bool proprietaireImage();
    QString identifiantImage();
    void setParent(WorkspaceAmeliore *parent);

public slots :
    void pointeurMain();
    void pointeurNormal();


protected:
    /**
    * @brief called when users roll the wheel of their mouse
    * @param event : define few parameters about the action (way,..)
    */
    void wheelEvent(QWheelEvent *event);

    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent ( QContextMenuEvent * event );
    void paintEvent ( QPaintEvent * event );
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
    void createActions();
private :
    MainWindow* m_mainWindow;
    QString idImage;
    QString idJoueur;
    QAction *actionAssociee;
    QLabel *labelImage;
    QPoint pointDepart;
    int horizontalDepart;
    int verticalDepart;
    bool deplacementAutorise;
    double m_zoomLevel;
    WorkspaceAmeliore* m_parent;

    QSize m_NormalSize;
    QSize m_windowSize;
    QPixmap  m_pixMap;

    QAction* m_actionZoomIn;
    QShortcut* m_zoomInShort;

    QAction* m_actionZoomOut;
    QShortcut* m_zoomOutShort;


    QAction* m_actionfitWorkspace;
    QShortcut* m_fitShort;

    QAction* m_actionNormalZoom; // *1
     QShortcut* m_normalShort;

    QAction* m_actionBigZoom;// * 4
    QShortcut* m_bigShort;

    QAction* m_actionlittleZoom;// * 0.2
    QShortcut* m_littleShort;


};

#endif
