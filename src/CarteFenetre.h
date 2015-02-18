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
 * @brief The CarteFenetre class - is the scroll area which manages the display of map.
 */
class CarteFenetre : public QScrollArea
{
Q_OBJECT

public :
    /**
     * @brief CarteFenetre
     * @param uneCarte - the embedded map
     * @param parent - parent QWidget
     */
    CarteFenetre(Carte *uneCarte, QWidget *parent = 0);
    /**
     *
     */
    ~CarteFenetre();
    /**
     * @brief carte
     * @return
     */
    Carte *carte();
    /**
     * @brief getMapId
     * @return
     */
    QString getMapId();
    /**
     * @brief getAssociatedAction
     * @return
     */
    QAction* getAssociatedAction() const;



signals:
    /**
     * @brief activated
     * @param carte
     */
    void activated(Carte * carte);
    /**
     * @brief visibleChanged
     */
    void visibleChanged(bool);

public slots :
    /**
     * @brief commencerDeplacement
     * @param position
     */
    void commencerDeplacement(QPoint position);
    /**
     * @brief deplacer
     * @param position
     */
    void deplacer(QPoint position);
    /**
     * @brief fitMapToWindow
     */
    void fitMapToWindow();

protected :
    /**
     * @brief focusInEvent
     * @param event
     */
    void focusInEvent(QFocusEvent * event);
    /**
     * @brief contextMenuEvent
     * @param event
     */
    void contextMenuEvent( QContextMenuEvent * event );
    /**
     * @brief hideEvent
     * @param event
     */
    void hideEvent ( QHideEvent * event );
    /**
     * @brief showEvent
     * @param event
     */
    void showEvent ( QShowEvent * event );

private :
    Carte *carteAssociee;
    QPoint pointDepart;
    int horizontalDepart;
    int verticalDepart;
    QAction* m_widgetResizeAct;
    QSize m_originalSize;
};

#endif
