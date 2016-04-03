/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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

#ifndef CHARACTER_TOKEN_H
#define CHARACTER_TOKEN_H


#include <QLabel>
#include <QString>
#include <QWidget>
#include <QColor>
#include <QPoint>

#include "network/networkmessagewriter.h"
//#include "userlist/playersList.h"
/**
 * @brief The DessinPerso class
 */
class CharacterToken : public QWidget
{
    Q_OBJECT

public :
    enum typePersonnage {pj, pnj};

    typedef struct
    {
        QColor couleurEtat;
        QString nomEtat;
    } etatDeSante;

    CharacterToken(QWidget *parent, QString persoId, QString nom, QColor couleurPerso, int taille, QPoint position, typePersonnage leType, bool showNpcNumber,bool showName,int numero = 0,bool isLocal=false);
    void moveCharacter(QPoint position);
    void moveCharacter(int x, int y);
    void defineToken(int *diam, QColor *coul, QString *nom);
    void moveCharaterCenter(QPoint position);
    void showCharacter();
    void hideCharater();
    void diskBorder(QPoint *coordonnees);
    void drawCharacter(QPoint positionSouris = QPoint(0,0));
    void showOrHideOrientation();
    void changeState();
    void renameCharacter(QString nouveauNom);
    void changeCharacterColor(QColor coul);
    void newOrientation(QPoint uneOrientation);
    void newHealtState(etatDeSante sante, int numeroSante);
    void emettrePnj(QString idCarte);
    void changeHealtStatus(int numEtat);
    void showOrientation(bool afficher);
    bool onTransparentPart(QPoint position);
    bool isVisible();
    bool isPc();
    bool orientationStatus();
    QString getCharacterId();
    QPoint getCharacterCenter();
    QPoint getCharacterOrientation();
    int prepareToSendOff(NetworkMessageWriter* msg, bool convertirEnPnj = false);
    int getSizeForEmiting();
    int getHealtState();
    void write(QDataStream &out);

    /*void setCharacter(Character* tmp);
    Character* getCharacter();*/

public slots :
    void showPcName(bool afficher);
    void showNpcName(bool afficher);
    void showNpcNumber(bool afficher);
    void setPcSize(int nouvelleTaille);



private :
    void updateTitle();
    void initializeBorder(QImage &disque);
	void AddHealthState(const QColor &color, const QString &label);

private:
    typePersonnage type;		// Indique si le personnage est un PJ ou un PNJ
    etatDeSante etat;			// Etat se sante actuel du personnage
    int numeroEtat;				// Numero de l'etat de sante dans la liste G_etatsDeSante
    QString nomPerso;			// Nom du personnage
    QString identifiant;		// Identifiant servant a differencier les personnages
    uchar numeroPnj;			// Numero du PNJ
    uchar diametre;				// Diametre du personnage
    QColor m_color;				// Couleur du personnage
    QLabel *disquePerso;		// Contient le pixmap d'un disque representant le personnage
    QLabel *intitulePerso;		// Affiche le nom et le numero du personnage
    bool m_showNpcName;			// True si le nom du personnage est actuellement affiche
    bool m_showPcName;
    bool m_showNpcNumber;			// True si le numero du PNJ est actuellement affiche
    bool visible;				// True si le personnage est actuellement affiche
    bool orientationAffichee;	// True si l'orientation du personnage est actuellement affichee
    QPoint contour[8];			// 8 points formant le contour du disque (dans l'espace de coordonnees de disquePerso)
    QPoint orientation;			// Orientation du personnage (difference entre le centre du disque et la souris)
    QPoint centre;				// Sert a memoriser la position du centre pendant que le personnage est cache
    bool m_localPerso;

    QList<CharacterToken::etatDeSante> m_healtStateList;

    //Character * person;


};

#endif
