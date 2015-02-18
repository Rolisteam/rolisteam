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


/********************************************************************/
/*                                                                  */
/* Editeur de notes permettant aux utilisateurs de prendre des      */
/* notes en partie                                                  */
/*                                                                  */
/********************************************************************/	


#ifndef EDITEUR_NOTES_H
#define EDITEUR_NOTES_H

#include <QWidget>
#include <QMainWindow>
#include <QTextEdit>
#include <QCloseEvent>
#include <QToolBar>
#include <QComboBox>
#include <QFile>

#include "mainwindow.h"

class EditeurNotes : public QMainWindow
{
Q_OBJECT

public :
    EditeurNotes(MainWindow* mainWindow);
    void sauvegarderNotes(QDataStream &file);
    void ouvrirNotes(QDataStream &file);
    QSize sizeHint() const;

private :
    QTextEdit *notes;			// Contient toutes les notes de l'utilisateur
    QToolBar *barreFontes;		// Contient les boutons de formatage du texte
    QComboBox *selecteurTaille;	// Permet de choisir la taille de la fonte
    MainWindow* m_mainWindow;

private slots :
    void styleNormal();
    void styleGras();
    void styleItalique();
    void styleSouligne();
    void mettreTailleAJour();
    void changementTaille(int index);

protected :
    void closeEvent(QCloseEvent *event);

};

#endif
