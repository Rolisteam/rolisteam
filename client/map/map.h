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

//#include "types.h"
#include "widgets/colorselector.h"
#include "toolsbar.h"
#include "userlist/playersList.h"
//#include "data/mediacontainer.h"
#include "network/networkmessage.h"

class Character;
class CharacterToken;
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

    virtual ~Map();

    enum PermissionMode{GM_ONLY, PC_MOVE,PC_ALL };

    void showHideNPC(CharacterToken *pnjSeul = nullptr);
    void toggleCharacterView(Character * character);
	void showPc(QString idPerso, bool afficher);
	void changePcSize(int nouvelleTaille, bool updatePj = true);
    void sendMap(QString titre);
    void sendMap(QString titre, QString idPlayer);
    void sendOffAllCharacters();
    void sendOffAllCharacters(QString idPlayer);
    void paintPenLine(QList<QPoint> *listePoints, QRect zoneToRefresh, quint8 diametre, SelectedColor couleur, bool joueurLocal);
    void paintText(QString texte, QPoint positionSouris, QRect zoneToRefresh, SelectedColor couleur);
    void paintOther(NetMsg::Action action, QPoint depart, QPoint arrivee, QRect zoneToRefresh, quint8 diametre, SelectedColor couleur);
    void adaptAlphaLayer(quint8 intensiteAlpha);
    void startCharacterMove(QString idPerso, QList<QPoint> listePoints);

    void saveMap(QDataStream &out, QString titre = "");
    int getPcSize();
	bool isVisiblePc(QString idPerso);
    QString getMapId();

    /**
     * @brief trouverPersonnage
     * @param idPerso id of the character
     * @return the corresponding DessinPerso or nullptr
     */
    CharacterToken* findCharacter(QString idPerso);

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
    void increaseNpcNumber();
    void changeCurrentColor(QColor couleur);
    void updateNPC(int diametre, QString nom);
    void showPcName(bool afficher);
    void showNpcName(bool afficher);
    void showNpcNumber(bool afficher);
    void setPcSize(int nouvelleTaille);
    void startBipmapMove(QPoint position);
    void moveBipMapWindow(QPoint position);
    void permissionModeChanged();


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
    bool convertToARGB32(QImage *original, QImage *copie);
    QRect zoneToRefresh();
	void paintMap(QPainter &painter);
    void sendTrace();
    void sendCharacterPath();
	void processNpcAction(QPoint positionSouris);
	void processNpcActionReleased(QPoint positionSouris);
	void processNpcMove(QPoint positionSouris);
    void sendOffGlobalMap(QString titre, QString idPlayer = QString(),  bool versNetworkLinkUniquement = false);
    void sendOffAllGlobalCharacters(QString idPlayer = QString(), bool versNetworkLinkUniquement = false);
	CharacterToken* paintCharacter(QPoint positionSouris);
    QColor getFogColor();

    typedef struct
    {
        QString idCharacter;
        QList<QPoint> motion;
    } CharacterMotion;

    int m_pcSize;                        // Taille courante des PJ de la carte
    QImage *m_backgroundImage;                        // image de fond affichee dans la fenetre
    QImage *m_originalBackground;                // image qui servira a effacer les annotations
    QImage *m_alphaLayer;                        // image contenant la couche alpha
    QImage *m_alphaBg;                    // image temporaire contenant le melange du fond et de la couche alpha
    QImage *m_eraseAlpha;                // image contenant la couche alpha permettant d'effacer le fond a l'aide du fond original
    bool m_leftButtonStatus;            // bouton gauche de la souris enfonce ou pas?
    bool m_rightButtonStatus;            // bouton droit de la souris enfonce ou pas?


    QPoint m_originPoint;
    QPoint m_mousePoint;

    QPoint m_originScalePoint;
    QPoint m_scalePoint;
    QRect  m_refreshZone;


    QPoint m_distanceBetweenMouseAndNPC;        // difference entre le coin sup gauche du PNJ selectionne (pnjSelectionne) et la position de la souris au moment du clic
    QRect m_origZone;                    // zone a rafraichir au 1er clic de la souris, puis zone precedemment rafraichie
    QRect m_newZone;                    // zone a rafraichir au prochain affichage
    QRect m_penGlobalZone;            // unite de toutes les zone a raffraichir lors du trace du crayon (emise aux autres utilisateurs)
    QCursor m_mouseCursor;                    // pointeur actuel de la souris
    CharacterToken *m_selectedNpc;        // pointe sur le PNJ actuellement selectionne (0 si aucun PNJ selectionne)
    CharacterToken *m_lastSelectedNpc;    // pointe sur le dernier PNJ selectionne (0 si aucun PNJ n'a deja ete selection)
    QString m_mapId;                    // identifiant de la carte
    QList<QPoint> m_penPointList;    // liste des points composant le trace du crayon, qui sera emise aux autres utilisateurs
    QList<QPoint> m_characterMoveList;        // liste des points composant le deplacement du perso qui vient d'etre deplace par l'utilisateur
    QList<CharacterMotion> m_motionList;    // liste des personnages a deplacer, ainsi que leur trajectoire
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
