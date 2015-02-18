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

#include "networkmessagewriter.h"
#include "DessinPerso.h"
#include "Liaison.h"
#include "persons.h"
#include "playersList.h"

#include "variablesGlobales.h"


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/    
Carte::Carte(QString identCarte, QImage *image, bool masquer, QWidget *parent)
    : QWidget(parent), idCarte(identCarte)
{
    // Les images sont creees en ARGB32_Premultiplied pour beneficier de l'antialiasing

    // Creation de l'image de fond originale qui servira a effacer
    fondOriginal = new QImage(image->size(), QImage::Format_ARGB32);
    *fondOriginal = image->convertToFormat(QImage::Format_ARGB32);
    
    // Creation de l'image de fond
    fond = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
    *fond = image->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // Creation de la couche alpha qui sera utilisee avec fondAlpha
    alpha = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painterAlpha(alpha);
    painterAlpha.fillRect(0, 0, image->width(), image->height(), masquer?G_couleurMasque:Qt::white);

    p_init();
}


void Carte::p_init()
{
    // Creation de la couche alpha qui sera utilisee pour effacer fond a l'aide de fondOriginal
    effaceAlpha = new QImage(fondOriginal->size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painterEfface(effaceAlpha);
    painterEfface.fillRect(0, 0, fondOriginal->width(), fondOriginal->height(), Qt::black);
    // Ajout de la couche alpha effaceAlpha a l'image de fond originale
    ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal);

    // Creation d'une image en mode ARGB32 qui sert a mixer le fond et la couche alpha
    fondAlpha = new QImage(fondOriginal->size(), QImage::Format_ARGB32);
                // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(fond, alpha, fondAlpha);

    // Le fond du widget est automatiquement rempli avec une couleur a chaque reaffichage
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::darkMagenta);
    setPalette(pal);
    
    // Initialisation des variables
    taillePj = 12;
    boutonGaucheEnfonce = false;
    boutonDroitEnfonce = false;
    pointSouris = QPoint(0,0);
    pointOrigine = QPoint(0,0);
    diffSourisDessinPerso = QPoint(0,0);
    pnjSelectionne = 0;
    dernierPnjSelectionne = 0;

    // Redimentionnement du widget
    resize(fond->size());
    // Initialisation de la liste de points du trace du crayon et de la liste de deplacement du PJ
    listePointsCrayon.clear();
    listeDeplacement.clear();
    // Mise a zero de la zone globale du trace du crayon
    zoneGlobaleCrayon.setRect(0,0,0,0);

    // Initialisation de la liste des mouvement de personnages
    mouvements.clear();

    // Get every characters
    PlayersList & g_playersList = PlayersList::instance();
    int maxPlayersIndex = g_playersList.numPlayers();
    for (int i = 0 ; i < maxPlayersIndex ; i++)
    {
        Player * player = g_playersList.getPlayer(i);
        int maxCharactersIndex = player->getCharactersCount();
        for (int j = 0 ; j < maxCharactersIndex ; j++)
        {
            addCharacter(player->getCharacterByIndex(j));
        }
    }

    // connect to g_playesList to stay tuned
    connect(&g_playersList, SIGNAL(characterAdded(Character *)),
            this, SLOT(addCharacter(Character *)));
    connect(&g_playersList, SIGNAL(characterDeleted(Character *)),
            this, SLOT(delCharacter(Character *)));
    connect(&g_playersList, SIGNAL(characterChanged(Character *)),
            this, SLOT(changeCharacter(Character *)));
}

/********************************************************************/
/* Constructeur : cree un plan a partir des 3 images le composant   */
/* (image d'origine, image avec les annotations, couche alpha)      */
/********************************************************************/    
Carte::Carte(QString identCarte, QImage *original, QImage *avecAnnotations, QImage *coucheAlpha, QWidget *parent)
    : QWidget(parent), idCarte(identCarte)
{
    // Les images sont creees en ARGB32_Premultiplied pour beneficier de l'antialiasing

    // Creation de l'image de fond originale qui servira a effacer
    fondOriginal = new QImage(original->size(), QImage::Format_ARGB32);
    *fondOriginal = original->convertToFormat(QImage::Format_ARGB32);
    
    // Creation de l'image de fond
    fond = new QImage(avecAnnotations->size(), QImage::Format_ARGB32_Premultiplied);
    *fond = avecAnnotations->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // Creation de la couche alpha qui sera utilisee avec fondAlpha
    alpha = new QImage(coucheAlpha->size(), QImage::Format_ARGB32_Premultiplied);
    *alpha = coucheAlpha->convertToFormat(QImage::Format_ARGB32_Premultiplied);

    p_init();
}

/********************************************************************/
/* Redessine le fond                                                */
/********************************************************************/    
void Carte::paintEvent(QPaintEvent *event)
{
    // Creation du painter pour pouvoir dessiner
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // La ruse consiste a passer par une image ARGB32 pour avoir une couche alpha
    // independante des couleurs, puis a lui rajouter la couche alpha
    
    // Recopie de fondAlpha (qui est m.a.j dans mouseReleaseEvent) dans la fenetre
    painter.drawImage(event->rect(), *fondAlpha, event->rect());

    // Si le bouton gauche n'est pas enfonce on sort de la fonction
    if (boutonGaucheEnfonce == false)
        return;

    // Si un outils de manipulation de PJ/PNJ est selectionne, on sort de la fonction
    if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj ||
        G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        return;
        
    // Dessin temporaire sur le widget
    dessiner(painter);
}

