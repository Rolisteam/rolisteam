/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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


#include "map/map.h"

#include <QMessageBox>
#include <QUuid>
#include <QBuffer>

#include "network/networkmessagewriter.h"
#include "map/charactertoken.h"
#include "network/networklink.h"
#include "data/persons.h"
#include "userlist/playersList.h"

#include "variablesGlobales.h"
#include <QDebug>
#include "preferences/preferencesmanager.h"



Map::Map(QString localPlayerId,QString identCarte, QImage *image, bool masquer, QWidget *parent)
    : QWidget(parent), idCarte(identCarte),m_hasPermissionMode(true)
{
    m_localPlayerId = localPlayerId;
    m_currentMode = Map::GM_ONLY;
    m_currentTool = ToolsBar::Handler;

    m_originalBackground = new QImage(image->size(), QImage::Format_ARGB32);
    *m_originalBackground = image->convertToFormat(QImage::Format_ARGB32);
    

    m_backgroundImage = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
    *m_backgroundImage = image->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    m_alphaLayer = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);

    QPainter painterAlpha(m_alphaLayer);
    painterAlpha.fillRect(0, 0, image->width(), image->height(),masquer?getFogColor():Qt::white);

    p_init();
}


void Map::p_init()
{
    initCursor();
    effaceAlpha = new QImage(m_originalBackground->size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painterEfface(effaceAlpha);
    painterEfface.fillRect(0, 0, m_originalBackground->width(), m_originalBackground->height(), Qt::black);
    // Ajout de la couche alpha effaceAlpha a l'image de fond originale
	addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground);

    m_localPlayer = PlayersList::instance()->localPlayer();

    fondAlpha = new QImage(m_originalBackground->size(), QImage::Format_ARGB32);



	addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha);

    QColor color(fondAlpha->pixel(10,10));
    QColor color2(m_alphaLayer->pixel(10,10));


    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, PreferencesManager::getInstance()->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());
    setPalette(pal);
    
    // variable Initialisation
	m_npcSize = 12;
    boutonGaucheEnfonce = false;
    boutonDroitEnfonce = false;
    m_mousePoint = QPoint(0,0);
    m_originePoint = QPoint(0,0);
    diffSourisDessinPerso = QPoint(0,0);
    pnjSelectionne = 0;
    dernierPnjSelectionne = 0;

    // Redimentionnement du widget
    resize(m_backgroundImage->size());
    // Initialisation de la liste de points du trace du crayon et de la liste de deplacement du PJ
    listePointsCrayon.clear();
    listeDeplacement.clear();
    // Mise a zero de la zone globale du trace du crayon
    zoneGlobaleCrayon.setRect(0,0,0,0);

    // Initialisation de la liste des mouvement de personnages
    mouvements.clear();

    // Get every characters
    PlayersList* g_playersList = PlayersList::instance();
    int maxPlayersIndex = g_playersList->numPlayers();
    for (int i = 0 ; i < maxPlayersIndex ; i++)
    {
        Player * player = g_playersList->getPlayer(i);
        int maxCharactersIndex = player->getCharactersCount();
        for (int j = 0 ; j < maxCharactersIndex ; j++)
        {
            addCharacter(player->getCharacterByIndex(j));
        }
    }

    // connect to g_playesList to stay tuned
    connect(g_playersList, SIGNAL(characterAdded(Character *)),
            this, SLOT(addCharacter(Character *)));
    connect(g_playersList, SIGNAL(characterDeleted(Character *)),
            this, SLOT(delCharacter(Character *)));
    connect(g_playersList, SIGNAL(characterChanged(Character *)),
            this, SLOT(changeCharacter(Character *)));
}


Map::Map(QString localPlayerId,QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent)
    : QWidget(parent), idCarte(identCarte),m_hasPermissionMode(true)
{
       m_localPlayerId = localPlayerId;

    m_currentMode = Map::GM_ONLY;
    // Les images sont creees en ARGB32_Premultiplied pour beneficier de l'antialiasing

    // Creation de l'image de fond originale qui servira a effacer
    m_originalBackground = new QImage(original->size(), QImage::Format_ARGB32);
    *m_originalBackground = original->convertToFormat(QImage::Format_ARGB32);
    
    // Creation de l'image de fond
    m_backgroundImage = new QImage(avecAnnotations->size(), QImage::Format_ARGB32_Premultiplied);
    *m_backgroundImage = avecAnnotations->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // Creation de la couche alpha qui sera utilisee avec fondAlpha
    m_alphaLayer = new QImage(coucheAlpha->size(), QImage::Format_ARGB32_Premultiplied);
    *m_alphaLayer = coucheAlpha->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    p_init();
}

void Map::initCursor()
{

    //InitMousePointer(&G_pointeurTexte, ":/resources/icones/pointeur texte.png", 4, 13); //strange values here

    //G_pointeurDeplacer
    m_orientCursor  = new QCursor(QPixmap(":/resources/icones/pointeur orienter.png"), 10, 12);
    m_pipetteCursor   = new QCursor(QPixmap(":/resources/icones/pointeur pipette.png"), 1, 19);
    //G_pointeurAjouter
    //G_pointeurSupprimer

    m_addCursor= new QCursor(QPixmap(":/resources/icones/pointeur ajouter.png"), 6, 0);
    m_delCursor = new QCursor(QPixmap(":/resources/icones/pointeur supprimer.png"), 6, 0);
    m_movCursor= new QCursor(QPixmap(":/resources/icones/pointeur deplacer.png"), 0, 0);
    m_textCursor= new QCursor(QPixmap(":/resources/icones/pointeur texte.png"), 4, 13);
    m_pencilCursor = new QCursor(QPixmap(":/resources/icones/pointeur dessin.png"), 8, 8);

    m_stateCursor = new QCursor(QPixmap(":/resources/icones/pointeur etat.png"), 0, 0);


}

