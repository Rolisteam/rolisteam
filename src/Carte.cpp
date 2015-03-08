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


#include "Carte.h"

#include <QMessageBox>
#include <QUuid>
#include <QBuffer>

#include "network/networkmessagewriter.h"
#include "DessinPerso.h"
#include "Liaison.h"
#include "persons.h"
#include "playersList.h"

#include "variablesGlobales.h"
#include <QDebug>
#include "preferencesmanager.h"
//GM_ONLY, PC_MOVE,PC_ALL


Carte::Carte(QString identCarte, QImage *image, bool masquer, QWidget *parent)
    : QWidget(parent), idCarte(identCarte),m_hasPermissionMode(true)
{

    m_currentMode = Carte::GM_ONLY;
    m_currentTool = BarreOutils::main;

    m_originalBackground = new QImage(image->size(), QImage::Format_ARGB32);
    *m_originalBackground = image->convertToFormat(QImage::Format_ARGB32);
    

    m_backgroundImage = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
    *m_backgroundImage = image->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    m_alphaLayer = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);

    QPainter painterAlpha(m_alphaLayer);
    painterAlpha.fillRect(0, 0, image->width(), image->height(),masquer?getFogColor():Qt::white);

    p_init();
}


void Carte::p_init()
{
    initCursor();
    effaceAlpha = new QImage(m_originalBackground->size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painterEfface(effaceAlpha);
    painterEfface.fillRect(0, 0, m_originalBackground->width(), m_originalBackground->height(), Qt::black);
    // Ajout de la couche alpha effaceAlpha a l'image de fond originale
    ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground);

    m_localPlayer = PlayersList::instance()->localPlayer();

    fondAlpha = new QImage(m_originalBackground->size(), QImage::Format_ARGB32);



    ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha);

    QColor color(fondAlpha->pixel(10,10));
    QColor color2(m_alphaLayer->pixel(10,10));


    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, PreferencesManager::getInstance()->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());
    setPalette(pal);
    
    // variable Initialisation
    taillePj = 12;
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


