/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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




#ifndef CARTE_H
#define CARTE_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QFile>
#include <QDataStream>

#include "types.h"

#include "BarreOutils.h"

#include "playersList.h"


class Character;
class DessinPerso;
class Liaison;


/**
 * @brief Display area for map. this widget displays the map and allows users to draw on it. The alpha layer allows
 * to hide part of the picture. Character are displayed thank to widgets
 * DessinPerso.
 */
class Carte : public QWidget
{
    Q_OBJECT


public :
    Carte(QString identCarte, QImage *image, bool masquer = false, QWidget *parent = 0);
    Carte(QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent = 0);

    enum PermissionMode{GM_ONLY, PC_MOVE,PC_ALL };

    void afficheOuMasquePnj(DessinPerso *pnjSeul = 0);
    void toggleCharacterView(Character * character);
    void affichageDuPj(QString idPerso, bool afficher);
    void changerTaillePjCarte(int nouvelleTaille, bool updatePj = true);
    void emettreCarte(QString titre);
    void emettreCarte(QString titre, Liaison * link);
    void emettreTousLesPersonnages();
    void emettreTousLesPersonnages(Liaison * link);
    void dessinerTraceCrayon(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur, bool joueurLocal);
    void dessinerTraceTexte(QString texte, QPoint positionSouris, QRect zoneARafraichir, couleurSelectionee couleur);
    void dessinerTraceGeneral(actionDessin action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur);
    void adapterCoucheAlpha(quint8 intensiteAlpha);
    void lancerDeplacementPersonnage(QString idPerso, QList<QPoint> listePoints);
    //void sauvegarderCarte(QFile &file, QString titre = "");
    void sauvegarderCarte(QDataStream &out, QString titre = "");
    int tailleDesPj();
    bool pjAffiche(QString idPerso);
    QString identifiantCarte();

    /**
     * @brief trouverPersonnage
     * @param idPerso id of the character
     * @return the corresponding DessinPerso or NULL
     */
    DessinPerso* trouverPersonnage(QString idPerso);

    QString getLastSelectedCharacterId();
    bool selectCharacter(QString& id);
    void setPermissionMode(Carte::PermissionMode mode);
    Carte::PermissionMode getPermissionMode();

    void setHasPermissionMode(bool b);
    bool hasPermissionMode();

signals :
    void incrementeNumeroPnj();
    void changeCouleurActuelle(QColor couleur);
    void mettreAJourPnj(int diametre, QString nom);
    void afficherNomsPj(bool afficher);
    void afficherNomsPnj(bool afficher);
    void afficherNumerosPnj(bool afficher);
    void changerTaillePj(int nouvelleTaille);
    void commencerDeplacementCarteFenetre(QPoint position);
    void deplacerCarteFenetre(QPoint position);

public slots :
    void setPointeur(BarreOutils::Tool currentTool);
    void deplacerLesPersonnages();
    void effacerPerso(QString idPerso);
    void addCharacter(Character * person);
    void changeCharacter(Character * person);
    void delCharacter(Character * person);

protected :
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPoint mapToScale(const QPoint &);
    QPoint mapToNormal(const QPoint & p);

private :
    void p_init();
    bool ajouterAlpha(QImage *source, QImage *m_alphaLayer, QImage *destination, const QRect &rect = QRect());
    bool convertirARGB32(QImage *original, QImage *copie);
    QRect zoneARafraichir();
    void dessiner(QPainter &painter);
    void emettreTrace();
    void emettreTrajetPersonnage();
    void actionPnjBoutonEnfonce(QPoint positionSouris);
    void actionPnjBoutonRelache(QPoint positionSouris);
    void actionPnjMouvementSouris(QPoint positionSouris);
    void emettreCarteGeneral(QString titre, Liaison * link = NULL, bool versLiaisonUniquement = false);
    void emettreTousLesPersonnagesGeneral(Liaison * link = NULL, bool versLiaisonUniquement = false);
    DessinPerso* dansDessinPerso(QPoint positionSouris);

    typedef struct
    {
        QString idPersonnage;
        QList<QPoint> trajet;
    } PersoEnMouvement;

    int taillePj;                        // Taille courante des PJ de la carte
    QImage *m_backgroundImage;                        // image de fond affichee dans la fenetre
    QImage *m_originalBackground;                // image qui servira a effacer les annotations
    QImage *m_alphaLayer;                        // image contenant la couche alpha
    QImage *fondAlpha;                    // image temporaire contenant le melange du fond et de la couche alpha
    QImage *effaceAlpha;                // image contenant la couche alpha permettant d'effacer le fond a l'aide du fond original
    bool boutonGaucheEnfonce;            // bouton gauche de la souris enfonce ou pas?
    bool boutonDroitEnfonce;            // bouton droit de la souris enfonce ou pas?


    QPoint m_originePoint;
    QPoint m_mousePoint;

    QPoint m_origineScalePoint;
    QPoint m_scalePoint;
    QRect  m_refreshZone;

    QPoint diffSourisDessinPerso;        // difference entre le coin sup gauche du PNJ selectionne (pnjSelectionne) et la position de la souris au moment du clic
    QRect zoneOrigine;                    // zone a rafraichir au 1er clic de la souris, puis zone precedemment rafraichie
    QRect zoneNouvelle;                    // zone a rafraichir au prochain affichage
    QRect zoneGlobaleCrayon;            // unite de toutes les zone a raffraichir lors du trace du crayon (emise aux autres utilisateurs)
    QCursor pointeur;                    // pointeur actuel de la souris
    DessinPerso *pnjSelectionne;        // pointe sur le PNJ actuellement selectionne (0 si aucun PNJ selectionne)
    DessinPerso *dernierPnjSelectionne;    // pointe sur le dernier PNJ selectionne (0 si aucun PNJ n'a deja ete selection)
    QString idCarte;                    // identifiant de la carte
    QList<QPoint> listePointsCrayon;    // liste des points composant le trace du crayon, qui sera emise aux autres utilisateurs
    QList<QPoint> listeDeplacement;        // liste des points composant le deplacement du perso qui vient d'etre deplace par l'utilisateur
    QList<PersoEnMouvement> mouvements;    // liste des personnages a deplacer, ainsi que leur trajectoire
    Carte::PermissionMode m_currentMode;
    BarreOutils::Tool m_currentTool;
    Player* m_localPlayer;
    qreal m_scaleY;
    qreal m_scaleX;

    bool m_hasPermissionMode;

};

#endif