void Map::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    // La ruse consiste a passer par une image ARGB32 pour avoir une couche alpha
    // independante des couleurs, puis a lui rajouter la couche alpha
    

    painter.drawImage(event->rect(), *fondAlpha, event->rect());


    //painter.drawImage(event->rect(), *fondAlpha, event->rect());
    //qDebug()<<"zoneNouvelle" << m_refreshZone;
    painter.drawImage(rect(), *fondAlpha, fondAlpha->rect());


    if (boutonGaucheEnfonce == false)
        return;


    if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
        m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        return;
        

	paintMap(painter);


   // painter.drawImage(rect(), *m_alphaLayer, m_alphaLayer->rect());

    /*painter.setBrush(Qt::black);
    painter.fillRect(m_refreshZone,Qt::black);*/
}
QPoint Map::mapToScale(const QPoint & p)
{
    QPoint tmp = p;
    m_scaleX=(qreal)fondAlpha->rect().width()/rect().width();
    m_scaleY=(qreal)fondAlpha->rect().height()/rect().height();
    tmp.setX(tmp.x()*m_scaleX);
    tmp.setY(tmp.y()*m_scaleY);




    return tmp;
}
QPoint Map::mapToNormal(const QPoint & p)
{
    QPoint tmp;
   /* m_origineScalePoint = m_scalePoint;
    m_scalePoint = p;*/
    tmp.setX(p.x()/m_scaleX);
    tmp.setY(p.y()/m_scaleY);
    return tmp;
}
void Map::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToScale(event->pos());


    if ((event->button() == Qt::LeftButton) && !boutonGaucheEnfonce && !boutonDroitEnfonce)
    {

            boutonGaucheEnfonce = true;



            if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
                m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
            {
				if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode)||(Map::PC_MOVE == m_currentMode))
                {
					processNpcAction(pos);
                }
            }

            // Il s'agit de l'outil main
            else if (m_currentTool == ToolsBar::Handler)
            {
                // On initialise le deplacement de la Carte
				//if((!m_localIsPlayer)||
               //   (((m_currentMode == Carte::PC_ALL))))
                {
					emit commencerDeplacementBipMapWindow(mapToGlobal(pos));
                }

        }
        // Il s'agit d'une action de dessin
        else
        {
				if(((!m_localIsPlayer))||
                  (((m_currentMode == Map::PC_ALL))))
                {
                        m_originePoint = m_mousePoint = pos;
                        zoneOrigine = zoneNouvelle = zoneARafraichir();
                        listePointsCrayon.clear();
                        zoneGlobaleCrayon = zoneNouvelle;
                        update();
                }
        }
    }

    // Si l'utilisateur a clique avec le bouton droit
    else if ((event->button() == Qt::RightButton) && !boutonGaucheEnfonce && !boutonDroitEnfonce)
    {

        QPoint positionSouris = pos;
        
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc)
        {
            boutonDroitEnfonce = true;

            setCursor(Qt::WhatsThisCursor);
			DessinPerso *pnj = paintCharacter(positionSouris);
            if (pnj)
            {
                int diametre;
                QColor couleur;
                QString nomPnj;

                pnj->diametreCouleurNom(&diametre, &couleur, &nomPnj);
                emit mettreAJourPnj(diametre, nomPnj);
                emit changeCurrentColor(couleur);
            }
        }
        else if (m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
            boutonDroitEnfonce = true;
            setCursor(*m_orientCursor);
            // On regarde s'il y a un PNJ sous la souris
			DessinPerso *pnj = paintCharacter(positionSouris);
            // Si oui, on modifie son affichage de l'orientation et on le selectionne
            if (pnj)
            {
                pnj->afficheOuMasqueOrientation();
                dernierPnjSelectionne = pnj;

                NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::showCharecterOrientation);
                msg.string8(idCarte);
                msg.string8(pnj->idPersonnage());
                msg.uint8(pnj->orientationEstAffichee());
                msg.sendAll();
            }
            else if (dernierPnjSelectionne)
            {
                boutonDroitEnfonce = true;
                dernierPnjSelectionne->dessinerPersonnage(positionSouris);
            }
        }
        else if(event->modifiers()==Qt::ControlModifier)
        {
            boutonDroitEnfonce = true;
            setCursor(*m_pipetteCursor);
            QColor couleur = QColor(m_backgroundImage->pixel(positionSouris.x(), positionSouris.y()));
            emit changeCurrentColor(couleur);
        }
    }
}


void Map::mouseReleaseEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (event->button() == Qt::LeftButton && boutonGaucheEnfonce)
    {
        // Bouton gauche relache
        boutonGaucheEnfonce = false;

        // Il s'agit d'une action sur les PJ/PNJ
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
            m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
			processNpcActionReleased(pos);
        }

        // Il s'agit de l'outil main
        else if (m_currentTool == ToolsBar::Handler)
        {
            // On ne fait rien
        }

        // Il s'agit d'une action de dessin
        else
        {
            // On recupere la position de la souris
            //QPoint pointSouris = event->pos();

            // Si l'ordinateur local est le serveur on peut dessiner directement sur le plan
            // Dans le cas contraire le plan ne sera mis a jour qu'a la reception du message de trace
            // ayant fait l'aller-retour avec le serveur (necessaire pour conserver la coherence
            // entre les differents utilisateurs : le serveur fait foi)
            if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
            {
                // Creation du painter pour pouvoir dessiner
                QPainter painter;
        
                // Choix de l'image sur laquelle dessiner, en fonction de la couleur actuelle
                if (G_couleurCourante.type == ColorType)
                    painter.begin(m_backgroundImage);
                else if (G_couleurCourante.type == Veil || G_couleurCourante.type == Unveil)
                    painter.begin(m_alphaLayer);
                else if (G_couleurCourante.type == Erase)
                    painter.begin(effaceAlpha);
                else
                {
                    painter.begin(m_backgroundImage);
					//qWarning(tr("Type de couleur incorrecte (mouseReleaseEvent - map.cpp)"));
                }
                painter.setRenderHint(QPainter::Antialiasing);
                


				paintMap(painter);
            }

			if(((!m_localIsPlayer))||
              (((m_currentMode == Map::PC_ALL))))
            {
                 m_mousePoint = pos;
                zoneNouvelle = zoneARafraichir();
            }

            // Idem : seul le serveur peut dessiner directement sur le plan
            if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
            {
                if (G_couleurCourante.type == Erase)
                {
                    // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
					addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground, zoneNouvelle);
                    // Apres quoi on recopie la zone concernee sur l'image de fond
                    QPainter painterFond(m_backgroundImage);
                    painterFond.drawImage(zoneNouvelle, *m_originalBackground, zoneNouvelle);
                    // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
                    QPainter painterEfface(effaceAlpha);
                    painterEfface.fillRect(zoneNouvelle, Qt::black);
                }

                // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
				addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha, zoneNouvelle);
                // Demande de rafraichissement de la fenetre (appel a paintEvent)
                update(/*zoneOrigine.unite(zoneNouvelle)*/);
                // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
                afficheOuMasquePnj();
            }
			if((!m_localIsPlayer)||
              (((m_currentMode == Map::PC_ALL))))
            {

                emettreTrace();
            }

        }        // Fin action de dessin
    }            // Fin bouton gauche relache

    // Si le bouton droit est relache
    else if (event->button() == Qt::RightButton && boutonDroitEnfonce)
    {
        // Bouton droit relache
        boutonDroitEnfonce = false;
        // On restaure le curseur
        setCursor(pointeur);

        // Il s'agit d'une action de deplacement ou de l'action de changement d'etat du perso et qu'un perso a ete selectionne
        if ((m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState) && dernierPnjSelectionne)
        {///@todo test orientation pnj
            QPoint orientation =  dernierPnjSelectionne->orientationPersonnage();
            NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::changeCharacterOrientation);
            msg.string8(idCarte);
            msg.string8(dernierPnjSelectionne->idPersonnage());
            msg.int16(orientation.x());
            msg.int16(orientation.y());
            msg.sendAll();
        }
    }
}


void Map::mouseMoveEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (boutonGaucheEnfonce && !boutonDroitEnfonce)
    {
        // Il s'agit d'une action sur les PJ/PNJ
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc ||
            m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
			processNpcMove(pos);
        }

        // Il s'agit de l'outil main
        else if (m_currentTool == ToolsBar::Handler)
        {
			// On deplace la Carte dans la BipMapWindow
			emit deplacerBipMapWindow(mapToGlobal(pos));
        }

        // Il s'agit d'une action de dessin
        else
        {
			if((!m_localIsPlayer)||
              (((m_currentMode == Map::PC_ALL))))
            {
                //m_originePoint = m_mousePoint;
                m_mousePoint = pos;

                zoneNouvelle = zoneARafraichir();
                update();
                zoneOrigine = zoneNouvelle;
            }
        }
    }

    // Si le bouton droit est enfonce et que le pointeur est dans l'image (evite un plantage)
    else if (boutonDroitEnfonce && !boutonGaucheEnfonce && (m_backgroundImage->rect()).contains(pos))
    {
        // Recuperation de la position de la souris
        QPoint positionSouris = pos;
        
        // Il s'agit d'une action d'ajout ou de suppression de PNJ
        if (m_currentTool == ToolsBar::AddNpc || m_currentTool == ToolsBar::DelNpc)
        {
        }
        // Il s'agit d'une action de deplacement ou de changement d'etat de PNJ
        else if (m_currentTool == ToolsBar::MoveCharacterToken || m_currentTool == ToolsBar::ChangeCharacterState)
        {
            // On met a jour l'orientation du PNJ (si la souris n'est pas sur un PNJ
            // et qu'il en existe un de selectionne)
			if (dernierPnjSelectionne && !paintCharacter(positionSouris))
                dernierPnjSelectionne->dessinerPersonnage(positionSouris);
        }
        else
        {
            QColor couleur = QColor(m_backgroundImage->pixel(positionSouris.x(), positionSouris.y()));
            emit changeCurrentColor(couleur);
        }
    }
}


