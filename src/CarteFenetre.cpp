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


#include "CarteFenetre.h"

#include <QtGui>

#include "Carte.h"


#include "variablesGlobales.h"



CarteFenetre::CarteFenetre(Carte *uneCarte,MainWindow* mainWindow, QWidget *parent)
    : QScrollArea(parent),m_mainWindow(mainWindow)
{
    setObjectName("CarteFenetre");
    setWindowIcon(QIcon(":/map.png"));
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);



    m_widgetResizeAct = new QAction(tr("Map fits window"),this);
    m_widgetResizeAct->setCheckable(true);
    m_widgetResizeAct->setChecked(false);


    carteAssociee = uneCarte;
    m_originalSize = carteAssociee->size();
    setWidget(carteAssociee);
    setViewportMargins(0,0,0,0);

    resize(carteAssociee->width()+4, carteAssociee->height()+4);


    connect(carteAssociee, SIGNAL(commencerDeplacementCarteFenetre(QPoint)),
            this, SLOT(commencerDeplacement(QPoint)));
    connect(carteAssociee, SIGNAL(deplacerCarteFenetre(QPoint)),
            this, SLOT(deplacer(QPoint)));
    connect(m_widgetResizeAct,SIGNAL(triggered()),this,SLOT(fitMapToWindow()));
}


CarteFenetre::~CarteFenetre()
{
    //no need to delete, actionAssociee it is delete when qmenu is deleted
    m_mainWindow->enleverCarteDeLaListe(carteAssociee->identifiantCarte());
}
//QAction* CarteFenetre::getAssociatedAction() const
//{
 //   return actionAssociee;
//}
/*void CarteFenetre::closeEvent(QCloseEvent *event)
{
    hide();
    actionAssociee->setChecked(false);
    event->ignore();
}*/

void CarteFenetre::hideEvent ( QHideEvent * event )
{
    emit visibleChanged(false);
    QScrollArea::hideEvent(event);
}
void CarteFenetre::showEvent ( QShowEvent * event )
{
    emit visibleChanged(true);
   QScrollArea::showEvent(event);
}
//void CarteFenetre::associerAction(QAction *action)
//{
//    actionAssociee = action;
//}


Carte * CarteFenetre::carte()
{
    return carteAssociee;
}


void CarteFenetre::commencerDeplacement(QPoint position)
{
    pointDepart = position;
    horizontalDepart = horizontalScrollBar()->value();
    verticalDepart = verticalScrollBar()->value();
}

void CarteFenetre::deplacer(QPoint position)
{

    QPoint diff = pointDepart - position;
    horizontalScrollBar()->setValue(horizontalDepart + diff.x());
    verticalScrollBar()->setValue(verticalDepart + diff.y());
}
QString CarteFenetre::getMapId()
{
    return carteAssociee->identifiantCarte();
}

void CarteFenetre::fitMapToWindow()
{
    setWidgetResizable(m_widgetResizeAct->isChecked());
    if(!m_widgetResizeAct->isChecked())
    {
        carteAssociee->resize(m_originalSize);
    }
}

void CarteFenetre::focusInEvent(QFocusEvent * event)
{
    emit activated(carteAssociee);

    QWidget::focusInEvent(event);
}
void CarteFenetre::contextMenuEvent( QContextMenuEvent * event )
{
   /* if(event->modifiers()==Qt::NoModifier)
    {
        QMenu pop;

        event->accept();
        pop.addAction(m_widgetResizeAct);

        pop.exec(event->globalPos());
    }*/
}