Carte::Carte(QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent)
    : QWidget(parent), idCarte(identCarte),m_hasPermissionMode(true)
{

    m_currentMode = Carte::GM_ONLY;
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

void Carte::initCursor()
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

void Carte::paintEvent(QPaintEvent *event)
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


    if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj ||
        m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
        return;
        

    dessiner(painter);


   // painter.drawImage(rect(), *m_alphaLayer, m_alphaLayer->rect());

    /*painter.setBrush(Qt::black);
    painter.fillRect(m_refreshZone,Qt::black);*/
}
QPoint Carte::mapToScale(const QPoint & p)
{
    QPoint tmp = p;
    m_scaleX=(qreal)fondAlpha->rect().width()/rect().width();
    m_scaleY=(qreal)fondAlpha->rect().height()/rect().height();
    tmp.setX(tmp.x()*m_scaleX);
    tmp.setY(tmp.y()*m_scaleY);




    return tmp;
}
QPoint Carte::mapToNormal(const QPoint & p)
{
    QPoint tmp;
   /* m_origineScalePoint = m_scalePoint;
    m_scalePoint = p;*/
    tmp.setX(p.x()/m_scaleX);
    tmp.setY(p.y()/m_scaleY);
    return tmp;
}
void Carte::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToScale(event->pos());


    if ((event->button() == Qt::LeftButton) && !boutonGaucheEnfonce && !boutonDroitEnfonce)
    {

            boutonGaucheEnfonce = true;



            if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj ||
                m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
            {
                if((!G_joueur)||(Carte::PC_ALL==m_currentMode)||(Carte::PC_MOVE == m_currentMode))
                {
                    actionPnjBoutonEnfonce(pos);
                }
            }

            // Il s'agit de l'outil main
            else if (m_currentTool == BarreOutils::main)
            {
                // On initialise le deplacement de la Carte
                //if((!G_joueur)||
               //   (((m_currentMode == Carte::PC_ALL))))
                {
                    emit commencerDeplacementCarteFenetre(mapToGlobal(pos));
                }

        }
        // Il s'agit d'une action de dessin
        else
        {
                if(((!G_joueur))||
                  (((m_currentMode == Carte::PC_ALL))))
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
        
        if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj)
        {
            boutonDroitEnfonce = true;

            setCursor(Qt::WhatsThisCursor);
            DessinPerso *pnj = dansDessinPerso(positionSouris);
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
        else if (m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
        {
            boutonDroitEnfonce = true;
            setCursor(*m_orientCursor);
            // On regarde s'il y a un PNJ sous la souris
            DessinPerso *pnj = dansDessinPerso(positionSouris);        
            // Si oui, on modifie son affichage de l'orientation et on le selectionne
            if (pnj)
            {
                pnj->afficheOuMasqueOrientation();
                dernierPnjSelectionne = pnj;
                
                // Emission de la demande d'affichage/masquage de l'orientation
                
                // Taille des donnees
                quint32 tailleCorps =
                    // Taille de l'identifiant de la carte
                    sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                    // Taille de l'identifiant du perso
                    sizeof(quint8) + pnj->idPersonnage().size()*sizeof(QChar) +
                    // Taille de l'info affichage/masquage de l'orientation
                    sizeof(quint8);
                                            
                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];
        
                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = personnage;
                uneEntete->action = afficherMasquerOrientationPerso;
                uneEntete->tailleDonnees = tailleCorps;
                
                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout de l'identifiant de la carte
                quint8 tailleIdCarte = idCarte.size();
                memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                p+=tailleIdCarte*sizeof(QChar);
                // Ajout de l'identifiant du perso
                quint8 tailleIdPnj = pnj->idPersonnage().size();
                memcpy(&(donnees[p]), &tailleIdPnj, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), pnj->idPersonnage().data(), tailleIdPnj*sizeof(QChar));
                p+=tailleIdPnj*sizeof(QChar);
                // Ajout du numero d'etat
                quint8 afficheOrientation = pnj->orientationEstAffichee();
                memcpy(&(donnees[p]), &afficheOrientation, sizeof(quint8));
                p+=sizeof(quint8);
                // Emission du changement d'etat de perso au serveur ou aux clients
                emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;
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


void Carte::mouseReleaseEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (event->button() == Qt::LeftButton && boutonGaucheEnfonce)
    {
        // Bouton gauche relache
        boutonGaucheEnfonce = false;

        // Il s'agit d'une action sur les PJ/PNJ
        if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj ||
            m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
        {
            actionPnjBoutonRelache(pos);
        }

        // Il s'agit de l'outil main
        else if (m_currentTool == BarreOutils::main)
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
                if (G_couleurCourante.type == qcolor)
                    painter.begin(m_backgroundImage);
                else if (G_couleurCourante.type == masque || G_couleurCourante.type == demasque)
                    painter.begin(m_alphaLayer);
                else if (G_couleurCourante.type == efface)
                    painter.begin(effaceAlpha);
                else
                {
                    painter.begin(m_backgroundImage);
                    //qWarning(tr("Type de couleur incorrecte (mouseReleaseEvent - Carte.cpp)"));
                }
                painter.setRenderHint(QPainter::Antialiasing);
                


                dessiner(painter);
            }

            if(((!G_joueur))||
              (((m_currentMode == Carte::PC_ALL))))
            {
                 m_mousePoint = pos;
                zoneNouvelle = zoneARafraichir();
            }

            // Idem : seul le serveur peut dessiner directement sur le plan
            if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
            {
                if (G_couleurCourante.type == efface)
                {
                    // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
                    ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground, zoneNouvelle);
                    // Apres quoi on recopie la zone concernee sur l'image de fond
                    QPainter painterFond(m_backgroundImage);
                    painterFond.drawImage(zoneNouvelle, *m_originalBackground, zoneNouvelle);
                    // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
                    QPainter painterEfface(effaceAlpha);
                    painterEfface.fillRect(zoneNouvelle, Qt::black);
                }

                // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
                ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha, zoneNouvelle);
                // Demande de rafraichissement de la fenetre (appel a paintEvent)
                update(/*zoneOrigine.unite(zoneNouvelle)*/);
                // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
                afficheOuMasquePnj();
            }
            if((!G_joueur)||
              (((m_currentMode == Carte::PC_ALL))))
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
        if ((m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso) && dernierPnjSelectionne)
        {
            // Emission de la nouvelle orientation
            
            // Taille des donnees
            quint32 tailleCorps =
                // Taille de l'identifiant de la carte
                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                // Taille de l'identifiant du perso
                sizeof(quint8) + dernierPnjSelectionne->idPersonnage().size()*sizeof(QChar) +
                // Taille de l'orientation
                sizeof(qint16) + sizeof(qint16);
            
            // Buffer d'emission
            char *donnees = new char[tailleCorps + sizeof(enteteMessage)];
    
            // Creation de l'entete du message
            enteteMessage *uneEntete;
            uneEntete = (enteteMessage *) donnees;
            uneEntete->categorie = personnage;
            uneEntete->action = changerOrientationPerso;
            uneEntete->tailleDonnees = tailleCorps;
            
            // Creation du corps du message
            int p = sizeof(enteteMessage);
            // Ajout de l'identifiant de la carte
            quint8 tailleIdCarte = idCarte.size();
            memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
            p+=sizeof(quint8);
            memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
            p+=tailleIdCarte*sizeof(QChar);
            // Ajout de l'identifiant du perso
            quint8 tailleIdPnj = dernierPnjSelectionne->idPersonnage().size();
            memcpy(&(donnees[p]), &tailleIdPnj, sizeof(quint8));
            p+=sizeof(quint8);
            memcpy(&(donnees[p]), dernierPnjSelectionne->idPersonnage().data(), tailleIdPnj*sizeof(QChar));
            p+=tailleIdPnj*sizeof(QChar);
            // Ajout de l'orientation
            QPoint orientation =  dernierPnjSelectionne->orientationPersonnage();
            qint16 orientationX = orientation.x();
            qint16 orientationY = orientation.y();
            memcpy(&(donnees[p]), &orientationX, sizeof(qint16));
            p+=sizeof(qint16);
            memcpy(&(donnees[p]), &orientationY, sizeof(qint16));
            p+=sizeof(qint16);
            // Emission du changement d'orientation au serveur ou aux clients
            emettre(donnees, tailleCorps + sizeof(enteteMessage));
            // Liberation du buffer d'emission
            delete[] donnees;
            
        }        // Fin de l'outil de deplacement ou de changement d'etat de perso
    }            // Fin du bouton droit relache
}


