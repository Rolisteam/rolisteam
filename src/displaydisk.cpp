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


#include <QtGui>
#include <QWheelEvent>




#include "displaydisk.h"

/**
 * Define the current diameter for any disk
 */
int g_currentDiameterLine;
/**
 * Define the diameter for the NPC
 */
int g_currentNPCDiameter;

DisplayDisk::DisplayDisk(QWidget *parent, bool fill, int minimum,int maximum)
    : QWidget(parent)
{

    m_currentDiameter = minimum;
    m_minimumDiameter = minimum;
    m_maximumDiameter = maximum;
    m_fill = fill;


    if (m_fill)
        g_currentDiameterLine = minimum;
    else
        g_currentNPCDiameter = minimum;
}


void DisplayDisk::paintEvent(QPaintEvent *event)
{
        Q_UNUSED(event)
    int displayedDiameter;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);


    if (m_fill)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        displayedDiameter = m_currentDiameter;
    }
    else
    {
        QPen pen(Qt::black);
        pen.setWidth(4);
        painter.setPen(pen);
        painter.setBrush(Qt::white);
        displayedDiameter = m_currentDiameter - m_minimumDiameter +1;
    }


    painter.drawEllipse((width()-m_currentDiameter)/2, (height()-m_currentDiameter)/2, m_currentDiameter, m_currentDiameter);
    painter.setPen(Qt::darkGray);

    painter.drawText(0, 0, width(), height(), Qt::AlignRight | Qt::AlignBottom, QString::number(displayedDiameter));
}
void DisplayDisk::wheelEvent ( QWheelEvent * event )
{
    int step = event->delta() / 8;

    if(step+m_currentDiameter>m_maximumDiameter)
        m_currentDiameter=m_maximumDiameter;
    else if(step+m_currentDiameter<m_minimumDiameter)
        m_currentDiameter=0;
    else
        m_currentDiameter+=step;
    emit diameterChanged(m_currentDiameter);
    update();
}

void DisplayDisk::changeDiameter(int diameter)
{
    m_currentDiameter = diameter;


    if (m_fill)
        g_currentDiameterLine = diameter;
    else
        g_currentNPCDiameter = diameter;


    update();
}

