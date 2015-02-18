/***************************************************************************
 *      Copyright (C) 2007 by Romain Campioni                              *
 *      Copyright (C) 2009 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef DISPLAY_DISK_H
#define DISPLAY_DISK_H

#include <QWidget>


/**
 *
 * @brief Display a disk
 * @version 1.0
 * @date 10/01/2009
 *
 */
class DisplayDisk : public QWidget
{
    Q_OBJECT

public :
    /**
     * @brief Constructor
     *
     * @param parent its parent widget
     * @param fill the disk must be filled or not.
     * @param minimum set the minimum diameter
     */
    DisplayDisk(QWidget *parent = 0, bool fill = true, int minimum = 1,int maximum = 50);

signals:
    void diameterChanged(int);

public slots :
    /**
     * @brief Change the diameter of the disk and call to redraw it
     *
     * @param newDiameter the new diameter
     */
    void changeDiameter(int newDiameter);
protected:
    /**
     * @brief Overwriting the paintevent. To draw the disk.
     *
     * @param event gathering many details about the event.
     */
    void paintEvent(QPaintEvent *event);
    void wheelEvent ( QWheelEvent * event );
private :
    /**
     * store the current diameter
     */
    int m_currentDiameter;

    /**
     * store the minimum diameter
     */
    int m_minimumDiameter;
    /**
     * store the maximum diameter
     */
    int m_maximumDiameter;

    /**
     * store fill state
     */
    bool m_fill;
};

#endif
