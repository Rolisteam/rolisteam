/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CHARACTERSHEETWINDOW_H
#define CHARACTERSHEETWINDOW_H
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "submdiwindows.h"
#include "charactersheetmodel.h"

class CharacterSheetWindow : public SubMdiWindows
{
    Q_OBJECT
public:
    CharacterSheetWindow();

    virtual bool defineMenu(QMenu* menu);
    virtual SubWindowType getType();


protected slots:
    void addLine();
    void addSection();

private:
    QTableView m_view;
    CharacterSheetModel m_model;

    QPushButton m_addSection;
    QPushButton m_addLine;

    QVBoxLayout m_vertiLayout;
    QHBoxLayout m_horizonLayout;
    QWidget m_widget;

};

#endif // CHARACTERSHEETWINDOW_H
