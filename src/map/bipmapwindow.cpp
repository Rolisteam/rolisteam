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


#include "map/bipmapwindow.h"


#include "map/Carte.h"


#include "variablesGlobales.h"
#include <QScrollBar>


BipMapWindow::BipMapWindow(Carte *uneCarte, QWidget *parent)
    : QScrollArea(parent)
{
	setObjectName("BipMapWindow");
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


	connect(carteAssociee, SIGNAL(commencerDeplacementBipMapWindow(QPoint)),
            this, SLOT(commencerDeplacement(QPoint)));
	connect(carteAssociee, SIGNAL(deplacerBipMapWindow(QPoint)),
            this, SLOT(deplacer(QPoint)));
    connect(m_widgetResizeAct,SIGNAL(triggered()),this,SLOT(fitMapToWindow()));
}


BipMapWindow::~BipMapWindow()
{
    //no need to delete, actionAssociee it is delete when qmenu is deleted
    //m_mainWindow->enleverCarteDeLaListe(carteAssociee->identifiantCarte());
}
//QAction* BipMapWindow::getAssociatedAction() const
//{
//    return carteAssociee;
//}
/*void BipMapWindow::closeEvent(QCloseEvent *event)
{
    hide();
    actionAssociee->setChecked(false);
    event->ignore();
}*/

void BipMapWindow::hideEvent ( QHideEvent * event )
{
    emit visibleChanged(false);
    QScrollArea::hideEvent(event);
}
void BipMapWindow::showEvent ( QShowEvent * event )
{
    emit visibleChanged(true);
   QScrollArea::showEvent(event);
}
//void BipMapWindow::associerAction(QAction *action)
//{
//    actionAssociee = action;
//}


Carte * BipMapWindow::carte()
{
    return carteAssociee;
}


void BipMapWindow::commencerDeplacement(QPoint position)
{
    pointDepart = position;
    horizontalDepart = horizontalScrollBar()->value();
    verticalDepart = verticalScrollBar()->value();
}

void BipMapWindow::deplacer(QPoint position)
{
    QPoint diff = pointDepart - position;
    horizontalScrollBar()->setValue(horizontalDepart + diff.x());
    verticalScrollBar()->setValue(verticalDepart + diff.y());
}
QString BipMapWindow::getMapId()
{
    if(NULL!=carteAssociee)
    {
        return carteAssociee->identifiantCarte();
    }
    else
    {
        return QString();
    }

}

void BipMapWindow::fitMapToWindow()
{
    setWidgetResizable(m_widgetResizeAct->isChecked());
    if(!m_widgetResizeAct->isChecked())
    {
        carteAssociee->resize(m_originalSize);
    }
}

void BipMapWindow::focusInEvent(QFocusEvent * event)
{
    emit activated(carteAssociee);

    QWidget::focusInEvent(event);
}
void BipMapWindow::contextMenuEvent( QContextMenuEvent * event )
{
   /* if(event->modifiers()==Qt::NoModifier)
    {
        QMenu pop;

        event->accept();
        pop.addAction(m_widgetResizeAct);

        pop.exec(event->globalPos());
    }*/
}
