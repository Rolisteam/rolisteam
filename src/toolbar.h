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


/********************************************************************/
/* DockWidget contenant la barre d'outils.                          */
/********************************************************************/


#ifndef BARRE_OUTILS_H
#define BARRE_OUTILS_H

#include <QAction>
#include <QLineEdit>
#include <QLCDNumber>
#include <QDockWidget>
#include <QActionGroup>

class SelecteurCouleur;
class SelecteurDiametre;
class Map;
/**
 * @brief The ToolBar class is gathering all tool and manages selection of them.
 */
class ToolBar : public QDockWidget
{
    Q_OBJECT
    
public :
    ToolBar(QWidget *parent = 0);
    virtual ~ToolBar();
    void majCouleursPersonnelles();
    QColor donnerCouleurPersonnelle(int numero);
    void updateUi();

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

    // Outils selectionnables par l'utilisateur
    enum Tool {crayon, ligne, rectVide, rectPlein, elliVide, elliPlein, texte, main, ajoutPnj, supprPnj, deplacePerso, etatPerso};

    ToolBar::Tool getCurrentTool() const;
signals:
    void currentToolChanged(ToolBar::Tool);

public slots :
    void incrementeNumeroPnj();
    void changeCouleurActuelle(QColor coul);
    void mettreAJourPnj(int diametre, QString nom);
    void changeMap(Map * map);

    
private :
    void creerActions();
    void creerOutils();

    QWidget *outils;
    QLineEdit *ligneDeTexte;
    QLineEdit *nomPnj;
    QLCDNumber *afficheNumeroPnj;
    SelecteurCouleur *couleur;
    SelecteurDiametre *diametreTrait;
    SelecteurDiametre *m_npcDiameter;
    Map * m_map;
    //SelecteurDiametre* m_pcDiameter;


private slots :
    void razNumeroPnj();
    void changementTaille(bool floating);
    void texteChange(const QString &texte);
    void nomPnjChange(const QString &texte);

    void crayonSelectionne();
    void ligneSelectionne();
    void rectVideSelectionne();
    void rectPleinSelectionne();
    void elliVideSelectionne();
    void elliPleinSelectionne();
    void texteSelectionne();
    void mainSelectionne();
    void ajoutPnjSelectionne();
    void supprPnjSelectionne();
    void deplacePersoSelectionne();
    void etatPersoSelectionne();

    void currentToolHasChanged(QAction*);

    void changeCharacterSize(int size);
    void sendNewCharacterSize(int size);


private:
    QActionGroup* m_actionGroup;
    ToolBar::Tool m_currentTool;
};

#endif