void Map::paintMap(QPainter &painter)
{
    QColor couleurPinceau;
    

    if (G_couleurCourante.type == ColorType)
        couleurPinceau = G_couleurCourante.color;
    else if (G_couleurCourante.type == Veil)
    {
        couleurPinceau = getFogColor();
    }
    else if(G_couleurCourante.type == Unveil)
        couleurPinceau = Qt::white;
    else if(G_couleurCourante.type == Erase)
        couleurPinceau = Qt::white;
    else
		qWarning() << tr("color type not allowed (paintMap - map.cpp)");


    QPen pen;
	pen.setWidth(m_penSize);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);


    if (m_currentTool == ToolsBar::Pen)
    {

        QPainter painterCrayon;
        
        if (G_couleurCourante.type == ColorType)
            painterCrayon.begin(m_backgroundImage);
        else if (G_couleurCourante.type == Veil || G_couleurCourante.type == Unveil)
            painterCrayon.begin(m_alphaLayer);
        else if (G_couleurCourante.type == Erase)
            painterCrayon.begin(effaceAlpha);
        else
        {
            painter.begin(m_backgroundImage);
			qWarning() << tr("color type not allowed  (paintMap - map.cpp)");
        }
        painterCrayon.setRenderHint(QPainter::Antialiasing);
        
        // M.a.j du pinceau pour avoir des bouts arrondis
        QPen pen;
        pen.setColor(couleurPinceau);
		pen.setWidth(m_penSize);
        pen.setCapStyle(Qt::RoundCap);
        painterCrayon.setPen(pen);
        painter.setPen(pen);
        // On dessine une ligne entre le point d'origine et le pointeur de la souris, sur le fond et sur le widget
        painterCrayon.drawLine(m_originePoint, m_mousePoint);
        painter.drawLine(m_originePoint, m_mousePoint);

        // Dessin d'un point pour permettre a l'utilisateur de ne dessiner qu'un unique point (cas ou il ne deplace pas la souris)
        painter.drawPoint(m_mousePoint);
        
        if (G_couleurCourante.type == Erase)
        {
            // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
			addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground, zoneNouvelle);
            // Apres quoi on recopie la zone concernee sur l'image de fond
            QPainter painterFond(m_backgroundImage);
            painterFond.drawImage(zoneNouvelle, *m_originalBackground, zoneNouvelle);
            // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
            painterCrayon.fillRect(zoneNouvelle, Qt::black);
        }
        
        // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
		addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha, zoneNouvelle);

        // On ajoute la position de la souris a la liste de points destinee a l'emission
        listePointsCrayon.append(m_mousePoint);
        // On agglomere les zones a rafraichir pour obtenir la zone globale du trace du crayon (pour l'emission)
        zoneGlobaleCrayon = zoneGlobaleCrayon.united(zoneNouvelle);

        // M.a.j du point d'origine
        m_originePoint = m_mousePoint;
    }
    
    else if (m_currentTool == ToolsBar::Line)
    {
        // On dessine une ligne entre le point d'origine et le pointeur de la souris
        painter.drawLine(m_originePoint, m_mousePoint);
    }
    
    else if (m_currentTool == ToolsBar::EmptyRect)
    {
        // Creation des points du rectangle a partir du point d'origine et du pointeur de souris
        QPoint supGauche, infDroit;
        supGauche.setX(m_originePoint.x()<m_mousePoint.x()?m_originePoint.x():m_mousePoint.x());
        supGauche.setY(m_originePoint.y()<m_mousePoint.y()?m_originePoint.y():m_mousePoint.y());
        infDroit.setX(m_originePoint.x()>m_mousePoint.x()?m_originePoint.x():m_mousePoint.x());
        infDroit.setY(m_originePoint.y()>m_mousePoint.y()?m_originePoint.y():m_mousePoint.y());

        // Si le rectangle est petit on dessine un rectangle plein (correction d'un bug Qt)
		if ((infDroit.x()-supGauche.x() < m_penSize) && (infDroit.y()-supGauche.y() < m_penSize))
        {
			QPoint delta(m_penSize/2, m_penSize/2);
            painter.fillRect(QRect(supGauche - delta, infDroit + delta), couleurPinceau);
        }
        // Sinon on dessine un rectangle vide
        else
            painter.drawRect(QRect(supGauche, infDroit));
    }
    
    else if (m_currentTool == ToolsBar::FilledRect)
    {
        // On dessine un rectangle plein
        painter.fillRect(QRect(m_originePoint, m_mousePoint), couleurPinceau);
    }
    else if (m_currentTool == ToolsBar::EmptyEllipse)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point d'origine
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;

        // Si l'ellipse est petite on dessine une ellipse pleine (correction d'un bug Qt)
		if (abs(m_mousePoint.x()-nouveauPointOrigine.x()) < m_penSize && abs(m_mousePoint.y()-nouveauPointOrigine.y()) < m_penSize)
        {
            // Redefinition des points du rectangle
            QPoint supGauche, infDroit;
            supGauche.setX(nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x());
            supGauche.setY(nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y());
            infDroit.setX(nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x());
            infDroit.setY(nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y());
            
            // Parametrage pour une ellipse pleine
            QPen pen;
            pen.setColor(couleurPinceau);
            pen.setWidth(0);
            painter.setPen(pen);
            painter.setBrush(couleurPinceau);
			QPoint delta(m_penSize/2, m_penSize/2);
            painter.drawEllipse(QRect(supGauche - delta, infDroit + delta));
        }
        // Sinon on dessine une ellipse vide
        else
            painter.drawEllipse(QRect(nouveauPointOrigine, m_mousePoint));
    }
    
    else if (m_currentTool == ToolsBar::FilledEllipse)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point d'origine
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;

        // On dessine une ellipse pleine    
        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(couleurPinceau);
        painter.drawEllipse(QRect(nouveauPointOrigine, m_mousePoint));
    }
    
    else if (m_currentTool == ToolsBar::Text)
    {
        // Parametrage de la fonte
        QFont font;
        font.setPixelSize(16);
        painter.setFont(font);
        // On affiche le texte de la zone de saisie
		painter.drawText(m_mousePoint, m_currentText);  // + QPoint(2,7)
    }
    
    else if (m_currentTool == ToolsBar::Handler)
    {
    }
            
    else
		qWarning() << tr("undefined drawing tools (paintMap - map.cpp)");
}

