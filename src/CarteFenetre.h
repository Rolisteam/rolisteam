/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


/********************************************************************/
/* Encapsule une Carte dans un QScollArea.                          */
/********************************************************************/    


#ifndef CARTE_FENETRE_H
#define CARTE_FENETRE_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QScrollArea>

#include "mainwindow.h"

class Carte;

/**
 * @brief The CarteFenetre class
 */
class CarteFenetre : public QScrollArea
{
Q_OBJECT

public :
    CarteFenetre(Carte *uneCarte,MainWindow* mainWindow, QWidget *parent = 0);
    ~CarteFenetre();
    //void associerAction(QAction *action);
    Carte *carte();
    QString getMapId();
    //QAction* getAssociatedAction() const;



signals:
    void activated(Carte * carte);
    void visibleChanged(bool);

public slots :
    void commencerDeplacement(QPoint position);
    void deplacer(QPoint position);
    void fitMapToWindow();

protected :
    //void closeEvent(QCloseEvent *event);
    void focusInEvent(QFocusEvent * event);
    void contextMenuEvent( QContextMenuEvent * event );
    void hideEvent ( QHideEvent * event );
    void showEvent ( QShowEvent * event );

private :
    Carte *carteAssociee;
    //QAction *actionAssociee;
    QPoint pointDepart;
    int horizontalDepart;
    int verticalDepart;
    MainWindow* m_mainWindow;
    QAction* m_widgetResizeAct;
    QSize m_originalSize;
};

#endif
