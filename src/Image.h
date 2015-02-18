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
class Image : public SubMdiWindows //public QScrollArea
{
    Q_OBJECT

public :
    Image(QString& m_filename, ImprovedWorkspace *parent = 0);
    ~Image();



    virtual bool defineMenu(QMenu* menu);
    void saveFile(QString & file);

    void openFile(QString& file);

public slots :
    void pointeurMain();
    void pointeurNormal();

protected :
    virtual void closeEvent(QCloseEvent *event);
    bool  eventFilter(QObject *obj,QEvent *e);
    void setUi();

    void contextMenuEvent ( QContextMenuEvent * event );
    virtual void paintEvent ( QPaintEvent * event );
    void wheelEvent(QWheelEvent *event);

private slots:
    void setZoomLevel(double zoomlevel);
    void zoomIn();
    void zoomOut();
    void resizeLabel();
    void zoomLittle();
    void zoomNormal();
    void zoomBig();
    void onFitWindow();

private :
    void fitWindow();
    void createActions();



    QString m_filename;
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