QRect Map::zoneARafraichir()
{
    QRect resultat;
    

    QPoint supGauche, infDroit;
    int gauche = m_originePoint.x()<m_mousePoint.x()?m_originePoint.x():m_mousePoint.x();
    int haut = m_originePoint.y()<m_mousePoint.y()?m_originePoint.y():m_mousePoint.y();
    int droit = m_originePoint.x()>m_mousePoint.x()?m_originePoint.x():m_mousePoint.x();
    int bas = m_originePoint.y()>m_mousePoint.y()?m_originePoint.y():m_mousePoint.y();
    // Calcul de la largeur et de la hauteur
    int largeur = droit - gauche + 1;
    int hauteur = bas - haut + 1;

    //qDebug() << "Refresh Haut: " << haut << gauche << droit << bas << m_originePoint << m_mousePoint;

    // Action a effectuer en fonction de l'outil en cours d'utilisation
    if (m_currentTool == ToolsBar::Pen)
    {
		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::Line)
    {
		resultat = QRect(gauche-m_penSize, haut-m_penSize, largeur+m_penSize*2, hauteur+m_penSize*2);
    }
    
    else if (m_currentTool == ToolsBar::EmptyRect)
    {
		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::FilledRect)
    {
        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (m_currentTool == ToolsBar::EmptyEllipse)
    {
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;
        
        int gauche = nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int haut = nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int droit = nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int bas = nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

		resultat = QRect(gauche-m_penSize/2-2, haut-m_penSize/2-2, largeur+m_penSize+4, hauteur+m_penSize+4);
    }
    
    else if (m_currentTool == ToolsBar::FilledEllipse)
    {
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;
        
        int gauche = nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int haut = nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int droit = nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int bas = nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (m_currentTool == ToolsBar::Text)
    {
        resultat = QRect(QPoint(m_mousePoint.x()-2, m_mousePoint.y()-15), QPoint(m_backgroundImage->width(), m_mousePoint.y()+4));
    }
    
    else if (m_currentTool == ToolsBar::Handler)
    {

    } 
    else
		qWarning() <<  (tr("Undefined tool  (paintMap - map.cpp)"));

    /*m_originePoint=m_origineScalePoint;
    m_mousePoint=m_scalePoint;*/
    /*m_originePoint=mapToScale(m_originePoint);
    m_mousePoint=mapToScale(m_mousePoint);*/

   // qDebug() << "Refresh zone1: " << m_backgroundImage->rect() << resultat << m_mousePoint << m_originePoint ;
    m_refreshZone = resultat.intersected(m_backgroundImage->rect());
   //qDebug() << "Refresh zone2: " <<resultat.intersect(m_backgroundImage->rect()) << width() << height();
    return m_refreshZone;//rect();
}


bool Map::addAlphaLayer(QImage *source, QImage *alpha, QImage *destination, const QRect &rect)
{
    if (source->size() != destination->size() || source->size() != alpha->size())
    {
		qWarning() << (tr("Source, destination and alpha layer have not the same size  (addAlphaLayer - map.cpp)"));
        return false;
    }

    // Initialisation de la zone a mettre a jour
    QRect zone = rect;
    if (rect.isNull())
    {
        zone = QRect(0, 0, source->width(), source->height());
    }
    if((zone.height() > source->height())||(zone.width() > source->width()))
    {
        zone.setWidth(source->width());
        zone.setHeight(source->height());
    }

    // Calcule de la position de fin
    int finX = zone.left() + zone.width();
    int finY = (zone.top() + zone.height()) * source->width();

    // Recuperation des pointeurs vers les images
    QRgb *pixelSource = (QRgb *)source->bits();
    QRgb *pixelDestination = (QRgb *)destination->bits();
    QRgb *pixelAlpha = (QRgb *)alpha->bits();

    int x, y;
    // Destination = Source + couche alpha (pas propre, mais rapide)
    for (y=zone.top()*source->width(); y<finY; y+=source->width())
    {
        for (x=zone.left(); x<finX; x++)
        {
            pixelDestination[x+y] = (pixelSource[x+y] & 0x00FFFFFF) | ((pixelAlpha[x+y] & 0x00FF0000) << 8);
        }
    }


        
    return true;
}


void Map::processNpcAction(QPoint positionSouris)
{
    if (m_currentTool == ToolsBar::AddNpc)
    {
		if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode))
        {
            if (G_couleurCourante.type == ColorType)
            {
                // Creation de l'identifiant du PNJ
                QString idPnj = QUuid::createUuid().toString();
                // Creation du dessin du PNJ qui s'affiche dans le widget
                DessinPerso *pnj = new DessinPerso(this, idPnj, m_currentNpcName, G_couleurCourante.color, m_npcSize, positionSouris, DessinPerso::pnj, m_showNpcNumber, m_showNpcName, m_currentNpcNumber);
                pnj->afficherPerso();
                // Un PNJ est selectionne
                pnjSelectionne = pnj;
                // On calcule la difference entre le coin sup gauche du PNJ et le pointeur de la souris
                diffSourisDessinPerso = pnjSelectionne->mapFromParent(positionSouris);
                // Demande d'incrementation du numero de PNJ (envoyee a la barre d'outils)
                emit incrementeNumeroPnj();
            }
        }
    }
    else
    {
		DessinPerso *pnj = paintCharacter(positionSouris);
        if (pnj)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
                    ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnj->idPersonnage())>-1)) )//if not found -1
            {
                if (m_currentTool == ToolsBar::DelNpc)
                {
                        if (!pnj->estUnPj())
                        {
                            if (dernierPnjSelectionne == pnj)
                                dernierPnjSelectionne = 0;

                            // Emission de la demande de suppression de de PNJ
                            NetworkMessageWriter msg(NetMsg::NPCCategory,NetMsg::delNpc);
                            msg.string8(idCarte);
                            msg.string8(pnj->idPersonnage());
                            msg.sendAll();

                            // Destruction du personnage
                            pnj->~DessinPerso();
                        }

                }

                else if (m_currentTool == ToolsBar::MoveCharacterToken)
                {
                            pnjSelectionne = pnj;
                            // On calcule la difference entre le coin sup gauche du PNJ et le pointeur de la souris
                            diffSourisDessinPerso = pnj->mapFromParent(positionSouris);
                            // Mise a zero de la liste de points
                            listeDeplacement.clear();
                            // Ajout de la position actuelle du perso dans la liste
                            listeDeplacement.append(pnj->positionCentrePerso());
                }
                else if (m_currentTool == ToolsBar::ChangeCharacterState)
                {
                        // changement d'etat du personnage
                        pnj->changerEtat();
                        dernierPnjSelectionne = pnj;

                        NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::changeCharacterState);
                        msg.string8(idCarte);
                        msg.string8(pnj->idPersonnage());
                        msg.uint16(pnj->numeroEtatSante());
                        msg.sendAll();
                }
                else
					qWarning() << (tr("undefine tool for processing action on NPC or PC (processNpcAction - map.cpp)"));
            }
    }
}

}