/********************************************************************/
/* Un bouton de la souris vient d'etre enfonce                      */
/********************************************************************/    
void Carte::mousePressEvent(QMouseEvent *event)
{
    // Si l'utilisateur a clique avec la bouton gauche
    if ((event->button() == Qt::LeftButton) && !boutonGaucheEnfonce && !boutonDroitEnfonce)
    {
        // Bonton gauche enfonce
        boutonGaucheEnfonce = true;
        
        // Il s'agit d'une action sur les PJ/PNJ
        if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj ||
            G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        {
            actionPnjBoutonEnfonce(event->pos());
        }

        // Il s'agit de l'outil main
        else if (G_outilCourant == BarreOutils::main)
        {
            // On initialise le deplacement de la Carte
            emit commencerDeplacementCarteFenetre(mapToGlobal(event->pos()));
        }

        // Il s'agit d'une action de dessin
        else
        {            
            // M.a.j. du point d'origine et du point de la souris
            pointOrigine = pointSouris = event->pos();
            // Calcul de la zone a rafraichir
            zoneOrigine = zoneNouvelle = zoneARafraichir();
            // On vide la liste de points du trace du crayon
            listePointsCrayon.clear();
            // Initialisation de la zone globale du trace du crayon
            zoneGlobaleCrayon = zoneNouvelle;
            // Demande de rafraichissement de la fenetre (appel a paintEvent)
            update(zoneOrigine);
        }
         }

    // Si l'utilisateur a clique avec le bouton droit
    else if ((event->button() == Qt::RightButton) && !boutonGaucheEnfonce && !boutonDroitEnfonce)
    {
        // Bouton droit enfonce
        boutonDroitEnfonce = true;
        // Recuperation de la position de la souris
        QPoint positionSouris = event->pos();
        
        // Il s'agit d'une action d'ajout ou de suppression de PNJ
        if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj)
        {
            // Changement de curseur
            setCursor(Qt::WhatsThisCursor);
            // On regarde s'il y a un PNJ sous la souris
            DessinPerso *pnj = dansDessinPerso(positionSouris);
            if (pnj)
            {
                int diametre;
                QColor couleur;
                QString nomPnj;
                // On reucpere le nom et le diametre du PNJ
                pnj->diametreCouleurNom(&diametre, &couleur, &nomPnj);
                // On met a jour le diametre et le nom du PNJ dans la barre d'outils...
                emit mettreAJourPnj(diametre, nomPnj);
                // ...ainsi que la couleur actuelle
                emit changeCouleurActuelle(couleur);
            }
        }
        // Il s'agit d'une action de deplacement ou de l'action de changement d'etat du PNJ
        else if (G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        {
            // Changement de curseur
            setCursor(*G_pointeurOrienter);
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
            // Sinon on met a jour l'orientation du PNJ
            else if (dernierPnjSelectionne)
            {
                dernierPnjSelectionne->dessinerPersonnage(positionSouris);
            }
        }
        // Il s'agit d'une action de dessin on met a jour la couleur courante
        else
        {
            // Changement de curseur
            setCursor(*G_pointeurPipette);
            // Recuperation de la couleur sous le pointeur de la souris
            QColor couleur = QColor(fond->pixel(positionSouris.x(), positionSouris.y()));
            // Emission du signal demandant le changement de la couleur courante
            emit changeCouleurActuelle(couleur);
        }
    }
}

