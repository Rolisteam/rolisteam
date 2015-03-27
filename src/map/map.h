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
#ifndef MAP_H
#define MAP_H

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
#include "toolsbar.h"
#include "userlist/playersList.h"
//#include "data/mediacontainer.h"
#include "network/networkmessage.h"

class Character;
class DessinPerso;
class NetworkLink;


/**
 * @brief Display area for map. this widget displays the map and allows users to draw on it. The alpha layer allows
 * to hide part of the picture. Character are displayed thank to widgets
 * DessinPerso.
 */
class Map : public QWidget//, public MediaContainer
{
    Q_OBJECT


public :
    Map(QString localPlayerId,QString identCarte, QImage *image, bool masquer = false, QWidget *parent = 0);
    Map(QString localPlayerId,QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent = 0);

    enum PermissionMode{GM_ONLY, PC_MOVE,PC_ALL };

    void afficheOuMasquePnj(DessinPerso *pnjSeul = 0);
    void toggleCharacterView(Character * character);
	void showPc(QString idPerso, bool afficher);
	void changePjSize(int nouvelleTaille, bool updatePj = true);
    void emettreCarte(QString titre);
    void emettreCarte(QString titre, NetworkLink * link);
    void emettreTousLesPersonnages();
    void emettreTousLesPersonnages(NetworkLink * link);
	void paintPenLine(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur, bool joueurLocal);
	void paintText(QString texte, QPoint positionSouris, QRect zoneARafraichir, SelectedColor couleur);
	void paintOther(NetMsg::Action action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur);
    void adapterCoucheAlpha(quint8 intensiteAlpha);
    void lancerDeplacementPersonnage(QString idPerso, QList<QPoint> listePoints);

    void saveMap(QDataStream &out, QString titre = "");
    int tailleDesPj();
	bool isVisiblePc(QString idPerso);
    QString identifiantCarte();

    /**
     * @brief trouverPersonnage
     * @param idPerso id of the character
     * @return the corresponding DessinPerso or NULL
     */
    DessinPerso* trouverPersonnage(QString idPerso);

    QString getLastSelectedCharacterId();
    bool selectCharacter(QString& id);
    void setPermissionMode(Map::PermissionMode mode);
    Map::PermissionMode getPermissionMode();

    void setHasPermissionMode(bool b);
    bool hasPermissionMode();
public slots :
	void setPointeur(ToolsBar::SelectableTool currentTool);
	void moveAllCharacters();
	void eraseCharacter(QString idPerso);
	void addCharacter(Character * person);
	void changeCharacter(Character * person);
	void delCharacter(Character * person);
	void setCurrentText(QString text);
	void setCurrentNpcName(QString text);
	void setCurrentNpcNumber(int number);
	void setPenSize(int number);
	void setCharacterSize(int number);
    void setNpcNameVisible(bool);
    void setPcNameVisible(bool);
    void setNpcNumberVisible(bool);
	void setLocalIsPlayer(bool b);


signals :
    void incrementeNumeroPnj();
    void changeCurrentColor(QColor couleur);
    void mettreAJourPnj(int diametre, QString nom);
    void afficherNomsPj(bool afficher);
    void afficherNomsPnj(bool afficher);
    void afficherNumerosPnj(bool afficher);
    void changerTaillePj(int nouvelleTaille);
    void commencerDeplacementBipMapWindow(QPoint position);
    void deplacerBipMapWindow(QPoint position);


protected :
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QPoint mapToScale(const QPoint &);
    QPoint mapToNormal(const QPoint & p);

private :
    void p_init();
    void initCursor();
	bool addAlphaLayer(QImage *source, QImage *m_alphaLayer, QImage *destination, const QRect &rect = QRect());
    bool convertirARGB32(QImage *original, QImage *copie);
    QRect zoneARafraichir();
	void paintMap(QPainter &painter);
    void emettreTrace();
    void emettreTrajetPersonnage();
	void processNpcAction(QPoint positionSouris);
	void processNpcActionReleased(QPoint positionSouris);
	void processNpcMove(QPoint positionSouris);
    void emettreCarteGeneral(QString titre, NetworkLink * link = NULL, bool versNetworkLinkUniquement = false);
    void emettreTousLesPersonnagesGeneral(NetworkLink * link = NULL, bool versNetworkLinkUniquement = false);
	DessinPerso* paintCharacter(QPoint positionSouris);
    QColor getFogColor();

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
    Map::PermissionMode m_currentMode;
    ToolsBar::SelectableTool m_currentTool;
    Player* m_localPlayer;
    qreal m_scaleY;
    qreal m_scaleX;

    QCursor* m_stateCursor;
    QCursor* m_pencilCursor;
    QCursor* m_addCursor;
    QCursor* m_delCursor;
    QCursor* m_movCursor;
    QCursor* m_textCursor;
    QCursor* m_orientCursor;
    QCursor* m_pipetteCursor;

    bool m_hasPermissionMode;

	QString m_currentText;
	QString m_currentNpcName;
	int m_currentNpcNumber;
	int m_penSize;
	int m_npcSize;

    bool m_showNpcName;
    bool m_showPcName;
    bool m_showNpcNumber;
	bool m_localIsPlayer;

	QString m_localPlayerId;

};

#endif