void Map::processNpcActionReleased(QPoint positionSouris)
{
             Q_UNUSED(positionSouris)
    if (m_currentTool == ToolsBar::AddNpc)
    {
        // On verifie que la couleur courante peut etre utilisee pour dessiner un PNJ
        if (G_couleurCourante.type == ColorType)
        {
            if (pnjSelectionne!=NULL)
            {
                // Affiche ou masque le PNJ selon qu'il se trouve sur une zone masquee ou pas
                afficheOuMasquePnj(pnjSelectionne);
                // sauvegarde du dernier PNJ selectionne
                dernierPnjSelectionne = pnjSelectionne;
                // Plus de PNJ selectionne
                pnjSelectionne = 0;
                // Emission du PNJ vers les clients ou le serveur
                dernierPnjSelectionne->emettrePnj(idCarte);
            }
        }

        // S'il s'agit d'une couleur speciale, on affiche une boite d'alerte
        else
        {
            // Creation de la boite d'alerte
            QMessageBox msgBox(this);
            msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle(tr("Inappropriate Color"));
            // On supprime l'icone de la barre de titre
            Qt::WindowFlags flags = msgBox.windowFlags();
            msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
            // On met a jour le message et on ouvre la boite d'alerte
            msgBox.setText(tr("You can not select this color because\n It's a special color."));
            msgBox.exec();
        }
    }
    
    else if (m_currentTool == ToolsBar::DelNpc)
    {
    }
    
    else if (m_currentTool == ToolsBar::MoveCharacterToken)
    {
        if (pnjSelectionne)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
               ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnjSelectionne->idPersonnage())>-1)) )
            {
                // Affiche ou masque le PNJ selon qu'il se trouve sur une zone masquee ou pas
                afficheOuMasquePnj(pnjSelectionne);
                // sauvegarde du dernier PNJ selectionne
                dernierPnjSelectionne = pnjSelectionne;
                // Emission du trajet du personnage
                emettreTrajetPersonnage();
                // Plus de PNJ selectionne
                pnjSelectionne = 0;
            }

        }
    }
    
    else if (m_currentTool == ToolsBar::ChangeCharacterState)
    {
    }

    else
		qWarning() << (tr("undefine tool for processing action on NPC or PC (processNpcActionReleased - map.cpp)"));

}


void Map::processNpcMove(QPoint positionSouris)
{
    if (m_currentTool == ToolsBar::AddNpc)
    {
        // Si un PNJ est selectionne, on le deplace
        if (pnjSelectionne)
        {
            // On verifie que la souris reste dans les limites de la carte
            if ( QRect(0, 0, m_backgroundImage->width(), m_backgroundImage->height()).contains(positionSouris, true) )
                pnjSelectionne->deplacePerso(positionSouris - diffSourisDessinPerso);
        }
    }
    
    else if (m_currentTool == ToolsBar::DelNpc)
    {
    }
    
    else if (m_currentTool == ToolsBar::MoveCharacterToken)
    {
        if (pnjSelectionne)
        {
			if((!m_localIsPlayer)||
               (Map::PC_ALL==m_currentMode)||
               ((Map::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnjSelectionne->idPersonnage())>-1)) )
            {
                // On verifie que la souris reste dans les limites de la carte
                if ( QRect(0, 0, m_backgroundImage->width(), m_backgroundImage->height()).contains(positionSouris, true) )
                {
                    // Deplacement du perso
                    pnjSelectionne->deplacePerso(positionSouris - diffSourisDessinPerso);
                    // Ajout de la position actuelle du perso dans la liste
                    listeDeplacement.append(pnjSelectionne->positionCentrePerso());
                }
            }
        }
    }
    
    else if (m_currentTool != ToolsBar::ChangeCharacterState)
    {
		qWarning() << (tr("undefine tool for processing action on NPC or PC (processNpcMove - map.cpp)"));
    }



}


DessinPerso* Map::paintCharacter(QPoint positionSouris)
{
    DessinPerso *resultat;
    DessinPerso *dessinPnj;
    
    // Recuperation du widget se trouvant sous la souris
    QWidget *widget = childAt(positionSouris);
    
    // S'il y a un widget sous la souris...
    if (widget)
    {
        // Il y a 3 cas de figure possibles :
        // Le widget est un disquePerso
        if (widget->objectName() == QString("disquePerso"))
        {
            // On recupere le parent du disquePerso : un DessinPerso
            dessinPnj = (DessinPerso *) (widget->parentWidget());
            // Si le pointeur se trouve dans la partie transparente de la pixmap...
            if (dessinPnj->dansPartieTransparente(positionSouris))
            {
                // On masque le DessinPerso courant...
                dessinPnj->hide();
                // ...et on regarde derriere lui
				resultat = paintCharacter(positionSouris);
                // Puis on refait apparaitre le DessinPerso
                dessinPnj->show();
            }
            // Si le pointeur n'est pas dans la partie transparente de la pixmap
            else
            {
                // On renvoie le DessinPerso courant
                resultat = dessinPnj;
            }
        }
        // Le widget est un intitulePerso
        else if (widget->objectName() == QString("intitulePerso"))
        {
            // On recupere le parent de l'intitulePerso : un DessinPerso
            dessinPnj = (DessinPerso *) (widget->parentWidget());
            // On masque le DessinPerso courant...
            dessinPnj->hide();
            // ...et on regarde derriere lui
			resultat = paintCharacter(positionSouris);
            // Puis on refait apparaitre le DessinPerso
            dessinPnj->show();
        }
        // Le widget est un DessinPerso
        else if (widget->objectName() == QString("DessinPerso"))
        {
            // On convertit le pointeur en DessinPerso*
            dessinPnj = (DessinPerso *) widget;
            // On masque le DessinPerso courant...
            dessinPnj->hide();
            // ...et on regarde derriere lui
			resultat = paintCharacter(positionSouris);
            // Puis on refait apparaitre le DessinPerso
            dessinPnj->show();
        }
        // Cas qui ne devrait jamais arriver
        else
        {
			qWarning() <<  (tr("unknown widget under cursor (paintCharacter - map.cpp)"));
            resultat = 0;
        }
    }

    // Le pointeur ne se trouve pas sur un widget
    else
        resultat = 0;

    return resultat;
}


void Map::afficheOuMasquePnj(DessinPerso *pnjSeul)
{
    QObjectList enfants;
    int i, j, masque, affiche;
    QPoint contour[8];
    DessinPerso *pnj;
    QRect limites = m_backgroundImage->rect();

    // Si pnjSeul n'est pas nul, on le met en tete de liste, comme seul element
    if (pnjSeul)
        enfants.append(pnjSeul);
    // Sinon on recupere la liste des enfants de la carte (tous des DessinPerso)
    else
        enfants = children();
    
    // Taille de la liste
    int tailleListe = enfants.size();
    
    // Parcours des DessinPerso
    for (i=0; i<tailleListe; i++)
    {
        pnj = (DessinPerso *)(enfants[i]);

        // On ne masque le pesonnage uniquement s'il s'agit d'un PNJ
        if (!pnj->estUnPj())
        {
            // Recuperation des 8 points formant le coutour du disque representant le PNJ
            pnj->contourDisque(contour);

            // Il y a 2 cas de figure :
            // Soit le DessinPerso est visible : on regarde s'il faut le cacher
            if (pnj->estVisible())
            {
                // On verifie la couche alpha de fondAlpha pour chacun des points
                masque = 0;
                for (j=0; j<8; j++)
                {
                    // On verifie que le point se trouve dans les limites de l'image (evite un plantage)
                    if (limites.contains(contour[j]))
                    {
                        if ( qAlpha(fondAlpha->pixel(contour[j].x(), contour[j].y())) == 0)
                            masque++;
                    }
                    else
                        masque++;
                }

                // Si tous les points se trouvent dans une zone masquee, on cache le PNJ
                if (masque == 8)
                    pnj->cacherPerso();
            }

            // Le PNJ est deja cache : on regarde s'il faut l'afficher
            else
            {
                // On verifie la couche alpha de fondAlpha pour chacun des points
                affiche = 0;
                for (j=0; j<8 && !affiche; j++)
                {
                    // On verifie que le point se trouve dans les limites de l'image (evite un plantage)
                    if (limites.contains(contour[j]))
                    {
                        if ( qAlpha(fondAlpha->pixel(contour[j].x(), contour[j].y())) == 255)
                            affiche++;
                    }
                    else
                        affiche++;
                }

                // Si un des points se trouve dans la zone visible, on affiche le PNJ
                if (affiche)
                    pnj->afficherPerso();
            }
        }    // Fin de la condition : le personnage est un PNJ
    }        // Fin du parcours de la liste des enfants
}


