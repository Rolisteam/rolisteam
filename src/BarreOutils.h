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
class Carte;
/**
 * @brief The BarreOutils class is gathering all tool and manages selection of them.
 */
class BarreOutils : public QDockWidget
{
    Q_OBJECT
    
public :
    BarreOutils(QWidget *parent = 0);
    virtual ~BarreOutils();
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

    BarreOutils::Tool getCurrentTool() const;
signals:
    void currentToolChanged(BarreOutils::Tool);

public slots :
    void incrementeNumeroPnj();
    void changeCouleurActuelle(QColor coul);
    void mettreAJourPnj(int diametre, QString nom);
    void changeMap(Carte * map);

    
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
    Carte * m_map;
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
    BarreOutils::Tool m_currentTool;
};

#endif
