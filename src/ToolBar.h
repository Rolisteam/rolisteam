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
/* DockWidget contenant la barre d'outils.                          */
/*                                                                  */
/********************************************************************/


#ifndef TOOLS_BAR_H
#define TOOLS_BAR_H

#include <QAction>
#include <QLineEdit>
#include <QLCDNumber>
#include <QDockWidget>
#include <QResizeEvent>

#include "SelecteurCouleur.h"
#include "SelecteurDiametre.h"


class ToolsBar : public QDockWidget
{
Q_OBJECT

public :
    ToolsBar(QWidget *parent = 0);
    void majCouleursPersonnelles();
    QColor donnerCouleurPersonnelle(int numero);
    void autoriserOuInterdireCouleurs();


    QColor& currentColor();




    // Outils selectionnables par l'utilisateur
    enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC, DELNPC, MOVECHARACTER, STATECHARACTER};

public slots :
    void incrementeNumeroPnj();
    void changeCurrentColor(QColor color);
    void mettreAJourPnj(int diametre, QString nom);


signals:
    void currentToolChanged(ToolsBar::SelectableTool);
    void currentColorChanged(QColor&);
    void currentPenSizeChanged(int);
    void currentPNCSizeChanged(int);

private :
    void creerActions();
    void creerOutils();

    QWidget *outils;
    QLineEdit *ligneDeTexte;
    QLineEdit *nomPnj;
    QLCDNumber *afficheNumeroPnj;
    ColorSelector *couleur;
    DiameterSelector *diametreTrait;
    DiameterSelector *diametrePnj;
    QActionGroup *m_toolsGroup;
    SelectableTool m_currentTool;

    QAction *actionCrayon;
    QAction *actionLigne;
    QAction *actionRectVide;
    QAction *actionRectPlein;
    QAction *actionElliVide;
    QAction *actionElliPlein;
    QAction *actionTexte;
    QAction *actionMain;
    QAction *actionAjoutPnj;
    QAction *actionSupprPnj;
    QAction *actionDeplacePnj;
    QAction *actionEtatPnj;
    QAction *actionRazChrono;

private slots :
    void razNumeroPnj();
    void changementTaille(bool floating);
    void texteChange(const QString &texte);
    void nomPnjChange(const QString &texte);



    void currentActionChanged(QAction* p);

};

#endif
