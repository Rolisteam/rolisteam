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

#include "charactersheetwindow.h"
#include "charactersheet.h"
#include "headermodel.h"
#include <QMenu>

CharacterSheetWindow::CharacterSheetWindow()
    //: m_vheader(Qt::Vertical)
{
    setObjectName("CharacterSheet");


    m_addSection = new QAction(tr("Add Section"),this);
    m_addLine= new QAction(tr("Add line"),this);
    m_addCharacterSheet= new QAction(tr("Add CharacterSheet"),this);

    m_view.setModel(&m_model);


    m_widget.setLayout(&m_horizonLayout);
    setWidget(&m_view);
    m_view.setContextMenuPolicy(Qt::CustomContextMenu);


    connect(m_addLine,SIGNAL(triggered()),this,SLOT(addLine()));
    connect(m_addSection,SIGNAL(triggered()),this,SLOT(addSection()));
    connect(m_addCharacterSheet,SIGNAL(triggered()),this,SLOT(addCharacterSheet()));

    connect(&m_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayCustomMenu(QPoint)));
}
bool CharacterSheetWindow::defineMenu(QMenu* menu)
{
    menu->addAction(m_addCharacterSheet);
    menu->addAction(m_addSection);
    menu->addAction(m_addLine);
    return true;
}

SubMdiWindows::SubWindowType CharacterSheetWindow::getType()
{
    return CHARACTERSHEET;
}
void CharacterSheetWindow::addLine()
{
        m_model.addLine(m_view.currentIndex());
}
void CharacterSheetWindow::displayCustomMenu(const QPoint & pos)
{
    QMenu menu(this);

    menu.addAction(m_addCharacterSheet);
    menu.addAction(m_addSection);
    menu.addAction(m_addLine);
    if(!(m_view.indexAt(pos).isValid()))
    {
        m_addSection->setEnabled(true);
        m_addLine->setEnabled(false);
        m_addCharacterSheet->setEnabled(true);
    }
    else
    {
      m_addSection->setEnabled(true);
      m_addLine->setEnabled(true);
      m_addCharacterSheet->setEnabled(true);

    }
    menu.exec(QCursor::pos());
}

void CharacterSheetWindow::addSection()
{
    m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    m_model.addCharacterSheet();
}