void Map::changePjSize(int nouvelleTaille, bool updatePj)
{
	m_npcSize = nouvelleTaille;
    if((updatePj)&&(dernierPnjSelectionne!=NULL))
    {
		dernierPnjSelectionne->changerTaillePj(nouvelleTaille);
    }
		//emit changerm_npcSize(nouvelleTaille);
}

int Map::tailleDesPj()
{
	return m_npcSize;
}


DessinPerso* Map::trouverPersonnage(QString idPerso)
{
    DessinPerso *perso=NULL;
    bool ok = false;
    for (int i=0; i<children().size() && !ok; i++)
    {
        perso = dynamic_cast<DessinPerso *>(children().at(i));
        if(NULL!=perso)
        {
            if (perso->idPersonnage() == idPerso)
            {
                ok = true;
            }
        }

    }

    if (!ok)
        return NULL;

    return perso;
}

void Map::toggleCharacterView(Character * character)
{
    QString uuid = character->uuid();
	bool newState = !isVisiblePc(uuid);
	if((!m_localIsPlayer)||(Map::PC_ALL==m_currentMode)||(Map::PC_MOVE == m_currentMode))
    {
		showPc(uuid, newState);

        NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::ToggleViewPlayerCharacterAction);
        message.string8(idCarte);
        message.string8(uuid);
        message.uint8(newState ? 1 : 0);
        message.sendAll();
    }

}


void Map::showPc(QString idPerso, bool afficher)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (NULL==pj)
    {
		qWarning() << (tr("PC ID not found (showPc - map.cpp)"));
        return;
    }
    // On masque ou on affiche le PJ
    if (afficher)
        pj->afficherPerso();
    else
    {
        pj->cacherPerso();
        // M.a.j de la carte pour eviter les residus dans les autres PJ/PNJ
        update(QRect(pj->pos(), pj->size()));
    }
}


bool Map::isVisiblePc(QString idPerso)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (NULL==pj)
    {
		qWarning() << tr("PC ID: %1 not found (isVisiblePc - map.cpp)") << qPrintable(idPerso);
        return false;
    }

    return pj->estVisible();
}

void Map::addCharacter(Character * person)
{
    new DessinPerso(this, person->uuid(), person->name(), person->color(), m_npcSize, QPoint(m_backgroundImage->width()/2, m_backgroundImage->height()/2), DessinPerso::pj, false, m_showPcName);
}


void Map::eraseCharacter(QString idCharacter)
{
    // Recherche du personnage
	DessinPerso *perso = trouverPersonnage(idCharacter);
    // Ne devrait jamais arriver
     if (NULL==perso)
    {
		qWarning() << tr("No character with this id: %1").arg(idCharacter)<<"(eraseCharacter - map.cpp)";
        return;
    }
    // Suppression du personnage
    perso->~DessinPerso();
}

void Map::delCharacter(Character * person)
{
	if(NULL==person)
		return;

    DessinPerso * pj = trouverPersonnage(person->uuid());
    if (pj == NULL)
    {
		qWarning() << ( tr("Person %s %s unknown in Carte::changePerson"),
				qPrintable(person->uuid()), qPrintable(person->name()) );
        return;
    }

    delete pj;
}

void Map::changeCharacter(Character * person)
{
	if(NULL==person)
		return;
    DessinPerso * pj = trouverPersonnage(person->uuid());
    if (pj == NULL)
    {
        qWarning() << tr("Person %s %s unknown in Carte::changePerson").arg(person->uuid()).arg(person->name()) ;
        return;
    }

    pj->renommerPerso(person->name());
    pj->changerCouleurPerso(person->color());
}


void Map::emettreCarte(QString titre)
{
    emettreCarteGeneral(titre);
}


void Map::emettreCarte(QString titre, NetworkLink * link)
{
    emettreCarteGeneral(titre, link, true);
}


void Map::emettreCarteGeneral(QString titre, NetworkLink * link, bool versNetworkLinkUniquement)
{

    // On commence par compresser le fond original (format jpeg) dans un tableau
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    if (!m_originalBackground->save(&bufFondOriginal, "jpeg", 70))
    {
		qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);

    if (!m_backgroundImage->save(&bufFond, "jpeg", 70))
    {
		qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }
    // Enfin on compresse la couche alpha (format jpeg) dans un tableau
    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    if (!m_alphaLayer->save(&bufAlpha, "jpeg", 100))
    {
		qWarning() << (tr("Codec Error (emettreCarte - map.cpp)"));
    }


    NetworkMessageWriter message(NetMsg::MapCategory, NetMsg::ImportMap);
    message.string16(titre);
    message.string8(idCarte);
	message.uint8(m_npcSize);
    message.uint8(getPermissionMode());
    message.uint8(getFogColor().red());
    message.byteArray32(baFondOriginal);
    message.byteArray32(baFond);
    message.byteArray32(baAlpha);

    if (versNetworkLinkUniquement)
    {
     message.sendTo(link);
    }
    else
    {

     message.sendAll();
    }
}


void Map::emettreTousLesPersonnages()
{
    emettreTousLesPersonnagesGeneral();
}


void Map::emettreTousLesPersonnages(NetworkLink * link)
{
    emettreTousLesPersonnagesGeneral(link, true);
}


void Map::emettreTousLesPersonnagesGeneral(NetworkLink * link, bool versNetworkLinkUniquement)
{

    NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterList);
    msg.string8(idCarte);
    msg.uint16(children().size());

     msg.string8(idCarte);


      msg.string8(idCarte);
    QObjectList enfants = children();
    // Taille de la liste
    int tailleListe = enfants.size();
    DessinPerso* character;
    for (int i=0; i<tailleListe; i++)
    {
        character = (DessinPerso *)(enfants[i]);
        character->preparerPourEmission(&msg);
    }

    if (versNetworkLinkUniquement)
    {
        //link->emissionDonnees(donnees, tailleCorps + sizeof(enteteMessage));
        msg.sendTo(link);
    }
    else
        msg.sendAll();

}


void Map::emettreTrace()
{
    NetworkMessageWriter* msg = NULL;

    if(ToolsBar::Pen == m_currentTool )
    {
        msg = new NetworkMessageWriter(NetMsg::DrawCategory,NetMsg::penPainting);
        msg->string8(m_localPlayerId);
        msg->string8(idCarte);
        msg->uint32(listePointsCrayon.size());
        for (int i=0; i<listePointsCrayon.size(); i++)
        {
            msg->uint16(listePointsCrayon[i].x());
            msg->uint16(listePointsCrayon[i].y());
        }
        msg->uint16(zoneGlobaleCrayon.x());
        msg->uint16(zoneGlobaleCrayon.y());
        msg->uint16(zoneGlobaleCrayon.width());
        msg->uint16(zoneGlobaleCrayon.height());

		msg->uint8(m_penSize);
        msg->uint8(G_couleurCourante.type);
        msg->rgb(G_couleurCourante.color);
    }
    else if(m_currentTool == ToolsBar::Text)
    {
        msg = new NetworkMessageWriter(NetMsg::DrawCategory,NetMsg::textPainting);
        msg->string8(idCarte);
		msg->string16(m_currentText);
        msg->uint16(m_mousePoint.x());
        msg->uint16(m_mousePoint.y());

        msg->uint16(zoneNouvelle.x());
        msg->uint16(zoneNouvelle.y());
        msg->uint16(zoneNouvelle.width());
        msg->uint16(zoneNouvelle.height());

        msg->uint8(G_couleurCourante.type);
        msg->rgb(G_couleurCourante.color);

    }
    else if (m_currentTool == ToolsBar::Handler)
    {
        return;
    }
    else
    {
        NetMsg::Action action;
        switch(m_currentTool)
        {
        case ToolsBar::Line:
            action = NetMsg::linePainting;
            break;
        case ToolsBar::EmptyRect:
            action = NetMsg::emptyRectanglePainting;
            break;
        case ToolsBar::FilledRect:
            action = NetMsg::filledRectanglePainting;
            break;
        case ToolsBar::EmptyEllipse:
            action = NetMsg::emptyEllipsePainting;
            break;
        case ToolsBar::FilledEllipse:
            action = NetMsg::filledEllipsePainting;
            break;
        }

        msg = new NetworkMessageWriter(NetMsg::DrawCategory,action);
        msg->string8(idCarte);
        msg->uint16(m_originePoint.x());
        msg->uint16(m_originePoint.y());

        msg->uint16(m_mousePoint.x());
        msg->uint16(m_mousePoint.y());

        msg->uint16(zoneNouvelle.x());
        msg->uint16(zoneNouvelle.y());
        msg->uint16(zoneNouvelle.width());
        msg->uint16(zoneNouvelle.height());

		msg->uint8(m_penSize);

        msg->uint8(G_couleurCourante.type);
        msg->rgb(G_couleurCourante.color);
    }

    if(NULL!=msg)
    {
        msg->sendAll();
    }
}