void Carte::mouseMoveEvent(QMouseEvent *event)
{
      QPoint pos = mapToScale(event->pos());

    if (boutonGaucheEnfonce && !boutonDroitEnfonce)
    {
        // Il s'agit d'une action sur les PJ/PNJ
        if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj ||
            m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
        {
            actionPnjMouvementSouris(pos);
        }

        // Il s'agit de l'outil main
        else if (m_currentTool == BarreOutils::main)
        {
            // On deplace la Carte dans la CarteFenetre
            emit deplacerCarteFenetre(mapToGlobal(pos));
        }

        // Il s'agit d'une action de dessin
        else
        {
            if((!G_joueur)||
              (((m_currentMode == Carte::PC_ALL))))
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
        if (m_currentTool == BarreOutils::ajoutPnj || m_currentTool == BarreOutils::supprPnj)
        {
        }
        // Il s'agit d'une action de deplacement ou de changement d'etat de PNJ
        else if (m_currentTool == BarreOutils::deplacePerso || m_currentTool == BarreOutils::etatPerso)
        {
            // On met a jour l'orientation du PNJ (si la souris n'est pas sur un PNJ
            // et qu'il en existe un de selectionne)
            if (dernierPnjSelectionne && !dansDessinPerso(positionSouris))
                dernierPnjSelectionne->dessinerPersonnage(positionSouris);
        }
        else
        {
            QColor couleur = QColor(m_backgroundImage->pixel(positionSouris.x(), positionSouris.y()));
            emit changeCurrentColor(couleur);
        }
    }
}


void Carte::dessiner(QPainter &painter)
{
    QColor couleurPinceau;
    

    if (G_couleurCourante.type == qcolor)
        couleurPinceau = G_couleurCourante.color;
    else if (G_couleurCourante.type == masque)
    {
        couleurPinceau = getFogColor();
    }
    else if(G_couleurCourante.type == demasque)
        couleurPinceau = Qt::white;
    else if(G_couleurCourante.type == efface)
        couleurPinceau = Qt::white;
    else
        qWarning() << tr("color type not allowed (dessiner - Carte.cpp)");


    QPen pen;
    pen.setWidth(G_diametreTraitCourant);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);


    if (m_currentTool == BarreOutils::crayon)
    {

        QPainter painterCrayon;
        
        if (G_couleurCourante.type == qcolor)
            painterCrayon.begin(m_backgroundImage);
        else if (G_couleurCourante.type == masque || G_couleurCourante.type == demasque)
            painterCrayon.begin(m_alphaLayer);
        else if (G_couleurCourante.type == efface)
            painterCrayon.begin(effaceAlpha);
        else
        {
            painter.begin(m_backgroundImage);
            qWarning() << tr("color type not allowed  (dessiner - Carte.cpp)");
        }
        painterCrayon.setRenderHint(QPainter::Antialiasing);
        
        // M.a.j du pinceau pour avoir des bouts arrondis
        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(G_diametreTraitCourant);
        pen.setCapStyle(Qt::RoundCap);
        painterCrayon.setPen(pen);
        painter.setPen(pen);
        // On dessine une ligne entre le point d'origine et le pointeur de la souris, sur le fond et sur le widget
        painterCrayon.drawLine(m_originePoint, m_mousePoint);
        painter.drawLine(m_originePoint, m_mousePoint);

        // Dessin d'un point pour permettre a l'utilisateur de ne dessiner qu'un unique point (cas ou il ne deplace pas la souris)
        painter.drawPoint(m_mousePoint);
        
        if (G_couleurCourante.type == efface)
        {
            // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
            ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground, zoneNouvelle);
            // Apres quoi on recopie la zone concernee sur l'image de fond
            QPainter painterFond(m_backgroundImage);
            painterFond.drawImage(zoneNouvelle, *m_originalBackground, zoneNouvelle);
            // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
            painterCrayon.fillRect(zoneNouvelle, Qt::black);
        }
        
        // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
        ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha, zoneNouvelle);

        // On ajoute la position de la souris a la liste de points destinee a l'emission
        listePointsCrayon.append(m_mousePoint);
        // On agglomere les zones a rafraichir pour obtenir la zone globale du trace du crayon (pour l'emission)
        zoneGlobaleCrayon = zoneGlobaleCrayon.united(zoneNouvelle);

        // M.a.j du point d'origine
        m_originePoint = m_mousePoint;
    }
    
    else if (m_currentTool == BarreOutils::ligne)
    {
        // On dessine une ligne entre le point d'origine et le pointeur de la souris
        painter.drawLine(m_originePoint, m_mousePoint);
    }
    
    else if (m_currentTool == BarreOutils::rectVide)
    {
        // Creation des points du rectangle a partir du point d'origine et du pointeur de souris
        QPoint supGauche, infDroit;
        supGauche.setX(m_originePoint.x()<m_mousePoint.x()?m_originePoint.x():m_mousePoint.x());
        supGauche.setY(m_originePoint.y()<m_mousePoint.y()?m_originePoint.y():m_mousePoint.y());
        infDroit.setX(m_originePoint.x()>m_mousePoint.x()?m_originePoint.x():m_mousePoint.x());
        infDroit.setY(m_originePoint.y()>m_mousePoint.y()?m_originePoint.y():m_mousePoint.y());

        // Si le rectangle est petit on dessine un rectangle plein (correction d'un bug Qt)
        if ((infDroit.x()-supGauche.x() < G_diametreTraitCourant) && (infDroit.y()-supGauche.y() < G_diametreTraitCourant))
        {
            QPoint delta(G_diametreTraitCourant/2, G_diametreTraitCourant/2);
            painter.fillRect(QRect(supGauche - delta, infDroit + delta), couleurPinceau);
        }
        // Sinon on dessine un rectangle vide
        else
            painter.drawRect(QRect(supGauche, infDroit));
    }
    
    else if (m_currentTool == BarreOutils::rectPlein)
    {
        // On dessine un rectangle plein
        painter.fillRect(QRect(m_originePoint, m_mousePoint), couleurPinceau);
    }
    else if (m_currentTool == BarreOutils::elliVide)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point d'origine
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;

        // Si l'ellipse est petite on dessine une ellipse pleine (correction d'un bug Qt)
        if (abs(m_mousePoint.x()-nouveauPointOrigine.x()) < G_diametreTraitCourant && abs(m_mousePoint.y()-nouveauPointOrigine.y()) < G_diametreTraitCourant)
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
            QPoint delta(G_diametreTraitCourant/2, G_diametreTraitCourant/2);
            painter.drawEllipse(QRect(supGauche - delta, infDroit + delta));
        }
        // Sinon on dessine une ellipse vide
        else
            painter.drawEllipse(QRect(nouveauPointOrigine, m_mousePoint));
    }
    
    else if (m_currentTool == BarreOutils::elliPlein)
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
    
    else if (m_currentTool == BarreOutils::texte)
    {
        // Parametrage de la fonte
        QFont font;
        font.setPixelSize(16);
        painter.setFont(font);
        // On affiche le texte de la zone de saisie
        painter.drawText(m_mousePoint, G_texteCourant);  // + QPoint(2,7)
    }
    
    else if (m_currentTool == BarreOutils::main)
    {
    }
            
    else
        qWarning() << tr("undefined drawing tools (dessiner - Carte.cpp)");
}