/********************************************************************/
/* Relache d'un bouton de la souris                                 */
/********************************************************************/    
void Carte::mouseReleaseEvent(QMouseEvent *event)
{
    // Si le bouton gauche est relache
    if (event->button() == Qt::LeftButton && boutonGaucheEnfonce)
    {
        // Bouton gauche relache
        boutonGaucheEnfonce = false;

        // Il s'agit d'une action sur les PJ/PNJ
        if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj ||
            G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        {
            actionPnjBoutonRelache(event->pos());
        }

        // Il s'agit de l'outil main
        else if (G_outilCourant == BarreOutils::main)
        {
            // On ne fait rien
        }

        // Il s'agit d'une action de dessin
        else
        {
            // On recupere la position de la souris
            QPoint pointSouris = event->pos();

            // Si l'ordinateur local est le serveur on peut dessiner directement sur le plan
            // Dans le cas contraire le plan ne sera mis a jour qu'a la reception du message de trace
            // ayant fait l'aller-retour avec le serveur (necessaire pour conserver la coherence
            // entre les differents utilisateurs : le serveur fait foi)
            if (!G_client)
            {
                // Creation du painter pour pouvoir dessiner
                QPainter painter;
        
                // Choix de l'image sur laquelle dessiner, en fonction de la couleur actuelle
                if (G_couleurCourante.type == qcolor)
                    painter.begin(fond);
                else if (G_couleurCourante.type == masque || G_couleurCourante.type == demasque)
                    painter.begin(alpha);
                else if (G_couleurCourante.type == efface)
                    painter.begin(effaceAlpha);
                else
                {
                    painter.begin(fond);
                    qWarning("Type de couleur incorrecte (mouseReleaseEvent - Carte.cpp)");
                }
                painter.setRenderHint(QPainter::Antialiasing);
                
                // Dessin definitif sur l'image de fond, la couche alpha ou la couche alpha d'effacement
                dessiner(painter);
            }

            // Calcule de la nouvelle zone a rafraichir
            zoneNouvelle = zoneARafraichir();

            // Idem : seul le serveur peut dessiner directement sur le plan
            if (!G_client)
            {
                if (G_couleurCourante.type == efface)
                {
                    // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
                    ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal, zoneNouvelle);
                    // Apres quoi on recopie la zone concernee sur l'image de fond
                    QPainter painterFond(fond);
                    painterFond.drawImage(zoneNouvelle, *fondOriginal, zoneNouvelle);
                    // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
                    QPainter painterEfface(effaceAlpha);
                    painterEfface.fillRect(zoneNouvelle, Qt::black);
                }

                // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
                ajouterAlpha(fond, alpha, fondAlpha, zoneNouvelle);
                // Demande de rafraichissement de la fenetre (appel a paintEvent)
                update(zoneOrigine.unite(zoneNouvelle));
                // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
                afficheOuMasquePnj();
            }
            
            // On emet le trace que l'on vient d'effectuer vers les clients ou le serveur
            emettreTrace();

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
        if ((G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso) && dernierPnjSelectionne)
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

/********************************************************************/
/* Deplacement de la souris                                         */
/********************************************************************/    
void Carte::mouseMoveEvent(QMouseEvent *event)
{
    // Si le bouton gauche est enfonce
    if (boutonGaucheEnfonce && !boutonDroitEnfonce)
    {
        // Il s'agit d'une action sur les PJ/PNJ
        if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj ||
            G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        {
            actionPnjMouvementSouris(event->pos());
        }

        // Il s'agit de l'outil main
        else if (G_outilCourant == BarreOutils::main)
        {
            // On deplace la Carte dans la CarteFenetre
            emit deplacerCarteFenetre(mapToGlobal(event->pos()));
        }

        // Il s'agit d'une action de dessin
        else
        {
            // On recupere la position de la souris
            pointSouris = event->pos();
            // Calcule de la nouvelle zone a rafraichir
            zoneNouvelle = zoneARafraichir();
            // Demande de rafraichissement de la fenetre (appel a paintEvent)
            update(zoneOrigine.unite(zoneNouvelle));
            // M.a.j de la zone d'origine
            zoneOrigine = zoneNouvelle;
        }
    }

    // Si le bouton droit est enfonce et que le pointeur est dans l'image (evite un plantage)
    else if (boutonDroitEnfonce && !boutonGaucheEnfonce && (fond->rect()).contains(event->pos()))
    {
        // Recuperation de la position de la souris
        QPoint positionSouris = event->pos();
        
        // Il s'agit d'une action d'ajout ou de suppression de PNJ
        if (G_outilCourant == BarreOutils::ajoutPnj || G_outilCourant == BarreOutils::supprPnj)
        {
        }
        // Il s'agit d'une action de deplacement ou de changement d'etat de PNJ
        else if (G_outilCourant == BarreOutils::deplacePerso || G_outilCourant == BarreOutils::etatPerso)
        {
            // On met a jour l'orientation du PNJ (si la souris n'est pas sur un PNJ
            // et qu'il en existe un de selectionne)
            if (dernierPnjSelectionne && !dansDessinPerso(positionSouris))
                dernierPnjSelectionne->dessinerPersonnage(positionSouris);
        }
        // Il s'agit d'une action de dessin
        else
        {
            // Recuperation de la couleur sous le pointeur de la souris
            QColor couleur = QColor(fond->pixel(positionSouris.x(), positionSouris.y()));
            // Emission du signal demandant le changement de la couleur courante
            emit changeCouleurActuelle(couleur);
        }
    }
}

/********************************************************************/
/* Dessine les lignes, rectangles et autres sur le widget le fond,  */
/* ou la couche alpha                                               */
/********************************************************************/    
void Carte::dessiner(QPainter &painter)
{
    QColor couleurPinceau;
    
    // Choix de le couleur du trait en fonction du type de couleur selectionne
    if (G_couleurCourante.type == qcolor)
        couleurPinceau = G_couleurCourante.color;
    else if (G_couleurCourante.type == masque)
        couleurPinceau = G_couleurMasque;
    else if(G_couleurCourante.type == demasque)
        couleurPinceau = Qt::white;
    else if(G_couleurCourante.type == efface)
        couleurPinceau = Qt::white;
    else
        qWarning("Type de couleur incorrecte (dessiner - Carte.cpp)");

    // Reglage du pinceau
    QPen pen;
    pen.setWidth(G_diametreTraitCourant);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(couleurPinceau);
    painter.setPen(pen);

    // Action a effectuer en fonction de l'outil en cours d'utilisation
    if (G_outilCourant == BarreOutils::crayon)
    {
        // Seule exception : on cree un nouveau painter pour dessiner en permanence sur le fond ou la couche alpha
        QPainter painterCrayon;
        
        if (G_couleurCourante.type == qcolor)
            painterCrayon.begin(fond);
        else if (G_couleurCourante.type == masque || G_couleurCourante.type == demasque)
            painterCrayon.begin(alpha);
        else if (G_couleurCourante.type == efface)
            painterCrayon.begin(effaceAlpha);
        else
        {
            painter.begin(fond);
            qWarning("Type de couleur incorrecte (dessiner - Carte.cpp)");
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
        painterCrayon.drawLine(pointOrigine, pointSouris);
        painter.drawLine(pointOrigine, pointSouris);
        // Dessin d'un point pour permettre a l'utilisateur de ne dessiner qu'un unique point (cas ou il ne deplace pas la souris)
        painter.drawPoint(pointSouris);
        
        if (G_couleurCourante.type == efface)
        {
            // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
            ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal, zoneNouvelle);
            // Apres quoi on recopie la zone concernee sur l'image de fond
            QPainter painterFond(fond);
            painterFond.drawImage(zoneNouvelle, *fondOriginal, zoneNouvelle);
            // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
            painterCrayon.fillRect(zoneNouvelle, Qt::black);
        }
        
        // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
        ajouterAlpha(fond, alpha, fondAlpha, zoneNouvelle);

        // On ajoute la position de la souris a la liste de points destinee a l'emission
        listePointsCrayon.append(pointSouris);
        // On agglomere les zones a rafraichir pour obtenir la zone globale du trace du crayon (pour l'emission)
        zoneGlobaleCrayon = zoneGlobaleCrayon.united(zoneNouvelle);

        // M.a.j du point d'origine
        pointOrigine = pointSouris;
    }
    
    else if (G_outilCourant == BarreOutils::ligne)
    {
        // On dessine une ligne entre le point d'origine et le pointeur de la souris
        painter.drawLine(pointOrigine, pointSouris);
    }
    
    else if (G_outilCourant == BarreOutils::rectVide)
    {
        // Creation des points du rectangle a partir du point d'origine et du pointeur de souris
        QPoint supGauche, infDroit;
        supGauche.setX(pointOrigine.x()<pointSouris.x()?pointOrigine.x():pointSouris.x());
        supGauche.setY(pointOrigine.y()<pointSouris.y()?pointOrigine.y():pointSouris.y());
        infDroit.setX(pointOrigine.x()>pointSouris.x()?pointOrigine.x():pointSouris.x());
        infDroit.setY(pointOrigine.y()>pointSouris.y()?pointOrigine.y():pointSouris.y());

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
    
    else if (G_outilCourant == BarreOutils::rectPlein)
    {
        // On dessine un rectangle plein
        painter.fillRect(QRect(pointOrigine, pointSouris), couleurPinceau);
    }
    
    else if (G_outilCourant == BarreOutils::elliVide)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point d'origine
        QPoint diff = pointSouris - pointOrigine;
        QPoint nouveauPointOrigine = pointOrigine - diff;

        // Si l'ellipse est petite on dessine une ellipse pleine (correction d'un bug Qt)
        if (abs(pointSouris.x()-nouveauPointOrigine.x()) < G_diametreTraitCourant && abs(pointSouris.y()-nouveauPointOrigine.y()) < G_diametreTraitCourant)
        {
            // Redefinition des points du rectangle
            QPoint supGauche, infDroit;
            supGauche.setX(nouveauPointOrigine.x()<pointSouris.x()?nouveauPointOrigine.x():pointSouris.x());
            supGauche.setY(nouveauPointOrigine.y()<pointSouris.y()?nouveauPointOrigine.y():pointSouris.y());
            infDroit.setX(nouveauPointOrigine.x()>pointSouris.x()?nouveauPointOrigine.x():pointSouris.x());
            infDroit.setY(nouveauPointOrigine.y()>pointSouris.y()?nouveauPointOrigine.y():pointSouris.y());
            
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
            painter.drawEllipse(QRect(nouveauPointOrigine, pointSouris));
    }
    
    else if (G_outilCourant == BarreOutils::elliPlein)
    {
        // Deplacement du point superieur gauche pour que l'ellipse soit centree sur le point d'origine
        QPoint diff = pointSouris - pointOrigine;
        QPoint nouveauPointOrigine = pointOrigine - diff;

        // On dessine une ellipse pleine    
        QPen pen;
        pen.setColor(couleurPinceau);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.setBrush(couleurPinceau);
        painter.drawEllipse(QRect(nouveauPointOrigine, pointSouris));
    }
    
    else if (G_outilCourant == BarreOutils::texte)
    {
        // Parametrage de la fonte
        QFont font;
        font.setPixelSize(16);
        painter.setFont(font);
        // On affiche le texte de la zone de saisie
        painter.drawText(pointSouris, G_texteCourant);  // + QPoint(2,7)
    }
    
    else if (G_outilCourant == BarreOutils::main)
    {
    }
            
    else
        qWarning("Outil non défini lors du dessin (dessiner - Carte.cpp)");
}

/********************************************************************/
/* Calcule la zone occupee par le dessin en cours : permet de       */
/* limiter la zone a rafraichir a un rectangle                      */
/********************************************************************/    
QRect Carte::zoneARafraichir()
{
    QRect resultat;
    
    // Creation des points superieur gauche et inferieur droit a partir du point d'origine et du pointeur de la souris
    QPoint supGauche, infDroit;
    int gauche = pointOrigine.x()<pointSouris.x()?pointOrigine.x():pointSouris.x();
    int haut = pointOrigine.y()<pointSouris.y()?pointOrigine.y():pointSouris.y();
    int droit = pointOrigine.x()>pointSouris.x()?pointOrigine.x():pointSouris.x();
    int bas = pointOrigine.y()>pointSouris.y()?pointOrigine.y():pointSouris.y();
    // Calcul de la largeur et de la hauteur
    int largeur = droit - gauche + 1;
    int hauteur = bas - haut + 1;

    // Action a effectuer en fonction de l'outil en cours d'utilisation
    if (G_outilCourant == BarreOutils::crayon)
    {
        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (G_outilCourant == BarreOutils::ligne)
    {
        resultat = QRect(gauche-G_diametreTraitCourant, haut-G_diametreTraitCourant, largeur+G_diametreTraitCourant*2, hauteur+G_diametreTraitCourant*2);
    }
    
    else if (G_outilCourant == BarreOutils::rectVide)
    {
        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (G_outilCourant == BarreOutils::rectPlein)
    {
        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (G_outilCourant == BarreOutils::elliVide)
    {
        QPoint diff = pointSouris - pointOrigine;
        QPoint nouveauPointOrigine = pointOrigine - diff;
        
        int gauche = nouveauPointOrigine.x()<pointSouris.x()?nouveauPointOrigine.x():pointSouris.x();
        int haut = nouveauPointOrigine.y()<pointSouris.y()?nouveauPointOrigine.y():pointSouris.y();
        int droit = nouveauPointOrigine.x()>pointSouris.x()?nouveauPointOrigine.x():pointSouris.x();
        int bas = nouveauPointOrigine.y()>pointSouris.y()?nouveauPointOrigine.y():pointSouris.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

        resultat = QRect(gauche-G_diametreTraitCourant/2-2, haut-G_diametreTraitCourant/2-2, largeur+G_diametreTraitCourant+4, hauteur+G_diametreTraitCourant+4);
    }
    
    else if (G_outilCourant == BarreOutils::elliPlein)
    {
        QPoint diff = pointSouris - pointOrigine;
        QPoint nouveauPointOrigine = pointOrigine - diff;
        
        int gauche = nouveauPointOrigine.x()<pointSouris.x()?nouveauPointOrigine.x():pointSouris.x();
        int haut = nouveauPointOrigine.y()<pointSouris.y()?nouveauPointOrigine.y():pointSouris.y();
        int droit = nouveauPointOrigine.x()>pointSouris.x()?nouveauPointOrigine.x():pointSouris.x();
        int bas = nouveauPointOrigine.y()>pointSouris.y()?nouveauPointOrigine.y():pointSouris.y();
        int largeur = droit - gauche + 1;
        int hauteur = bas - haut + 1;

        resultat = QRect(gauche-2, haut-2, largeur+4, hauteur+4);
    }
    
    else if (G_outilCourant == BarreOutils::texte)
    {
        resultat = QRect(QPoint(pointSouris.x()-2, pointSouris.y()-15), QPoint(fond->width(), pointSouris.y()+4));
    }
    
    else if (G_outilCourant == BarreOutils::main)
    {
    }
    
    else
        qWarning("Outil non défini lors du dessin (dessiner - Carte.cpp)");

    // On coupe aux dimensions de l'image
    return resultat.intersect(fond->rect());
}

/********************************************************************/
/* Ajoute une couche alpha a une image. Ne marche que si la couche  */
/* alpha de la source est egale a 255. Peut convertir une image     */
/* ARGB32_Premultiplied en ARGB32                                   */
/********************************************************************/    
bool Carte::ajouterAlpha(QImage *source, QImage *alpha, QImage *destination, const QRect &rect)
{
    // On verifie que la source, la destination et la couche alpha ont le meme nombre de pixels
    if (source->size() != destination->size() || source->size() != alpha->size())
    {
        qWarning("La source, la destination et la couche alpha n'ont pas le meme nombre de pixels (ajouterAlpha - Carte.cpp)");
        return false;
    }

    // Initialisation de la zone a mettre a jour
    QRect zone = rect;
    if (rect.isNull())
        zone = QRect(0, 0, source->width(), source->height());

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
        for (x=zone.left(); x<finX; x++)
            pixelDestination[x+y] = (pixelSource[x+y] & 0x00FFFFFF) | ((pixelAlpha[x+y] & 0x00FF0000) << 8);
        
    return true;
}

/********************************************************************/
/* Traite les actions liees aux PNJ/PJ lorsque le bouton gauche de  */
/* la souris est enfonce                                            */
/********************************************************************/    
void Carte::actionPnjBoutonEnfonce(QPoint positionSouris)
{
    if (G_outilCourant == BarreOutils::ajoutPnj)
    {
        // On verifie que la couleur courante peut etre utilisee pour dessiner un PNJ
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
    
    else if (G_outilCourant == BarreOutils::supprPnj)
    {
        // Recuperation du DessinPerso se trouvant sous la souris
        DessinPerso *pnj = dansDessinPerso(positionSouris);
        // S'il y a un DessinPerso sous la souris...
        if (pnj)
        {
            // ...et que ce n'est pas un PJ, alors on le detruit
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
    }
    
    else if (G_outilCourant == BarreOutils::deplacePerso)
    {
        // Recuperation du DessinPerso se trouvant sous la souris (0 si aucun PNJ sous la souris)
        pnjSelectionne = dansDessinPerso(positionSouris);
        // Si la souris etait bien sur un DessinPerso
        if (pnjSelectionne)
        {
            // On calcule la difference entre le coin sup gauche du PNJ et le pointeur de la souris
            diffSourisDessinPerso = pnjSelectionne->mapFromParent(positionSouris);
            // Mise a zero de la liste de points
            listeDeplacement.clear();
            // Ajout de la position actuelle du perso dans la liste
            listeDeplacement.append(pnjSelectionne->positionCentrePerso());
        }
    }
    
    else if (G_outilCourant == BarreOutils::etatPerso)
    {
        // Recuperation du DessinPerso se trouvant sous la souris
        DessinPerso *pnj = dansDessinPerso(positionSouris);
        // S'il y a un DessinPerso sous la souris, on change son etat
        if (pnj)
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
    }

    else
        qWarning("Outil non défini lors du traitement d'une action sur les PJ/PNJ (actionPnjBoutonEnfonce - Carte.cpp)");

}

/********************************************************************/
/* Traite les actions liees aux PNJ/PJ lorsque le bouton gauche de  */
/* la souris est relache                                            */
/********************************************************************/    
void Carte::actionPnjBoutonRelache(QPoint positionSouris)
{
             Q_UNUSED(positionSouris)
    if (G_outilCourant == BarreOutils::ajoutPnj)
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
            msgBox.setWindowTitle(tr("Couleur inadaptée"));
            // On supprime l'icone de la barre de titre
            Qt::WindowFlags flags = msgBox.windowFlags();
            msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
            // On met a jour le message et on ouvre la boite d'alerte
            msgBox.setText(tr("Il est impossible d'ajouter un PNJ lorsque\nl'une des couleurs spéciales est sélectionnée."));
            msgBox.exec();
        }
    }
    
    else if (G_outilCourant == BarreOutils::supprPnj)
    {
    }
    
    else if (G_outilCourant == BarreOutils::deplacePerso)
    {
        if (pnjSelectionne)
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
    
    else if (G_outilCourant == BarreOutils::etatPerso)
    {
    }

    else
        qWarning("Outil non défini lors du traitement d'une action sur les PJ/PNJ (actionPnjBoutonRelache - Carte.cpp)");

}

/********************************************************************/
/* Traite les actions liees aux PNJ/PJ lorsque la souris bouge et   */
/* que le bouton gauche est enfonce                                 */
/********************************************************************/    
void Carte::actionPnjMouvementSouris(QPoint positionSouris)
{
    if (G_outilCourant == BarreOutils::ajoutPnj)
    {
        // Si un PNJ est selectionne, on le deplace
        if (pnjSelectionne)
        {
            // On verifie que la souris reste dans les limites de la carte
            if ( QRect(0, 0, fond->width(), fond->height()).contains(positionSouris, true) )
                pnjSelectionne->deplacePerso(positionSouris - diffSourisDessinPerso);
        }
    }
    
    else if (G_outilCourant == BarreOutils::supprPnj)
    {
    }
    
    else if (G_outilCourant == BarreOutils::deplacePerso)
    {
        // Si un PNJ est selectionne, on le deplace
        if (pnjSelectionne)
        {
            // On verifie que la souris reste dans les limites de la carte
            if ( QRect(0, 0, fond->width(), fond->height()).contains(positionSouris, true) )
            {
                // Deplacement du perso
                pnjSelectionne->deplacePerso(positionSouris - diffSourisDessinPerso);
                // Ajout de la position actuelle du perso dans la liste
                listeDeplacement.append(pnjSelectionne->positionCentrePerso());
            }
        }
    }
    
    else if (G_outilCourant == BarreOutils::etatPerso)
    {
    }

    else
        qWarning("Outil non défini lors du traitement d'une action sur les PJ/PNJ (actionPnjMouvementSouris - Carte.cpp)");

}

/********************************************************************/
/* Renvoie le DessinPerso se trouvant sous le point, ou 0 sinon     */
/********************************************************************/    
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
            qWarning ("Widget inconnu sous le pointeur de la souris (dansDessinPerso - Carte.cpp)");
            resultat = 0;
        }
    }

    // Le pointeur ne se trouve pas sur un widget
    else
        resultat = 0;

    return resultat;
}

/********************************************************************/
/* Inspecte tous les DessinPerso de la carte pour voir s'ils se       */
/* trouvent dans une zone masquee ou pas. Si oui, le DessinPerso est  */
/* masque, sinon il est affiche. Si le parametre pnjSeul est        */
/* different de 0, alors seul ce PNJ est inspecte                   */
/********************************************************************/    
void Carte::afficheOuMasquePnj(DessinPerso *pnjSeul)
{
    QObjectList enfants;
    int i, j, masque, affiche;
    QPoint contour[8];
    DessinPerso *pnj;
    QRect limites = fond->rect();

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

/********************************************************************/
/* Emssion d'une demande de changement de taille aux PJ de la carte */
/********************************************************************/
void Carte::changerTaillePjCarte(int nouvelleTaille, bool updatePj)
{
    taillePj = nouvelleTaille;
    if (updatePj)
        emit changerTaillePj(nouvelleTaille);
}

/********************************************************************/
/* Renvoie la taille des PJ de la carte                             */
/********************************************************************/
int Carte::tailleDesPj()
{
    return taillePj;
}

/********************************************************************/
/* Recherche dans la carte le personnage dont l'ID est passe en     */
/* parametre. Renvoie 0 si le personnage n'est pas trouve           */
/********************************************************************/
DessinPerso* Carte::trouverPersonnage(QString idPerso)
{
    DessinPerso *perso;

    // On recupere la liste des enfants de la carte (tous des DessinPerso)
    QObjectList enfants = children();
    // Taille de la liste
    int tailleListe = enfants.size();
    
    bool ok = false;
    // Parcours des DessinPerso
    for (int i=0; i<tailleListe && !ok; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        // On cherche le perso dont l'identifiant correspond
        if (perso->idPersonnage() == idPerso)
            ok = true;
    }
    // Le personange n'a pas ete trouve
    if (!ok)
        return 0;
    // Le personnage a ete trouve
    return perso;
}

void Carte::toggleCharacterView(Character * character)
{
    QString uuid = character->uuid();
    bool newState = !pjAffiche(uuid);
    affichageDuPj(uuid, newState);

    NetworkMessageWriter message(NetMsg::CharacterCategory, NetMsg::ToggleViewCharacterAction);
    message.string8(idCarte);
    message.string8(uuid);
    message.uint8(newState ? 1 : 0);
    message.sendAll();
}

/********************************************************************/
/* Affiche ou masque le PJ dont l'indentifiant est passe en         */
/* parametre                                                        */
/********************************************************************/
void Carte::affichageDuPj(QString idPerso, bool afficher)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (!pj)
    {
        qWarning("L'identifiant du PJ n'a pas ete trouve (affichageDuPj - Carte.cpp)");
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

/********************************************************************/
/* Regarde si le PJ dont l'identifiant est passe en parametre est   */
/* affiche ou masque                                                */
/********************************************************************/
bool Carte::pjAffiche(QString idPerso)
{
    // Recherche du PJ
    DessinPerso *pj = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (!pj)
    {
        qWarning("L'identifiant %s n'a pas ete trouve (pjAffiche - Carte.cpp)", qPrintable(idPerso));
        return false;
    }

    return pj->estVisible();
}

/********************************************************************/
/* Ajoute un PJ dans la carte                                       */
/********************************************************************/
void Carte::addCharacter(Character * person)
{
    new DessinPerso(this, person->uuid(), person->name(), person->color(), taillePj, QPoint(fond->width()/2, fond->height()/2), DessinPerso::pj);
}

/********************************************************************/
/* Suprresion du personnage dont l'ID est passe en parametre        */
/********************************************************************/
void Carte::effacerPerso(QString idPerso)
{
    // Recherche du personnage
    DessinPerso *perso = trouverPersonnage(idPerso);
    // Ne devrait jamais arriver
    if (!perso)
    {
        qWarning("L'identifiant du personnage n'a pas ete trouve (effacerPerso - Carte.cpp)");
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
        qWarning( "Person %s %s unknown in Carte::changePerson",
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
        qWarning( "Person %s %s unknown in Carte::changePerson",
                qPrintable(person->uuid()), qPrintable(person->name()) );
        return;
    }

    pj->renommerPerso(person->name());
    pj->changerCouleurPerso(person->color());
}

/********************************************************************/
/* Emet la carte vers les autres utilisateurs                       */
/********************************************************************/
void Carte::emettreCarte(QString titre)
{
    emettreCarteGeneral(titre);
}

/********************************************************************/
/* Emet la carte vers la liaison passee en parametre                */
/********************************************************************/
void Carte::emettreCarte(QString titre, Liaison * link)
{
    emettreCarteGeneral(titre, link, true);
}

/********************************************************************/
/* Emet la carte vers la liaison passee en parametre si             */
/* versLiaisonUniquement = true, vers tous les utilisateurs sinon   */
/********************************************************************/
void Carte::emettreCarteGeneral(QString titre, Liaison * link, bool versLiaisonUniquement)
{
    bool ok;

    // On commence par compresser le fond original (format jpeg) dans un tableau
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    ok = fondOriginal->save(&bufFondOriginal, "jpeg", 70);
    if (!ok)
        qWarning("Probleme de compression du fond original (emettreCarte - Carte.cpp)");

    // On compresse le fond (format jpeg) dans un tableau
    QByteArray baFond;
    QBuffer bufFond(&baFond);
    ok = fond->save(&bufFond, "jpeg", 70);
    if (!ok)
        qWarning("Probleme de compression du fond (emettreCarte - Carte.cpp)");

    // Enfin on compresse la couche alpha (format jpeg) dans un tableau
    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    ok = alpha->save(&bufAlpha, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression de la couche alpha (emettreCarte - Carte.cpp)");
    
    // Taille des donnees
    quint32 tailleCorps =
        // Taille du titre
        sizeof(quint16) + titre.size()*sizeof(QChar) +
        // Taille de l'identifiant
        sizeof(quint8) + idCarte.size()*sizeof(QChar) +
        // Taille des PJ
        sizeof(quint8) +
        // Taille de l'intensite de la couche alpha
        sizeof(quint8) +
        // Taille du fond original
        sizeof(quint32) + baFondOriginal.size() +
        // Taille du fond
        sizeof(quint32) + baFond.size() +
        // Taille de la couche alpha
        sizeof(quint32) + baAlpha.size();
            
    // Buffer d'emission
    char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

    // Creation de l'entete du message
    enteteMessage *uneEntete;
    uneEntete = (enteteMessage *) donnees;
    uneEntete->categorie = plan;
    uneEntete->action = importerPlanComplet;
    uneEntete->tailleDonnees = tailleCorps;
    
    // Creation du corps du message
    int p = sizeof(enteteMessage);
    // Ajout du titre
    quint16 tailleTitre = titre.size();
    memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
    p+=sizeof(quint16);
    memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
    p+=tailleTitre*sizeof(QChar);
    // Ajout de l'identifiant
    quint8 tailleId = idCarte.size();
    memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
    p+=sizeof(quint8);
    memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
    p+=tailleId*sizeof(QChar);
    // Ajout de la taille des PJ
    memcpy(&(donnees[p]), &taillePj, sizeof(quint8));
    p+=sizeof(quint8);
    // Ajout de l'intensite de la couche alpha
    quint8 intensiteAlpha = G_couleurMasque.red();
    memcpy(&(donnees[p]), &intensiteAlpha, sizeof(quint8));
    p+=sizeof(quint8);        
    // Ajout du fond d'origine
    quint32 tailleFondOriginal = baFondOriginal.size();
    memcpy(&(donnees[p]), &tailleFondOriginal, sizeof(quint32));
    p+=sizeof(quint32);
    memcpy(&(donnees[p]), baFondOriginal.data(), tailleFondOriginal);
    p+=tailleFondOriginal;
    // Ajout du fond
    quint32 tailleFond = baFond.size();
    memcpy(&(donnees[p]), &tailleFond, sizeof(quint32));
    p+=sizeof(quint32);
    memcpy(&(donnees[p]), baFond.data(), tailleFond);
    p+=tailleFond;
    // Ajout de la couche alpha
    quint32 tailleAlpha = baAlpha.size();
    memcpy(&(donnees[p]), &tailleAlpha, sizeof(quint32));
    p+=sizeof(quint32);
    memcpy(&(donnees[p]), baAlpha.data(), tailleAlpha);
    p+=tailleAlpha;

    if (versLiaisonUniquement)
        // Emission de la carte vers la liaison indiquee
        link->emissionDonnees(donnees, tailleCorps + sizeof(enteteMessage));
    else
        // Emission de la carte vers tous les autres utilisateurs
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
    
    // Liberation du buffer d'emission
    delete[] donnees;
}

/********************************************************************/    
/* Envoie l'ensemble des personnages (PJ ou PNJ) se trouvant sur la */
/* carte aux autres utilisateurs                                    */
/********************************************************************/    
void Carte::emettreTousLesPersonnages()
{
    emettreTousLesPersonnagesGeneral();
}

/********************************************************************/    
/* Envoie vers la liaison passee en parametre l'ensemble des        */
/* personnages (PJ ou PNJ) se trouvant sur la carte                 */
/********************************************************************/    
void Carte::emettreTousLesPersonnages(Liaison * link)
{
    emettreTousLesPersonnagesGeneral(link, true);
}

/********************************************************************/    
/* Envoie l'ensemble des personnages (PJ ou PNJ) se trouvant sur    */
/* la carte, vers la liaison passee en parametre si                 */
/* versLiaisonUniquement = true, vers tous les utilisateurs sinon   */
/********************************************************************/    
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

/********************************************************************/    
/* Envoie aux clients ou au serveur le trace dessine par            */
/* l'utilisateur                                                    */
/********************************************************************/    
void Carte::emettreTrace()
{
                qint32 tailleCorps;
    char *donnees;
    enteteMessage *uneEntete;

    // Parametres du message en fonction de l'outil en cours d'utilisation
    if (G_outilCourant == BarreOutils::crayon)
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
    
    else if (G_outilCourant == BarreOutils::texte)
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
        qint16 positionX = pointSouris.x();
        qint16 positionY = pointSouris.y();
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
    
    else if (G_outilCourant == BarreOutils::main)
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

        if (G_outilCourant == BarreOutils::ligne)
            uneEntete->action = traceLigne;
        else if (G_outilCourant == BarreOutils::rectVide)
            uneEntete->action = traceRectangleVide;
        else if (G_outilCourant == BarreOutils::rectPlein)
            uneEntete->action = traceRectanglePlein;
        else if (G_outilCourant == BarreOutils::elliVide)
            uneEntete->action = traceEllipseVide;
        else if (G_outilCourant == BarreOutils::elliPlein)
            uneEntete->action = traceEllipsePleine;
        else
        {
            qWarning("Outil non défini lors de l'emission d'un trace (emettreTrace - Carte.cpp)");
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
        qint16 departX = pointOrigine.x();
        qint16 departY = pointOrigine.y();
        memcpy(&(donnees[p]), &departX, sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&(donnees[p]), &departY, sizeof(qint16));
        p+=sizeof(qint16);
        // Ajout du point d'arrivee
        qint16 arriveeX = pointSouris.x();
        qint16 arriveeY = pointSouris.y();
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

/********************************************************************/    
/* Emet la liste de points composant le trajet du personnage que    */
/* l'utilisateur vient de deplacer, vers le serveur ou les clients  */
/********************************************************************/    
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

/********************************************************************/    
/* Dessine le trace au crayon dont les caracteristiques sont        */
/* passees en parametre                                             */
/********************************************************************/    
void Carte::dessinerTraceCrayon(QList<QPoint> *listePoints, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur, bool joueurLocal)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(fond);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(alpha);
        couleurPinceau = G_couleurMasque;
    }
    else if (couleur.type == demasque)
    {
        painter.begin(alpha);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning("Type de couleur incorrecte (dessinerTraceGeneral - Carte.cpp)");
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
        ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(fond);
        painterFond.drawImage(zoneARafraichir, *fondOriginal, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(fond, alpha, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(zoneOrigine.unite(zoneARafraichir));
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}

/********************************************************************/    
/* Dessine le texte dont les caracteristiques sont passees en       */
/* parametres                                                       */
/********************************************************************/    
void Carte::dessinerTraceTexte(QString texte, QPoint positionSouris, QRect zoneARafraichir, couleurSelectionee couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(fond);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(alpha);
        couleurPinceau = G_couleurMasque;
    }
    else if (couleur.type == demasque)
    {
        painter.begin(alpha);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning("Type de couleur incorrecte (dessinerTraceTexte - Carte.cpp)");
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
        ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(fond);
        painterFond.drawImage(zoneARafraichir, *fondOriginal, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(fond, alpha, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(zoneOrigine.unite(zoneARafraichir));
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}

/********************************************************************/    
/* Dessine un trace dont les composants sont passes en parametres.  */
/* Le trace peut etre une ligne, un rectangle plein ou vide, une    */
/* ellipse pleine ou vide                                           */
/********************************************************************/    
void Carte::dessinerTraceGeneral(actionDessin action, QPoint depart, QPoint arrivee, QRect zoneARafraichir, quint8 diametre, couleurSelectionee couleur)
{
    QPainter painter;
    QColor couleurPinceau;

    // Choix de l'image sur laquelle dessiner et de la couleur du pinceau
    if (couleur.type == qcolor)
    {
        painter.begin(fond);
        couleurPinceau = couleur.color;
    }            
    else if (couleur.type == masque)
    {
        painter.begin(alpha);
        couleurPinceau = G_couleurMasque;
    }
    else if (couleur.type == demasque)
    {
        painter.begin(alpha);
        couleurPinceau = Qt::white;
    }
    else if (couleur.type == efface)
    {
        painter.begin(effaceAlpha);
        couleurPinceau = Qt::white;
    }
    else
    {
        qWarning("Type de couleur incorrecte (dessinerTraceGeneral - Carte.cpp)");
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
        qWarning("Outil non défini lors du dessin (dessinerTraceGeneral - Carte.cpp)");

    // Si le trace effacait, on recopie de fond original sur le fond
    if (couleur.type == efface)
    {
        // Si l'utilisateur est en train d'effacer, on mixe fondOriginal et effaceAlpha
        ajouterAlpha(fondOriginal, effaceAlpha, fondOriginal, zoneARafraichir);
        // Apres quoi on recopie la zone concernee sur l'image de fond
        QPainter painterFond(fond);
        painterFond.drawImage(zoneARafraichir, *fondOriginal, zoneARafraichir);
        // Enfin on remet a zero effaceAlpha pour la prochaine utilisation
        QPainter painterEfface(effaceAlpha);
        painterEfface.fillRect(zoneARafraichir, Qt::black);
    }
    
    // Conversion de l'image de fond en ARGB32 avec ajout de la couche alpha : le resultat est stocke dans fondAlpha
    ajouterAlpha(fond, alpha, fondAlpha, zoneARafraichir);
    // Demande de rafraichissement de la fenetre (appel a paintEvent)
    update(zoneOrigine.unite(zoneARafraichir));
    // Affiche ou masque les PNJ selon qu'ils se trouvent sur une zone masquee ou pas
    afficheOuMasquePnj();
}

/********************************************************************/    
/* Verifie que l'intensite de la couche alpha passee en parametre   */
/* (et qui correspond a l'intensite presente sur la couche alpha)   */
/* est compatible avec la nature de l'utilisateur. Si ce n'est pas  */
/* le cas la couche alpha est modifiee pour correspondre a la       */
/* nature de l'utilisateur. Ceci est necessaire par ex lorsque le   */
/* serveur est MJ (couche alpha partiellement transparente) et que  */
/* le recepteur de la carte est un Joueur (la couche alpha doit     */
/* etre totalement transparente)                                    */
/********************************************************************/    
void Carte::adapterCoucheAlpha(quint8 intensiteAlpha)
{
    // Si l'intensite passee en parametre correspond a celle utilisee par l'utilisateur, aucune modif n'est necessaire : on quitte la fonction
    if (intensiteAlpha == G_couleurMasque.red())
        return;
        
    // Dans le cas contraire il faut modifier tous les pixels de la couche alpha
    
    // Calcul de la difference entre les 2 intensites
    qint16 diff = (qint16)(G_couleurMasque.red()) - (qint16)intensiteAlpha;
    // Nbr de pixels de la couche alpha
    int tailleAlpha = alpha->width() * alpha->height();
    // Pointeur vers les donnees de l'image
    QRgb *pixelAlpha = (QRgb *)alpha->bits();

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
    ajouterAlpha(fond, alpha, fondAlpha);
}

/********************************************************************/    
/* Initialise le deplacement d'un personnage                        */
/********************************************************************/    
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
            QTimer::singleShot(10, this, SLOT(deplacerLesPersonnages()));;
    }
}

/********************************************************************/    
/* Deplace l'ensemble des personnages dont le trajet est enregistre */
/* dans la liste des mouvements                                     */
/********************************************************************/    
void Carte::deplacerLesPersonnages()
{
    QPoint position;
    // Deplacement des personnages
    int i=0;
    while(i < mouvements.size())
    {
        // On recupere le DessinPerso concerne
        DessinPerso *perso = trouverPersonnage(mouvements[i].idPersonnage);

        // Si le personnage existe, on le deplace
        if (perso)
        {
            // Prochain point de deplacement
            position = mouvements[i].trajet.takeFirst();
            // Deplacement du perso
            perso->deplaceCentrePerso(position);
            // Masquage/affichage du personnage
            afficheOuMasquePnj(perso);
        }

        // Si le trajet est vide ou que le personnage est introuvable, on supprime le mouvement de la liste
        if (mouvements[i].trajet.isEmpty() || !perso)
        {
            // Suppression de l'element
            mouvements.removeAt(i);
        }
        
        // Sinon on passe a l'element suivant
        else
            i++;
    }

    // Si la liste n'est pas vide on relance le timer
    if (!mouvements.isEmpty())
        QTimer::singleShot(10, this, SLOT(deplacerLesPersonnages()));;
}


void Carte::sauvegarderCarte(QDataStream &out, QString titre)
{
    bool ok;
    // On commence par compresser le fond original (format jpeg) dans un tableau
    QByteArray baFondOriginal;
    QBuffer bufFondOriginal(&baFondOriginal);
    ok = fondOriginal->save(&bufFondOriginal, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression du fond original (sauvegarderCarte - Carte.cpp)");

    // On compresse le fond (format jpeg) dans un tableau
    QByteArray baFond;
    QBuffer bufFond(&baFond);
    ok = fond->save(&bufFond, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression du fond (sauvegarderCarte - Carte.cpp)");

    // Enfin on compresse la couche alpha (format jpeg) dans un tableau
    QByteArray baAlpha;
    QBuffer bufAlpha(&baAlpha);
    ok = alpha->save(&bufAlpha, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression de la couche alpha (sauvegarderCarte - Carte.cpp)");

    // Ecriture de la carte dans le fichier

    // Ecriture du titre
    //out << titre.size();

    //QTextStream out2(stderr,QIODevice::WriteOnly);
    //out2 << "save plan" << pos().x() << "," << pos().y()  << " size=("<< size().width()<<","<<size().height() << endl;

    out << titre;
   // out << pos();
   // out << size();
    out << taillePj;
    out << baFondOriginal;
    out << baFond;
    out << baAlpha;

    // Ecriture des PNJ (les PJ sont convertis en PNJ) dans le fichier

    DessinPerso *perso;
    // Nombre de PNJ presents sur la carte
    quint16 nombrePnj = 0;

    // On recupere la liste des enfants de la carte (tous des DessinPerso)
    QObjectList enfants = children();
    // Taille de la liste
    int tailleListe = enfants.size();

    // On parcourt une premiere fois la liste des DessinPerso pour connaitre le nombre de PJ/PNJ visibles
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        // On regarde si le personnage est visible
        if (perso->estVisible())
            nombrePnj++;
    }

    // Ajout du nbr de PNJ presents dans le fichier
    //file.write((char *)&nombrePnj, sizeof(quint16));
    out << nombrePnj;

    // On parcourt la liste des DessinPerso une 2eme fois pour ajouter les donnees de chaque PNJ
    for (int i=0; i<tailleListe; i++)
    {
        perso = (DessinPerso *)(enfants[i]);
        // Si le perso est visible, on procede a l'ecriture de ses donnees
        if (perso->estVisible())
        {
            perso->write(out);
        }
    }
}


/********************************************************************/
/* Renvoie l'identifiant de la carte                                */
/********************************************************************/
QString Carte::identifiantCarte()
{
    return idCarte;
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil crayon             */
/********************************************************************/
void Carte::pointeurCrayon()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil ligne              */
/********************************************************************/    
void Carte::pointeurLigne()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil rectangle vide     */
/********************************************************************/    
void Carte::pointeurRectVide()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil rectangle plein    */
/********************************************************************/
void Carte::pointeurRectPlein()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil ellipse vide       */
/********************************************************************/    
void Carte::pointeurElliVide()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil ellipse pleine     */
/********************************************************************/    
void Carte::pointeurElliPlein()
{
    pointeur = *G_pointeurDessin;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil texte              */
/********************************************************************/    
void Carte::pointeurTexte()
{
    pointeur = *G_pointeurTexte;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil main               */
/********************************************************************/    
void Carte::pointeurMain()
{
    pointeur = Qt::OpenHandCursor;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil ajouter PNJ        */
/********************************************************************/    
void Carte::pointeurAjoutPnj()
{
    pointeur = *G_pointeurAjouter;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil supprimer PNJ      */
/********************************************************************/    
void Carte::pointeurSupprPnj()
{
    pointeur = *G_pointeurSupprimer;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur de souris pour l'outil deplacer PJ/PNJ    */
/********************************************************************/    
void Carte::pointeurDeplacePnj()
{
    pointeur = *G_pointeurDeplacer;
    setCursor(pointeur);
}

/********************************************************************/
/* Changement du pointeur souris pour l'outil changer etat PJ/PNJ   */
/********************************************************************/    
void Carte::pointeurEtatPnj()
{
    pointeur = *G_pointeurEtat;
    setCursor(pointeur);
}