void Map::emettreTrajetPersonnage()
{
    QString idPerso = pnjSelectionne->idPersonnage();
    quint32 tailleListe = listeDeplacement.size();

    NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::moveCharacter);
    msg.string8(idCarte);
    msg.string8(idPerso);
    msg.uint32(tailleListe);
    for (int i=0; i<tailleListe; i++)
    {
        msg.uint16(listeDeplacement[i].x());
        msg.uint16(listeDeplacement[i].y());
    }
    msg.sendAll();
}


void Map::paintPenLine(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur, bool joueurLocal)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintPenLine - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    // Reglage du pinceau pour avoir des bouts arrondis
    QPen pen;
    pen.setColor(couleurPinceau);
    // Si le trace a ete effectue par le joueur local, cela signifie qu'il est deja present sur le fond,
    // par consequence on le redessine en plus fin pour eviter qu'il ne soit plus opaque
    if (joueurLocal)
        pen.setWidthF(((qreal)diametre)-0.5);
    else
        pen.setWidth(diametre);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    int tailleListe = listePoints->size();
    
    // S'il n'y a qu'un point dans la liste on le dessine
    if (tailleListe == 1)
        painter.drawPoint(listePoints->at(0));

    // Sinon s'il n'y a que 2 points dans la liste et qu'ils sont identiques, on dessine un point, ou une ligne s'ils sont differents
    else if (tailleListe == 2)
    {
        if (listePoints->at(0) == listePoints->at(1))
            painter.drawPoint(listePoints->at(0));
        else
            painter.drawLine(listePoints->at(0), listePoints->at(1));
    }
    
    // Sinon on trace une ligne entre chaque point de la liste
    else
        for (int i=1; i<tailleListe; i++)
            painter.drawLine(listePoints->at(i-1), listePoints->at(i));

    // Si le trace effacait, on recopie de fond original sur le fond
    if (couleur.type == Erase)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
		addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
	addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}


void Map::paintText(QString texte, QPoint positionSouris, QRect zoneARafraichir, SelectedColor couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintText - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    // Reglage du pinceau
    QPen pen;
    pen.setColor(couleurPinceau);
    painter.setPen(pen);

    // Parametrage de la fonte
    QFont font;
    font.setPixelSize(16);
    painter.setFont(font);
    
    // On dessine le texte passe en parametre
    painter.drawText(positionSouris, texte);
    
    // Si la couleur effacait, on recopie de fond original sur le fond
    if (couleur.type == Erase)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
		addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
	addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}


void Map::paintOther(NetMsg::Action action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, SelectedColor couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == ColorType)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == Veil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == Unveil)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == Erase)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
		qWarning() << tr("Color type is not correct") <<  "(paintOther - map.cpp)";
        return;
    }
    painter.setRenderHint(QPainter::Antialiasing);

    // Reglage du pinceau
    QPen pen;
    pen.setWidth(diametre);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);

    // Dessin du trace en fonction de l'outil employe

    if (action == NetMsg::linePainting)
    {
        // On dessine une ligne entre le point de depart et d'arrivee
        painter.drawLine(depart, arrivee);
    }
    
    else if (action == NetMsg::emptyRectanglePainting)
    {
        // Creation des points du rectangle a partir des points de depart et d'arrivee
        QPoint supGauche, infDroit;
        supGauche.setX(depart.x()<arrivee.x()?depart.x():arrivee.x());
        supGauche.setY(depart.y()<arrivee.y()?depart.y():arrivee.y());
        infDroit.setX(depart.x()>arrivee.x()?depart.x():arrivee.x());
        infDroit.setY(depart.y()>arrivee.y()?depart.y():arrivee.y());

        // Si le rectangle est petit on dessine un rectangle plein (correction d'un bug Qt)
        if ((infDroit.x()-supGauche.x() < diametre) && (infDroit.y()-supGauche.y() < diametre))
        {
            QPoint delta(diametre/2, diametre/2);
            painter.fillRect(QRect(supGauche - delta, infDroit + delta), couleurPinceau);
        }
        // Sinon on dessine un rectangle vide
        else
            painter.drawRect(QRect(supGauche, infDroit));
    }
    
    else if (action == NetMsg::filledRectanglePainting)
    {
        // On dessine un rectangle plein
        painter.fillRect(QRect(depart, arrivee), couleurPinceau);
    }
    
    else if (action == NetMsg::emptyEllipsePainting)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point de depart
        QPoint diff = arrivee - depart;
        QPoint nouveauPointOrigine = depart - diff;

        // Si l'ellipse est petite on dessine une ellipse pleine (correction d'un bug Qt)
        if (abs(arrivee.x()-nouveauPointOrigine.x()) < diametre && abs(arrivee.y()-nouveauPointOrigine.y()) < diametre)
        {
            // Redefinition des points du rectangle
            QPoint supGauche, infDroit;
            supGauche.setX(nouveauPointOrigine.x()<arrivee.x()?nouveauPointOrigine.x():arrivee.x());
            supGauche.setY(nouveauPointOrigine.y()<arrivee.y()?nouveauPointOrigine.y():arrivee.y());
            infDroit.setX(nouveauPointOrigine.x()>arrivee.x()?nouveauPointOrigine.x():arrivee.x());
            infDroit.setY(nouveauPointOrigine.y()>arrivee.y()?nouveauPointOrigine.y():arrivee.y());
            
            // Parametrage pour une ellipse pleine
            QPen pen;
            pen.setColor(couleurPinceau);
            pen.setWidth(0);
            painter.setPen(pen);
            painter.setBrush(couleurPinceau);
            QPoint delta(diametre/2, diametre/2);
            painter.drawEllipse(QRect(supGauche - delta, infDroit + delta));
        }
        // Sinon on dessine une ellipse vide
        else
            painter.drawEllipse(QRect(nouveauPointOrigine, arrivee));
    }
    
    else if (action == NetMsg::filledEllipsePainting)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point de depart
        QPoint diff = arrivee - depart;
        QPoint nouveauPointOrigine = depart - diff;

        // On dessine une ellipse pleine    
        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(couleurPinceau);
        painter.drawEllipse(QRect(nouveauPointOrigine, arrivee));
    }
            
    else
		qWarning() << (tr("Undefined Tool (paintOther - map.cpp)"));

    // Si le trace effacait, on recopie de fond original sur le fond
    if (couleur.type == Erase)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
		addAlphaLayer(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
	addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}

