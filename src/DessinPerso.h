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
/* Widget contenant 2 labels : l'un contenant la pixmap d'un cercle */
/* representant un personnage, et l'autre contenant son nom et      */
/* eventuellement son numero. Cette classe qui sert principalement  */
/* pour les PNJ est egalement utilisee pour afficher les PJ.        */
/*                                                                  */
/********************************************************************/


#ifndef DESSIN_PERSO_H
#define DESSIN_PERSO_H

#include <QLabel>
#include <QString>
#include <QWidget>
#include <QColor>
#include <QPoint>

//#include "playersList.h"

class DessinPerso : public QWidget
{
    Q_OBJECT

public :
    enum typePersonnage {pj, pnj};

    typedef struct
    {
        QColor couleurEtat;
        QString nomEtat;
    } etatDeSante;

    DessinPerso(QWidget *parent, QString persoId, QString nom, QColor couleurPerso, int taille, QPoint position, typePersonnage leType, int numero = 0);
    void deplacePerso(QPoint position);
    void deplacePerso(int x, int y);
    void diametreCouleurNom(int *diam, QColor *coul, QString *nom);
    void deplaceCentrePerso(QPoint position);
    void afficherPerso();
    void cacherPerso();
    void contourDisque(QPoint *coordonnees);
    void dessinerPersonnage(QPoint positionSouris = QPoint(0,0));
    void afficheOuMasqueOrientation();
    void changerEtat();
    void renommerPerso(QString nouveauNom);
    void changerCouleurPerso(QColor coul);
    void nouvelleOrientation(QPoint uneOrientation);
    void nouvelEtatDeSante(etatDeSante sante, int numeroSante);
    void emettrePnj(QString idCarte);
    void changerEtatDeSante(int numEtat);
    void afficherOrientation(bool afficher);
    bool dansPartieTransparente(QPoint position);
    bool estVisible();
    bool estUnPj();
    bool orientationEstAffichee();
    QString idPersonnage();
    QPoint positionCentrePerso();
    QPoint orientationPersonnage();
    int preparerPourEmission(char *tampon, bool convertirEnPnj = false);
    int tailleDonneesAEmettre();
    int numeroEtatSante();
    void write(QDataStream &out);

    /*void setCharacter(Character* tmp);
    Character* getCharacter();*/

public slots :
    void afficherNomsPj(bool afficher);
    void afficherNomsPnj(bool afficher);
    void afficherNumerosPnj(bool afficher);
    void changerTaillePj(int nouvelleTaille);



private :
    void mettreIntituleAJour();
    void initialiserContour(QImage &disque);

    typePersonnage type;		// Indique si le personnage est un PJ ou un PNJ
    etatDeSante etat;			// Etat se sante actuel du personnage
    int numeroEtat;				// Numero de l'etat de sante dans la liste G_etatsDeSante
    QString nomPerso;			// Nom du personnage
    QString identifiant;		// Identifiant servant a differencier les personnages
    uchar numeroPnj;			// Numero du PNJ
    uchar diametre;				// Diametre du personnage
    QColor couleur;				// Couleur du personnage
    QLabel *disquePerso;		// Contient le pixmap d'un disque representant le personnage
    QLabel *intitulePerso;		// Affiche le nom et le numero du personnage
    bool nomAffiche;			// True si le nom du personnage est actuellement affiche
    bool numeroAffiche;			// True si le numero du PNJ est actuellement affiche
    bool visible;				// True si le personnage est actuellement affiche
    bool orientationAffichee;	// True si l'orientation du personnage est actuellement affichee
    QPoint contour[8];			// 8 points formant le contour du disque (dans l'espace de coordonnees de disquePerso)
    QPoint orientation;			// Orientation du personnage (difference entre le centre du disque et la souris)
    QPoint centre;				// Sert a memoriser la position du centre pendant que le personnage est cache

    //Character * person;


};

#endif
