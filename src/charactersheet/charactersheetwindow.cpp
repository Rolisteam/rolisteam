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
 #include <QHeaderView>
CharacterSheetWindow::CharacterSheetWindow()
    //: m_vheader(Qt::Vertical)
{
    setObjectName("CharacterSheet");


    m_addSection.setText(tr("Add Section"));
    m_addLine.setText(tr("Add line"));
    m_addCharacterSheet.setText(tr("Add CharacterSheet"));
    m_vertiLayout.addWidget(&m_addCharacterSheet);
    m_vertiLayout.addWidget(&m_addSection);
    m_vertiLayout.addWidget(&m_addLine);
    //HeaderModel* tmp= new HeaderModel;

    QHBoxLayout horizonLayout;
    //m_vheader.setModel(tmp);
    //m_vheader.setEditTriggers(QAbstractItemView::DoubleClicked);
    //m_view.setVerticalHeader(&m_vheader);
    //m_view.verticalHeader()->setModel(tmp);
    m_horizonLayout.addLayout(&m_vertiLayout);
    m_horizonLayout.addWidget(&m_view);
    m_view.setModel(&m_model);


    m_widget.setLayout(&m_horizonLayout);
    setWidget(&m_widget);



    connect(&m_addLine,SIGNAL(clicked()),this,SLOT(addLine()));
    connect(&m_addSection,SIGNAL(clicked()),this,SLOT(addSection()));
    connect(&m_addCharacterSheet,SIGNAL(clicked()),this,SLOT(addCharacterSheet()));
}
bool CharacterSheetWindow::defineMenu(QMenu* menu)
{
    return false;
}

SubMdiWindows::SubWindowType CharacterSheetWindow::getType()
{
    return CHARACTERSHEET;
}
void CharacterSheetWindow::addLine()
{

}

void CharacterSheetWindow::addSection()
{
    m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    m_model.addCharacterSheet();
}
