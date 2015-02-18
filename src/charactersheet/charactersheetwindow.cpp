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
#include <QFile>
#include <QFileDialog>
#include "preferencesmanager.h"

CharacterSheetWindow::CharacterSheetWindow(QWidget* parent)
    : SubMdiWindows(parent)
{
    setObjectName("CharacterSheet");

    setWindowIcon(QIcon(":/resources/icons/treeview.png"));
    m_addSection = new QAction(tr("Add Section"),this);
    m_addLine= new QAction(tr("Add line"),this);
    m_addCharacterSheet= new QAction(tr("Add CharacterSheet"),this);
    m_saveCharacterSheet = new QAction(tr("Save Character Sheets"),this);
    m_openCharacterSheet= new QAction(tr("Open Character Sheets"),this);
    m_view.setModel(&m_model);
    setWindowTitle(tr("Character Sheet Viewer"));

    m_widget.setLayout(&m_horizonLayout);
    setWidget(&m_view);
    m_view.setContextMenuPolicy(Qt::CustomContextMenu);


    connect(m_addLine,SIGNAL(triggered()),this,SLOT(addLine()));
    connect(m_addSection,SIGNAL(triggered()),this,SLOT(addSection()));
    connect(m_addCharacterSheet,SIGNAL(triggered()),this,SLOT(addCharacterSheet()));

    connect(m_saveCharacterSheet,SIGNAL(triggered()),this,SLOT(saveCharacterSheet()));
    connect(m_openCharacterSheet,SIGNAL(triggered()),this,SLOT(openCharacterSheet()));

    connect(&m_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayCustomMenu(QPoint)));


}
bool CharacterSheetWindow::defineMenu(QMenu* menu)
{
    menu->setTitle(tr("Character Sheet Viewer"));
    menu->addAction(m_addCharacterSheet);
    menu->addAction(m_addSection);
    menu->addAction(m_addLine);
    menu->addSeparator();
    menu->addAction(m_openCharacterSheet);
    menu->addAction(m_saveCharacterSheet);

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
    menu.addSeparator();
    menu.addAction(m_openCharacterSheet);
    menu.addAction(m_saveCharacterSheet);


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
void  CharacterSheetWindow::saveCharacterSheet()
{
    if(m_fileUri.isEmpty())
    {
        m_fileUri = QFileDialog::getSaveFileName(this, tr("Save Character Sheets"), m_options->value(QString("DataDirectory"),QVariant(".")).toString(),
                tr("Xml files (*.xml)"));
    }
    saveFile(m_fileUri);

}
void CharacterSheetWindow::saveFile(QString & file)
{
    if(!m_fileUri.isEmpty())
    {
        QFile url(m_fileUri);
        if(!url.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream in(&url);
        m_model.writeModel(in,true);
        url.close();
    }
}

void CharacterSheetWindow::openFile(QString& m_fileUri)
{
    if(!m_fileUri.isEmpty())
    {
        QFile url(m_fileUri);
        if(!url.open(QIODevice::ReadOnly))
            return;


        m_model.readModel(url);
        url.close();
    }
}

void CharacterSheetWindow::openCharacterSheet()
{
    m_fileUri = QFileDialog::getOpenFileName(this, tr("Save Character Sheets"), m_options->value(QString("DataDirectory"),QVariant(".")).toString(),
                    tr("Xml files (*.xml)"));

    openFile(m_fileUri);

}
void CharacterSheetWindow::closeEvent(QCloseEvent *event)
{
    setVisible(false);
    event->ignore();
}