QRect Carte::zoneARafraichir()
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
    if (m_currentTool == BarreOutils::crayon)
    {
        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (m_currentTool == BarreOutils::ligne)
    {
        resultat = QRect(gauche-G_diametreTraitCourant, haut-G_diametreTraitCourant, largeur+G_diametreTraitCourant*2, hauteur+G_diametreTraitCourant*2);
    }
    
    else if (m_currentTool == BarreOutils::rectVide)
    {
        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (m_currentTool == BarreOutils::rectPlein)
    {
        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (m_currentTool == BarreOutils::elliVide)
    {
        QPoint diff = m_mousePoint - m_originePoint;
        QPoint nouveauPointOrigine = m_originePoint - diff;
        
        int gauche = nouveauPointOrigine.x()<m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int haut = nouveauPointOrigine.y()<m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int droit = nouveauPointOrigine.x()>m_mousePoint.x()?nouveauPointOrigine.x():m_mousePoint.x();
        int bas = nouveauPointOrigine.y()>m_mousePoint.y()?nouveauPointOrigine.y():m_mousePoint.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (m_currentTool == BarreOutils::elliPlein)
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
    
    else if (m_currentTool == BarreOutils::texte)
    {
        resultat = QRect(QPoint(m_mousePoint.x()-2, m_mousePoint.y()-15), QPoint(m_backgroundImage->width(), m_mousePoint.y()+4));
    }
    
    else if (m_currentTool == BarreOutils::main)
    {

    } 
    else
        qWarning() <<  (tr("Undefined tool  (drawing - Carte.cpp)"));

    /*m_originePoint=m_origineScalePoint;
    m_mousePoint=m_scalePoint;*/
    /*m_originePoint=mapToScale(m_originePoint);
    m_mousePoint=mapToScale(m_mousePoint);*/

   // qDebug() << "Refresh zone1: " << m_backgroundImage->rect() << resultat << m_mousePoint << m_originePoint ;
    m_refreshZone = resultat.intersected(m_backgroundImage->rect());
   //qDebug() << "Refresh zone2: " <<resultat.intersect(m_backgroundImage->rect()) << width() << height();
    return m_refreshZone;//rect();
}


bool Carte::ajouterAlpha(QImage *source, QImage *alpha, QImage *destination, const QRect &rect)
{
    if (source->size() != destination->size() || source->size() != alpha->size())
    {
        qWarning() << (tr("Source, destination and alpha layer have not the same size  (ajouterAlpha - Carte.cpp)"));
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


void Carte::actionPnjBoutonEnfonce(QPoint positionSouris)
{
    if (m_currentTool == BarreOutils::ajoutPnj)
    {
        if((!G_joueur)||(Carte::PC_ALL==m_currentMode))
        {
            if (G_couleurCourante.type == qcolor)
            {
                // Creation de l'identifiant du PNJ
                QString idPnj = QUuid::createUuid().toString();
                // Creation du dessin du PNJ qui s'affiche dans le widget
                DessinPerso *pnj = new DessinPerso(this, idPnj, G_nomPnjCourant, G_couleurCourante.color, taillePj, positionSouris, DessinPerso::pnj, G_numeroPnjCourant);
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
        DessinPerso *pnj = dansDessinPerso(positionSouris);
        if (pnj)
        {
            if((!G_joueur)||
               (Carte::PC_ALL==m_currentMode)||
                    ((Carte::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnj->idPersonnage())>-1)) )//if not found -1
            {
                if (m_currentTool == BarreOutils::supprPnj)
                {
                        if (!pnj->estUnPj())
                        {
                            if (dernierPnjSelectionne == pnj)
                                dernierPnjSelectionne = 0;

                            // Emission de la demande de suppression de de PNJ

                            // Taille des donnees
                            quint32 tailleCorps =
                                // Taille de l'identifiant de la carte
                                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                                // Taille de l'identifiant du PNJ
                                sizeof(quint8) + pnj->idPersonnage().size()*sizeof(QChar);

                            // Buffer d'emission
                            char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                            // Creation de l'entete du message
                            enteteMessage *uneEntete;
                            uneEntete = (enteteMessage *) donnees;
                            uneEntete->categorie = persoNonJoueur;
                            uneEntete->action = supprimerPersoNonJoueur;
                            uneEntete->tailleDonnees = tailleCorps;

                            // Creation du corps du message
                            int p = sizeof(enteteMessage);
                            // Ajout de l'identifiant de la carte
                            quint8 tailleIdCarte = idCarte.size();
                            memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                            p+=sizeof(quint8);
                            memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                            p+=tailleIdCarte*sizeof(QChar);
                            // Ajout de l'identifiant du PNJ
                            quint8 tailleIdPnj = pnj->idPersonnage().size();
                            memcpy(&(donnees[p]), &tailleIdPnj, sizeof(quint8));
                            p+=sizeof(quint8);
                            memcpy(&(donnees[p]), pnj->idPersonnage().data(), tailleIdPnj*sizeof(QChar));
                            p+=tailleIdPnj*sizeof(QChar);

                            // Emission de la suppression de PNJ au serveur ou aux clients
                            emettre(donnees, tailleCorps + sizeof(enteteMessage));
                            // Liberation du buffer d'emission
                            delete[] donnees;

                            // Destruction du personnage
                            pnj->~DessinPerso();
                        }

                }

                else if (m_currentTool == BarreOutils::deplacePerso)
                {
                            pnjSelectionne = pnj;
                            // On calcule la difference entre le coin sup gauche du PNJ et le pointeur de la souris
                            diffSourisDessinPerso = pnj->mapFromParent(positionSouris);
                            // Mise a zero de la liste de points
                            listeDeplacement.clear();
                            // Ajout de la position actuelle du perso dans la liste
                            listeDeplacement.append(pnj->positionCentrePerso());


                }
                else if (m_currentTool == BarreOutils::etatPerso)
                {
                        // changement d'etat du personnage
                        pnj->changerEtat();
                        dernierPnjSelectionne = pnj;

                        // Emission de la demande de changement d'etat du perso

                        // Taille des donnees
                        quint32 tailleCorps =
                            // Taille de l'identifiant de la carte
                            sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                            // Taille de l'identifiant du perso
                            sizeof(quint8) + pnj->idPersonnage().size()*sizeof(QChar) +
                            // Taille du numero d'etat
                            sizeof(quint16);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = personnage;
                        uneEntete->action = changerEtatPerso;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant de la carte
                        quint8 tailleIdCarte = idCarte.size();
                        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                        p+=tailleIdCarte*sizeof(QChar);
                        // Ajout de l'identifiant du perso
                        quint8 tailleIdPnj = pnj->idPersonnage().size();
                        memcpy(&(donnees[p]), &tailleIdPnj, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), pnj->idPersonnage().data(), tailleIdPnj*sizeof(QChar));
                        p+=tailleIdPnj*sizeof(QChar);
                        // Ajout du numero d'etat
                        quint16 numEtat = pnj->numeroEtatSante();
                        memcpy(&(donnees[p]), &numEtat, sizeof(quint16));
                        p+=sizeof(quint16);
                        // Emission du changement d'etat de perso au serveur ou aux clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
                else
                    qWarning() << (tr("undefine tool for processing action on NPC or PC (actionPnjBoutonEnfonce - Carte.cpp)"));
            }
    }
}

}


void Carte::actionPnjBoutonRelache(QPoint positionSouris)
{
             Q_UNUSED(positionSouris)
    if (m_currentTool == BarreOutils::ajoutPnj)
    {
        // On verifie que la couleur courante peut etre utilisee pour dessiner un PNJ
        if (G_couleurCourante.type == qcolor)
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
    
    else if (m_currentTool == BarreOutils::supprPnj)
    {
    }
    
    else if (m_currentTool == BarreOutils::deplacePerso)
    {
        if (pnjSelectionne)
        {
            if((!G_joueur)||
               (Carte::PC_ALL==m_currentMode)||
               ((Carte::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnjSelectionne->idPersonnage())>-1)) )
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
    
    else if (m_currentTool == BarreOutils::etatPerso)
    {
    }

    else
        qWarning() << (tr("undefine tool for processing action on NPC or PC (actionPnjBoutonRelache - Carte.cpp)"));

}


void Carte::actionPnjMouvementSouris(QPoint positionSouris)
{
    if (m_currentTool == BarreOutils::ajoutPnj)
    {
        // Si un PNJ est selectionne, on le deplace
        if (pnjSelectionne)
        {
            // On verifie que la souris reste dans les limites de la carte
            if ( QRect(0, 0, m_backgroundImage->width(), m_backgroundImage->height()).contains(positionSouris, true) )
                pnjSelectionne->deplacePerso(positionSouris - diffSourisDessinPerso);
        }
    }
    
    else if (m_currentTool == BarreOutils::supprPnj)
    {
    }
    
    else if (m_currentTool == BarreOutils::deplacePerso)
    {
        if (pnjSelectionne)
        {
            if((!G_joueur)||
               (Carte::PC_ALL==m_currentMode)||
               ((Carte::PC_MOVE == m_currentMode)&&(m_localPlayer->getIndexOf(pnjSelectionne->idPersonnage())>-1)) )
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
    
    else if (m_currentTool != BarreOutils::etatPerso)
    {
        qWarning() << (tr("undefine tool for processing action on NPC or PC (actionPnjMouvementSouris - Carte.cpp)"));
    }



}


DessinPerso* Carte::dansDessinPerso(QPoint positionSouris)
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
                resultat = dansDessinPerso(positionSouris);
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
            resultat = dansDessinPerso(positionSouris);
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
            resultat = dansDessinPerso(positionSouris);
            // Puis on refait apparaitre le DessinPerso
            dessinPnj->show();
        }
        // Cas qui ne devrait jamais arriver
        else
        {
            qWarning() <<  (tr("unknown widget under cursor (dansDessinPerso - Carte.cpp)"));
            resultat = 0;
        }
    }

    // Le pointeur ne se trouve pas sur un widget
    else
        resultat = 0;

    return resultat;
}


void Carte::afficheOuMasquePnj(DessinPerso *pnjSeul)
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


void Carte::changerTaillePjCarte(int nouvelleTaille, bool updatePj)
{
    taillePj = nouvelleTaille;
    if((updatePj)&&(dernierPnjSelectionne!=NULL))
    {
        dernierPnjSelectionne->changerTaillePj(nouvelleTaille);
    }
        //emit changerTaillePj(nouvelleTaille);
}

int Carte::tailleDesPj()
{
    return taillePj;
}


DessinPerso* Carte::trouverPersonnage(QString idPerso)
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

void Carte::toggleCharacterView(Character * character)
{
    QString uuid = character->uuid();
    bool newState = !pjAffiche(uuid);
    if((!G_joueur)||(Carte::PC_ALL==m_currentMode)||(Carte::PC_MOVE == m_currentMode))
    {
        affichageDuPj(uuid, newState);

        NetworkMessageWriter message(NetMsg::CharacterCategory, NetMsg::ToggleViewCharacterAction);
        message.string8(idCarte);
        message.string8(uuid);
        message.uint8(newState ? 1 : 0);
        message.sendAll();
    }

}


void Carte::affichageDuPj(QString idPerso, bool afficher)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (NULL==pj)
    {
        qWarning() << (tr("PC ID not found (affichageDuPj - Carte.cpp)"));
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


bool Carte::pjAffiche(QString idPerso)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (NULL==pj)
    {
        qWarning() << tr("PC ID: %1 not found (pjAffiche - Carte.cpp)") << qPrintable(idPerso);
        return false;
    }

    return pj->estVisible();
}

void Carte::addCharacter(Character * person)
{
    new DessinPerso(this, person->uuid(), person->name(), person->color(), taillePj, QPoint(m_backgroundImage->width()/2, m_backgroundImage->height()/2), DessinPerso::pj);
}


void Carte::eraseCharacter(QString idPerso)
{
    // Recherche du personnage
    DessinPerso *perso = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
     if (NULL==perso)
    {
        qWarning() << (tr("L'identifiant du personnage n'a pas ete trouve (effacerPerso - Carte.cpp)"));
        return;
    }
    // Suppression du personnage
    perso->~DessinPerso();
}

void Carte::delCharacter(Character * person)
{
    DessinPerso * pj = trouverPersonnage(person->uuid());
    if (pj == NULL)
    {
        qWarning() << ( tr("Person %s %s unknown in Carte::changePerson"),
                qPrintable(person->uuid()), qPrintable(person->name()) );
        return;
    }

    delete pj;
}

void Carte::changeCharacter(Character * person)
{
    DessinPerso * pj = trouverPersonnage(person->uuid());
    if (pj == NULL)
    {
        qWarning() << tr("Person %s %s unknown in Carte::changePerson").arg(person->uuid()).arg(person->name()) ;
        return;
    }

    pj->renommerPerso(person->name());
    pj->changerCouleurPerso(person->color());
}


void Carte::emettreCarte(QString titre)
{
    emettreCarteGeneral(titre);
}


void Carte::emettreCarte(QString titre, Liaison * link)
{
    emettreCarteGeneral(titre, link, true);
}


void Carte::emettreCarteGeneral(QString titre, Liaison * link, bool versLiaisonUniquement)
{

    // On commence par compresser le fond original (format jpeg) dans un tableau
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    if (!m_originalBackground->save(&bufFondOriginal, "jpeg", 70))
    {
        qWarning() << (tr("Codec Error (emettreCarte - Carte.cpp)"));
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);

    if (!m_backgroundImage->save(&bufFond, "jpeg", 70))
    {
        qWarning() << (tr("Codec Error (emettreCarte - Carte.cpp)"));
    }
    // Enfin on compresse la couche alpha (format jpeg) dans un tableau
    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    if (!m_alphaLayer->save(&bufAlpha, "jpeg", 100))
    {
        qWarning() << (tr("Codec Error (emettreCarte - Carte.cpp)"));
    }


    NetworkMessageWriter message(NetMsg::MapCategory, NetMsg::ImportMap);
    message.string16(titre);
    message.string8(idCarte);
    message.uint8(taillePj);
    message.uint8(getPermissionMode());
    message.uint8(getFogColor().red());
    message.byteArray32(baFondOriginal);
    message.byteArray32(baFond);
    message.byteArray32(baAlpha);

    if (versLiaisonUniquement)
    {
     message.sendTo(link);
    }
    else
    {

     message.sendAll();
    }
}


void Carte::emettreTousLesPersonnages()
{
    emettreTousLesPersonnagesGeneral();
}


void Carte::emettreTousLesPersonnages(Liaison * link)
{
    emettreTousLesPersonnagesGeneral(link, true);
}


void Carte::emettreTousLesPersonnagesGeneral(Liaison * link, bool versLiaisonUniquement)
{
    // Taille des donnees
    quint32 tailleCorps =
        // Taille de l'identifiant de la carte
        sizeof(quint8) + idCarte.size()*sizeof(QChar) +
        // Taille du nombre de personnages presents dans le message
        sizeof(quint16);

    DessinPerso *perso;
    // On recupere la liste des enfants de la carte (tous des DessinPerso)
    QObjectList enfants = children();
    // Taille de la liste
    int tailleListe = enfants.size();
    
    // On parcourt une premiere fois la liste des DessinPerso pour calculer la taille des donnees a emettre
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        // On ajoute la taille du personnage a la taille totale
        tailleCorps += perso->tailleDonneesAEmettre();
    }

    // Reservation du buffer d'emission
    char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

    // Creation de l'entete du message
    enteteMessage *uneEntete = (enteteMessage *) donnees;
    uneEntete->categorie = personnage;
    uneEntete->action = ajouterListePerso;
    uneEntete->tailleDonnees = tailleCorps;
        
    // Creation du corps du message
    int p = sizeof(enteteMessage);
    // Ajout de l'identifiant de la carte
    quint8 tailleIdCarte = idCarte.size();
    memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
    p+=sizeof(quint8);
    memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
    p+=tailleIdCarte*sizeof(QChar);
    // Ajout du nbr de personnages presents dans le message
    quint16 nbrPersonnages = tailleListe;
    memcpy(&(donnees[p]), &nbrPersonnages, sizeof(quint16));
    p+=sizeof(quint16);
    
    // On parcourt la liste des DessinPerso une 2eme fois pour ajouter les donnees de chaque personnage
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        // Ajout des donnees du personnage au buffer d'emission
        p += perso->preparerPourEmission(&(donnees[p]));
    }        

    if (versLiaisonUniquement)
        // Emission de la carte vers la liaison indiquee
        link->emissionDonnees(donnees, tailleCorps + sizeof(enteteMessage));
    else
        // Emission de la carte vers tous les autres utilisateurs
        emettre(donnees, tailleCorps + sizeof(enteteMessage));

    // Liberation du buffer d'emission
    delete[] donnees;
}


void Carte::emettreTrace()
{
                qint32 tailleCorps;
    char *donnees;
    enteteMessage *uneEntete;

    // Parametres du message en fonction de l'outil en cours d'utilisation
    if (m_currentTool == BarreOutils::crayon)
    {
                        qint32 tailleListe = listePointsCrayon.size();

        // Taille des donnees
        tailleCorps =
            // Taille de l'identifiant du joueur
            sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
            // Taille de l'identifiant de la carte
            sizeof(quint8) + idCarte.size()*sizeof(QChar) +
            // Taille de la liste de points
            sizeof(quint32) + (sizeof(qint16) + sizeof(qint16)) * tailleListe +
            // Taille du rectangle a rafraichir
            sizeof(qint16) + sizeof(qint16) + sizeof(qint16) + sizeof(qint16) +
            // Taille du diametre du trait
            sizeof(quint8) +
            // Taille de la couleur
            sizeof(couleurSelectionee);

        // Buffer d'emission
        donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = dessin;
        uneEntete->action = traceCrayon;
        uneEntete->tailleDonnees = tailleCorps;
        
        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant de la carte
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'identifiant de la carte
        quint8 tailleIdCarte = idCarte.size();
        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
        p+=tailleIdCarte*sizeof(QChar);
        // Ajout du nombre de points du trace
        memcpy(&(donnees[p]), &tailleListe, sizeof(quint32));
        p+=sizeof(quint32);
        // Ajout des points de la liste
        for (int i=0; i<tailleListe; i++)
        {
            qint16 pointX = listePointsCrayon[i].x();
            qint16 pointY = listePointsCrayon[i].y();
            memcpy(&(donnees[p]), &pointX, sizeof(qint16));
            p+=sizeof(qint16);
            memcpy(&(donnees[p]), &pointY, sizeof(qint16));
            p+=sizeof(qint16);
        }
        // Ajout du rectangle a rafraichir
        qint16 zoneX = zoneGlobaleCrayon.x();
        qint16 zoneY = zoneGlobaleCrayon.y();
        qint16 zoneW = zoneGlobaleCrayon.width();
        qint16 zoneH = zoneGlobaleCrayon.height();
        memcpy(&(donnees[p]), &zoneX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneY, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneW, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneH, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du diametre du trait
        quint8 diametre = G_diametreTraitCourant;
        memcpy(&(donnees[p]), &diametre, sizeof(quint8));
        p+=sizeof(quint8);
        // Ajout de la couleur
        memcpy(&(donnees[p]), &G_couleurCourante, sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);
    }
    
    else if (m_currentTool == BarreOutils::texte)
    {
        // Taille des donnees
        tailleCorps =
            // Taille de l'identifiant de la carte
            sizeof(quint8) + idCarte.size()*sizeof(QChar) +
            // Taille du texte
            sizeof(quint16) + G_texteCourant.size()*sizeof(QChar) +
            // Taille de la position de la souris
            sizeof(qint16) + sizeof(qint16) +
            // Taille du rectangle a rafraichir
            sizeof(qint16) + sizeof(qint16) + sizeof(qint16) + sizeof(qint16) +
            // Taille de la couleur
            sizeof(couleurSelectionee);

        // Buffer d'emission
        donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = dessin;
        uneEntete->action = traceTexte;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant
        quint8 tailleId = idCarte.size();
        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        // Ajout du texte
        quint16 tailleTexte = G_texteCourant.size();
        memcpy(&(donnees[p]), &tailleTexte, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), G_texteCourant.data(), tailleTexte*sizeof(QChar));
        p+=tailleTexte*sizeof(QChar);
        // Ajout de la position de la souris
        qint16 positionX = m_mousePoint.x();
        qint16 positionY = m_mousePoint.y();
        memcpy(&(donnees[p]), &positionX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &positionY, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du rectangle a rafraichir
        qint16 zoneX = zoneNouvelle.x();
        qint16 zoneY = zoneNouvelle.y();
        qint16 zoneW = zoneNouvelle.width();
        qint16 zoneH = zoneNouvelle.height();
        memcpy(&(donnees[p]), &zoneX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneY, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneW, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneH, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout de la couleur
        memcpy(&(donnees[p]), &G_couleurCourante, sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);
    }
    
    else if (m_currentTool == BarreOutils::main)
    {
        return;
    }

    // Tous les autres outils
    else 
    {
        // Taille des donnees
        tailleCorps =
            // Taille de l'identifiant de la carte
            sizeof(quint8) + idCarte.size()*sizeof(QChar) +
            // Taille du point de depart
            sizeof(qint16) + sizeof(qint16) +
            // Taille du point d'arrivee
            sizeof(qint16) + sizeof(qint16) +
            // Taille du rectangle a rafraichir
            sizeof(qint16) + sizeof(qint16) + sizeof(qint16) + sizeof(qint16) +
            // Taille du diametre du trait
            sizeof(quint8) +
            // Taille de la couleur
            sizeof(couleurSelectionee);

        // Buffer d'emission
        donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = dessin;
        uneEntete->tailleDonnees = tailleCorps;

        if (m_currentTool == BarreOutils::ligne)
            uneEntete->action = traceLigne;
        else if (m_currentTool == BarreOutils::rectVide)
            uneEntete->action = traceRectangleVide;
        else if (m_currentTool == BarreOutils::rectPlein)
            uneEntete->action = traceRectanglePlein;
        else if (m_currentTool == BarreOutils::elliVide)
            uneEntete->action = traceEllipseVide;
        else if (m_currentTool == BarreOutils::elliPlein)
            uneEntete->action = traceEllipsePleine;
        else
        {
            qWarning() << (tr("Outil non défini lors de l'emission d'un trace (emettreTrace - Carte.cpp)"));
            delete[] donnees;
            return;
        }
        
        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant
        quint8 tailleId = idCarte.size();
        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        // Ajout du point de depart
        qint16 departX = m_originePoint.x();
        qint16 departY = m_originePoint.y();
        memcpy(&(donnees[p]), &departX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &departY, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du point d'arrivee
        qint16 arriveeX = m_mousePoint.x();
        qint16 arriveeY = m_mousePoint.y();
        memcpy(&(donnees[p]), &arriveeX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &arriveeY, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du rectangle a rafraichir
        qint16 zoneX = zoneNouvelle.x();
        qint16 zoneY = zoneNouvelle.y();
        qint16 zoneW = zoneNouvelle.width();
        qint16 zoneH = zoneNouvelle.height();
        memcpy(&(donnees[p]), &zoneX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneY, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneW, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &zoneH, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du diametre du trait
        quint8 diametre = G_diametreTraitCourant;
        memcpy(&(donnees[p]), &diametre, sizeof(quint8));
        p+=sizeof(quint8);
        // Ajout de la couleur
        memcpy(&(donnees[p]), &G_couleurCourante, sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);
    }        
    
    // Emission du trace
    emettre(donnees, tailleCorps + sizeof(enteteMessage));
    // Liberation du buffer d'emission
    delete[] donnees;
}


void Carte::emettreTrajetPersonnage()
{
    // Recuperation de l'identifiant du perso
    QString idPerso = pnjSelectionne->idPersonnage();
    
    // Taille de la liste
                qint32 tailleListe = listeDeplacement.size();

    // Taille des donnees
    quint32 tailleCorps =
        // Taille de l'identifiant de la carte
        sizeof(quint8) + idCarte.size()*sizeof(QChar) +
        // Taille de l'identifiant du personnage
        sizeof(quint8) + idPerso.size()*sizeof(QChar) +
        // Taille de la liste de points
        sizeof(quint32) + (sizeof(qint16) + sizeof(qint16)) * tailleListe;

    // Buffer d'emission
    char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

    // Creation de l'entete du message
    enteteMessage *uneEntete = (enteteMessage *) donnees;
    uneEntete->categorie = personnage;
    uneEntete->action = deplacerPerso;
    uneEntete->tailleDonnees = tailleCorps;

    // Creation du corps du message
    int p = sizeof(enteteMessage);
    // Ajout de l'identifiant de la carte
    quint8 tailleIdCarte = idCarte.size();
    memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
    p+=sizeof(quint8);
    memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
    p+=tailleIdCarte*sizeof(QChar);
    // Ajout de l'identifiant du perso
    quint8 tailleIdPerso = idPerso.size();
    memcpy(&(donnees[p]), &tailleIdPerso, sizeof(quint8));
    p+=sizeof(quint8);
    memcpy(&(donnees[p]), idPerso.data(), tailleIdPerso*sizeof(QChar));
    p+=tailleIdPerso*sizeof(QChar);
    // Ajout du nbr de points de la liste
    memcpy(&(donnees[p]), &tailleListe, sizeof(quint32));
    p+=sizeof(quint32);
    // Ajout de la liste de points
    qint16 positionX, positionY;
    for (int i=0; i<tailleListe; i++)
    {
        positionX = listeDeplacement[i].x();
        positionY = listeDeplacement[i].y();
        memcpy(&(donnees[p]), &positionX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &positionY, sizeof(qint16));
        p+=sizeof(qint16);
    }

    // Emission du deplacement
    emettre(donnees, tailleCorps + sizeof(enteteMessage));
    // Liberation du buffer d'emission
    delete[] donnees;
}


void Carte::dessinerTraceCrayon(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur, bool joueurLocal)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == demasque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning() << (tr("Type de couleur incorrecte (dessinerTraceGeneral - Carte.cpp)"));
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
    if (couleur.type == efface)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
        ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}


void Carte::dessinerTraceTexte(QString texte, QPoint positionSouris, QRect zoneARafraichir, couleurSelectionee couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == demasque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning() << (tr("Type de couleur incorrecte (dessinerTraceTexte - Carte.cpp)"));
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
    if (couleur.type == efface)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
        ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}


void Carte::dessinerTraceGeneral(actionDessin action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(m_backgroundImage);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = getFogColor();
    }
    else if (couleur.type == demasque)
    {
        painter.begin(m_alphaLayer);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning() << (tr("Type de couleur incorrecte (dessinerTraceGeneral - Carte.cpp)"));
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

    if (action == traceLigne)
    {
        // On dessine une ligne entre le point de depart et d'arrivee
        painter.drawLine(depart, arrivee);
    }
    
    else if (action == traceRectangleVide)
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
    
    else if (action == traceRectanglePlein)
    {
        // On dessine un rectangle plein
        painter.fillRect(QRect(depart, arrivee), couleurPinceau);
    }
    
    else if (action == traceEllipseVide)
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
    
    else if (action == traceEllipsePleine)
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
        qWarning() << (tr("Outil non défini lors du dessin (dessinerTraceGeneral - Carte.cpp)"));

    // Si le trace effacait, on recopie de fond original sur le fond
    if (couleur.type == efface)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
        ajouterAlpha(m_originalBackground, effaceAlpha, m_originalBackground, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(m_backgroundImage);
        painterFond.drawImage(zoneARafraichir, *m_originalBackground, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(/*zoneOrigine.unite(zoneARafraichir)*/);
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}

QColor Carte::getFogColor()
{
    if(!G_joueur)
    {
       return PreferencesManager::getInstance()->value("Fog_color",QColor(Qt::black)).value<QColor>();
    }
    else
    {
        return QColor(0,0,0);
    }
}

void Carte::adapterCoucheAlpha(quint8 intensiteAlpha)
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
    ajouterAlpha(m_backgroundImage, m_alphaLayer, fondAlpha);
}

void Carte::lancerDeplacementPersonnage(QString idPerso, QList<QPoint> listePoints)
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


void Carte::moveAllCharacters()
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


void Carte::saveMap(QDataStream &out, QString titre)
{
    bool ok;
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    ok = m_originalBackground->save(&bufFondOriginal, "jpeg", 100);
    if (!ok)
    {
        qWarning() << tr("Probleme de compression du fond original (sauvegarderCarte - Carte.cpp)");
    }


    QByteArray baFond;
    QBuffer bufFond(&baFond);
    ok = m_backgroundImage->save(&bufFond, "jpeg", 100);
    if (!ok)
    {
        qWarning() << tr("Probleme de compression du fond (sauvegarderCarte - Carte.cpp)");
    }


    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    ok = m_alphaLayer->save(&bufAlpha, "jpeg", 100);
    if (!ok)
    {
        qWarning() << tr("Probleme de compression de la couche alpha (sauvegarderCarte - Carte.cpp)");
    }

    out << titre;
    out << pos();
    out << (quint32)m_currentMode;
    out << m_alphaLayer->size();
    out << taillePj;
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

QString Carte::identifiantCarte()
{
    return idCarte;
}

QString Carte::getLastSelectedCharacterId()
{
    if(dernierPnjSelectionne==NULL)
        return QString();
    return dernierPnjSelectionne->idPersonnage();
}
bool Carte::selectCharacter(QString& id)
{
    DessinPerso* tmp=trouverPersonnage(id);
    if(tmp!=NULL)
        dernierPnjSelectionne=tmp;
    else
        return false;

    return true;
}
void Carte::setPermissionMode(Carte::PermissionMode mode)
{
    m_currentMode = mode;
}
Carte::PermissionMode Carte::getPermissionMode()
{
    return m_currentMode;
}
void Carte::setHasPermissionMode(bool b)
{
    m_hasPermissionMode =b;
}
bool Carte::hasPermissionMode()
{
    return m_hasPermissionMode;
}

void Carte::setPointeur(BarreOutils::Tool currentTool)
{
    m_currentTool = currentTool;
    switch(currentTool)
    {
        case BarreOutils::crayon:
        case BarreOutils::ligne:
        case BarreOutils::rectVide:
        case BarreOutils::rectPlein:
        case BarreOutils::elliVide:
        case BarreOutils::elliPlein:
            pointeur = *m_pencilCursor;
        break;
        case BarreOutils::texte:
            pointeur = *m_textCursor;
        break;
        case BarreOutils::main:
            pointeur =  Qt::OpenHandCursor;
        break;
        case BarreOutils::ajoutPnj:
            pointeur =*m_addCursor;
        break;
        case BarreOutils::supprPnj:
            pointeur =*m_delCursor;
        break;
        case BarreOutils::deplacePerso:
            pointeur =*m_movCursor;
        break;
        case BarreOutils::etatPerso:
            pointeur = *m_stateCursor;
        break;
    }
    setCursor(pointeur);
}