QColor Map::getFogColor()
{
	if(!m_localIsPlayer)
    {
       return PreferencesManager::getInstance()->value("Fog_color",QColor(Qt::black)).value<QColor>();
    }
    else
    {
        return QColor(0,0,0);
    }
}

void Map::adapterCoucheAlpha(quint8 intensiteAlpha)
{
    if (intensiteAlpha == getFogColor().red())
        return;
        
    // Dans le cas contraire il faut modifier tous les pixels de la couche alpha
    
    // Calcul de la difference entre les 2 intensites
    qint16 diff = (qint16)(getFogColor().red()) - (qint16)intensiteAlpha;
    // Nbr de pixels de la couche alpha
    int tailleAlpha = m_alphaLayer->width() * m_alphaLayer->height();
    // Pointeur vers les donnees de l'image
    QRgb *pixelAlpha = (QRgb *)m_alphaLayer->bits();

    qint16 nouvelleIntensite;
    // Parcours des pixels de l'image
    for (int i=0; i<tailleAlpha; i++)
    {
        // Si le pixel n'est pas opaque, on le modifie
        if (qRed(pixelAlpha[i]) != 255)
        {
            // Calcul de la nouvelle intensite pour le pixel concerne
            nouvelleIntensite = qRed(pixelAlpha[i]) + diff;

            // Correction de l'intensite en cas de depassement (probablement du a la compression jpeg)
            if (nouvelleIntensite < 0)
                nouvelleIntensite = 0;
            else if (nouvelleIntensite > 255)
                nouvelleIntensite = 255;

            // Ecriture du nouveau pixel
            pixelAlpha[i] = 0xFF000000 | nouvelleIntensite << 16 | nouvelleIntensite << 8 | nouvelleIntensite;
        }
    }
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
	addAlphaLayer(m_backgroundImage, m_alphaLayer, fondAlpha);
}

void Map::lancerDeplacementPersonnage(QString idPerso, QList<QPoint> listePoints)
{
    // On commence part verifier si le perso est deja present dans la liste des mouvements
    int i;
    int tailleListe = mouvements.size();
    bool trouve = false;
    // Parcours de la liste des mouvements
    for (i=0; i<tailleListe && !trouve; i++)
        if (mouvements[i].idPersonnage == idPerso)
            trouve = true;

    // Si le perso est deja present dans la liste, on ajoute les nouveaux deplacements a ceux existant
    if (trouve)
        mouvements[i-1].trajet += listePoints;

    // Sinon on cree un nouvel element dans les liste des mouvements
    else
    {
        // Creation du mouvement
        PersoEnMouvement trajetPerso;
        trajetPerso.idPersonnage = idPerso;
        trajetPerso.trajet = listePoints;
        // Ajout du mouvement a la liste des personnages en mouvement
        mouvements.append(trajetPerso);
        // Si l'element ajoute est le seul de la liste, on relance le timer
        if (mouvements.size() == 1)
        {
            QTimer::singleShot(10, this, SLOT(moveAllCharacters()));
        }
    }
}


void Map::moveAllCharacters()
{
    QPoint position;
    int i=0;
    while(i < mouvements.size())
    {

        DessinPerso *perso = trouverPersonnage(mouvements[i].idPersonnage);


        if (NULL != perso)
        {
            position = mouvements[i].trajet.takeFirst();
            perso->deplaceCentrePerso(position);
            afficheOuMasquePnj(perso);
        }
        if (mouvements[i].trajet.isEmpty() || !perso)
        {
            mouvements.removeAt(i);
        }
        else
            i++;
    }
    if (!mouvements.isEmpty())
    {
        QTimer::singleShot(10, this, SLOT(moveAllCharacters()));
    }
}


void Map::saveMap(QDataStream &out, QString titre)
{
    bool ok;
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    ok = m_originalBackground->save(&bufFondOriginal, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on original background(saveMap - map.cpp)");
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);
    ok = m_backgroundImage->save(&bufFond, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on background(saveMap - map.cpp)");
    }


    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    ok = m_alphaLayer->save(&bufAlpha, "jpeg", 100);
    if (!ok)
    {
		qWarning() << tr("Compression issue on alpha layer (saveMap - map.cpp)");
    }

    out << titre;
    out << pos();
    out << (quint32)m_currentMode;
    out << m_alphaLayer->size();
	out << m_npcSize;
    out << baFondOriginal;
    out << baFond;
    out << baAlpha;




    DessinPerso *perso;
    quint16 nombrePnj = 0;
    QObjectList enfants = children();
    int tailleListe = enfants.size();
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        if (perso->estVisible())
            nombrePnj++;
    }

    // Write NPC
    out << nombrePnj;
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        if (perso->estVisible())
        {
            perso->write(out);
        }
    }
}

QString Map::identifiantCarte()
{
    return idCarte;
}

QString Map::getLastSelectedCharacterId()
{
    if(dernierPnjSelectionne==NULL)
        return QString();
    return dernierPnjSelectionne->idPersonnage();
}
bool Map::selectCharacter(QString& id)
{
    DessinPerso* tmp=trouverPersonnage(id);
    if(tmp!=NULL)
        dernierPnjSelectionne=tmp;
    else
        return false;

    return true;
}
void Map::setPermissionMode(Map::PermissionMode mode)
{
    m_currentMode = mode;
}
Map::PermissionMode Map::getPermissionMode()
{
    return m_currentMode;
}
void Map::setHasPermissionMode(bool b)
{
    m_hasPermissionMode =b;
}
bool Map::hasPermissionMode()
{
    return m_hasPermissionMode;
}

void Map::setPointeur(ToolsBar::SelectableTool currentTool)
{
    m_currentTool = currentTool;
    switch(currentTool)
    {
        case ToolsBar::Pen:
        case ToolsBar::Line:
        case ToolsBar::EmptyRect:
        case ToolsBar::FilledRect:
        case ToolsBar::EmptyEllipse:
        case ToolsBar::FilledEllipse:
            pointeur = *m_pencilCursor;
        break;
        case ToolsBar::Text:
            pointeur = *m_textCursor;
        break;
        case ToolsBar::Handler:
            pointeur =  Qt::OpenHandCursor;
        break;
        case ToolsBar::AddNpc:
            pointeur =*m_addCursor;
        break;
        case ToolsBar::DelNpc:
            pointeur =*m_delCursor;
        break;
        case ToolsBar::MoveCharacterToken:
            pointeur =*m_movCursor;
        break;
        case ToolsBar::ChangeCharacterState:
            pointeur = *m_stateCursor;
        break;
    }
    setCursor(pointeur);
}
void Map::setCurrentText(QString text)
{
	m_currentText= text;
}
void Map::setCurrentNpcName(QString text)
{
	m_currentNpcName= text;
}
void Map::setCurrentNpcNumber(int number)
{
	m_currentNpcNumber= number;
}
void Map::setPenSize(int number)
{
	m_penSize=number;
}

void Map::setCharacterSize(int number)
{
	m_npcSize=number;

}
void Map::setNpcNameVisible(bool b)
{
    m_showNpcName = b;
}

void Map::setPcNameVisible(bool b)
{
    m_showPcName =b;
}
void Map::setNpcNumberVisible(bool b)
{
    m_showNpcNumber = b;
}
void Map::setLocalIsPlayer(bool b)
{
	m_localIsPlayer = b;
}
