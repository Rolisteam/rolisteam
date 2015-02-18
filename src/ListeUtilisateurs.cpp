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


#include <QtGui>

#include "ListeUtilisateurs.h"
#include "MainWindow.h"
#include "variablesGlobales.h"
#include "types.h"


/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/
// Pointeur vers l'unique instance de la liste d'utilisateurs
ListeUtilisateurs *G_listeUtilisateurs;


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
ListeUtilisateurs::ListeUtilisateurs(QWidget *parent)
        : QDockWidget(parent)
{
        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Initialisation de la variable globale contenant l'instance de la liste d'utilisateurs
        G_listeUtilisateurs = this;
        // On met a 0 la liste des joueurs clignotants
        listeClignotants.clear();
        // Le bouton de tchat commun ne clignote pas
        boutonTchatClignote = false;

        // Titre du dockWidget
        setWindowTitle(tr("Joueurs Connectés"));
        // Parametrage du dockWidget
        setAllowedAreas(Qt::AllDockWidgetAreas);
        setFeatures(QDockWidget::AllDockWidgetFeatures);

        // Creation du widget contenant le treeWidget et les boutons
        widgetUtilisateurs = new QWidget(this);
        // Creation du layout principal
        QVBoxLayout *layoutPrincipal = new QVBoxLayout(widgetUtilisateurs);
        layoutPrincipal->setMargin(0);
        layoutPrincipal->setSpacing(1);

        // Ajout d'un widget en debut de layout pour l'esthetisque
        QWidget *espace1 = new QWidget();
        espace1->setFixedHeight(1);
        layoutPrincipal->addWidget(espace1);

        // Creation du bouton de tchat commun
        boutonTchatCommun = new QPushButton (tr("Tchat commun"));
        // Changement de la couleur et du style du bouton (le changement de couleur ne fonctionne pas avec le style QWindowsXPStyle)
        QPalette palette(boutonTchatCommun->palette());
        boutonTchatCommun->setStyle(new QCleanlooksStyle());
        palette.setColor(QPalette::Normal, QPalette::Button, QColor(240,240,255));
        boutonTchatCommun->setPalette(palette);
        // Hauteur du bouton fixe
        boutonTchatCommun->setFixedHeight(40);
        // M.a.j du toolTips
        boutonTchatCommun->setToolTip(tr("Dialoguer avec tous les joueurs"));
        // Ajout du bouton au layout principal
        layoutPrincipal->addWidget(boutonTchatCommun);
        // Connexion du clic sur le bouton avec l'affichage du tchat commun
        QObject::connect(boutonTchatCommun, SIGNAL(clicked(bool)), this, SLOT(afficherTchatCommun()));

        // Ajout d'un widget entre le bouton de tchat et le liste pour l'esthetisque
        QWidget *espace2 = new QWidget();
        espace2->setFixedHeight(1);
        layoutPrincipal->addWidget(espace2);

        // Creation de l'arbre
        treeWidget = new QTreeWidget();
        // Un seul item est selectionnable a la fois
        treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        // L'arbre a 4 colonnes, seule la 1ere (qui contient le nom du joueur ou du personnage) est visible
        treeWidget->setColumnCount(6);
        // On cache les 3 dernieres colonnes
        treeWidget->hideColumn(1);	// Contient les identifiants
        treeWidget->hideColumn(2);	// Contient le type : Joueur, Joueur local, Personnage, Personnage local
        treeWidget->hideColumn(3);	// Contient le dernier etat de la case a cocher
        treeWidget->hideColumn(4);	// Contient la nature du joueur (MJ/Joueur) si s'en est un
        treeWidget->hideColumn(5);	// Contient l'etat du fond (Fixe ou Clignotant) s'il s'agit d'un joueur
        // Suppression des titres des colonnes
        QTreeWidgetItem *titre = treeWidget->headerItem();
        treeWidget->setItemHidden(titre, true);
        // Ajout du treeWidget au layout
        layoutPrincipal->addWidget(treeWidget);
        // Connexion du changement d'etat d'un objet au changement d'etat du joueur ou PJ associe
        QObject::connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(changementEtatItem(QTreeWidgetItem *, int)));
        // Connexion du clic droit sur un item a un eventuel changement de couleur du PJ ou joueur associe
        QObject::connect(treeWidget, SIGNAL(itemPressed(QTreeWidgetItem *, int)), this, SLOT(clicSurItem(QTreeWidgetItem *, int)));

        // Creation du selecteur de taille des PJ
        selecteurTaillePj = new QSlider(Qt::Horizontal);
        selecteurTaillePj->setToolTip(tr("Taille des PJ"));
        selecteurTaillePj->setRange(1,30);
        selecteurTaillePj->setValue(1);
        // Creation de l'afficheur de taille des PJ
        afficheurTaillePj = new QLabel("1");
        afficheurTaillePj->setMinimumWidth(25);
        afficheurTaillePj->setAlignment(Qt::AlignCenter);
        afficheurTaillePj->setFrameStyle(QFrame::Box | QFrame::Raised);
        afficheurTaillePj->setToolTip(tr("Taille des PJ"));
        // Creation du layout contenant le selecteur et l'afficheur de taille des PJ
        QHBoxLayout *layoutTaillePj = new QHBoxLayout();
        layoutTaillePj->setMargin(2);
        layoutTaillePj->setSpacing(4);
        // Ajout du selecteur et de l'afficheur au layout
        layoutTaillePj->addWidget(selecteurTaillePj);
        layoutTaillePj->addWidget(afficheurTaillePj);
        // Ajout du layout gerant la taille des PJ au layout principal
        layoutPrincipal->addLayout(layoutTaillePj);
        // Connexion du selecteur avec l'afficheur de taille des PJ
        QObject::connect(selecteurTaillePj, SIGNAL(valueChanged(int)), afficheurTaillePj, SLOT(setNum(int)));
        // Connexion du selecteur avec la demande de changement de taille des PJ
        QObject::connect(selecteurTaillePj, SIGNAL(valueChanged(int)), G_mainWindow, SLOT(changerTaillePj(int)));
        // Connexion du selecteur avec la demande d'emission de changement de taille des PJ
        QObject::connect(selecteurTaillePj, SIGNAL(sliderReleased()), this, SLOT(emettreChangementTaillePj()));

        // Creation des actions d'ajout et de suppression de personnage
        boutonNouveau   = new QPushButton (tr("Nouveau PJ"));
        boutonSupprimer = new QPushButton (tr("Supprimer PJ"));
        #ifdef MACOS
                // On fixe les tailles minimales des boutons, pour que les boutons restent ronds
                boutonNouveau->setFixedHeight(32);
                boutonSupprimer->setFixedHeight(32);
        #endif
        // M.a.j des toolTips
        boutonNouveau   ->setToolTip(tr("Ajouter un nouveau personnage"));
        boutonSupprimer ->setToolTip(tr("Supprimer le personnage sélectionné"));
        // Creation du layout des boutons
        QHBoxLayout *layoutBouton = new QHBoxLayout();
        // Ajout des boutons au layout
        layoutBouton->addWidget(boutonNouveau);
        layoutBouton->addWidget(boutonSupprimer);
        // Ajout du layout des bouton a celui du widget principal
        layoutPrincipal->addLayout(layoutBouton);
        // Connexion de l'action du bouton "Nouveau" avec la creation d'un nouveau PJ
        QObject::connect(boutonNouveau, SIGNAL(clicked(bool)), this, SLOT(nouveauPj(bool)));
        // Connexion de l'action du bouton "Supprimer" avec la suppression du PJ selectionne
        QObject::connect(boutonSupprimer, SIGNAL(clicked(bool)), this, SLOT(supprimerPjSelectionne(bool)));

        // Ajout d'un widget en fin de layout pour l'esthetisque
        QWidget *espace3 = new QWidget();
        espace3->setFixedHeight(1);
        layoutPrincipal->addWidget(espace3);

        // Ajout du widget au dockWidget
        setWidget(widgetUtilisateurs);
        // Largeur minimum du widget
        #ifdef WIN32
                setMinimumWidth(160);
        #elif defined (MACOS)
                setMinimumWidth(180);
        #endif

        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Ajoute un joueur a la liste. Si joueurLocal = true il s'agit du  */
/* joueur en local sur l'ordinateur. Renvoie false en cas de pb     */
/********************************************************************/
bool ListeUtilisateurs::ajouterJoueur(QString idJoueur, QString nomJoueur, QColor couleur, bool joueurLocal, bool mj)
{
        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Creation du joueur
        QTreeWidgetItem *joueur = new QTreeWidgetItem(treeWidget);
        // Nom et identifiant du joueur
        joueur->setText(0, nomJoueur);
        joueur->setText(1, idJoueur);
        // Couleur du joueur
        QImage image(15, 10, QImage::Format_ARGB32_Premultiplied);
        image.fill(couleur.rgb());
        QIcon icone(QPixmap::fromImage(image));
        joueur->setIcon(0, icone);
        // S'il s'agit du joueur local, le nom est editable
        if (joueurLocal)
        {
                joueur->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
                joueur->setToolTip(0, tr("Cliquez sur la couleur pour en changer"));
                joueur->setText(2, "Joueur local");
        }
        else
        {
                joueur->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                joueur->setText(2, "Joueur");
        }
        // La case est decochee
        joueur->setCheckState(0, Qt::Unchecked);
        joueur->setText(3, "decochee");

        // Si le joueur est le MJ on change la couleur de fond
        if (mj)
        {
                joueur->setBackground(0, QColor(200, 255, 200));
                joueur->setText(4, "MJ");
        }
        else
        {
                joueur->setBackground(0, Qt::white);
                joueur->setText(4, "Joueur");
        }

        // Le fond est fixe (il devient clignotant a la reception d'un message sur le tchat)
        joueur->setText(5, "Fixe");

        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;

        // Tout c'est passe correctement
        return true;
}

/********************************************************************/
/* Supprime le joueur dont l'identifiant est passe en parametre     */
/********************************************************************/
bool ListeUtilisateurs::supprimerJoueur(QString idJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (supprimerJoueur - ListeUtilisateurs.cpp)");
                return false;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (supprimerJoueur - ListeUtilisateurs.cpp)");
                return false;
        }

        // On commence par supprimer tous les PJ du joueur presents dans les cartes
        int nbrPersos = listeItem[0]->childCount();
        QTreeWidgetItem *personnage;
        // Parcours des personnages
        for (int p=0; p<nbrPersos; p++)
        {
                // Recuperation du personnage (on recupere tjrs le 1er car les personnages sont supprimes au fur et a mesure par supprimerPj)
                personnage = listeItem[0]->child(0);
                // Suppression du PJ dans la liste et dans les cartes
                supprimerPj(personnage);
        }

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;
        // Recuperation et suppression de l'item de la liste
        int index = treeWidget->indexOfTopLevelItem(listeItem[0]);
        QTreeWidgetItem *item = treeWidget->takeTopLevelItem(index);
        // Affichage d'un message dans le log utilisateur
        ecrireLogUtilisateur(item->text(0) + tr(" vient de quitter la partie"));
        // Destruction de l'item et de ses enfants
        item->~QTreeWidgetItem();
        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;

        // Tout c'est passe correctement
        return true;
}

/********************************************************************/
/* Ajoute un personnage au joueur indique. Si joueurLocal = true il */
/* s'agit du joueur en local sur l'ordinateur. Renvoie false en cas */
/* de pb                                                            */
/********************************************************************/
bool ListeUtilisateurs::ajouterPersonnage(QString idJoueur, QString idPerso, QString nomPerso, QColor couleur, bool joueurLocal, bool creerDessins)
{
        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (ajouterPersonnage - ListeUtilisateurs.cpp)");
                return false;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (ajouterPersonnage - ListeUtilisateurs.cpp)");
                return false;
        }
        // Creation du personnage
        QTreeWidgetItem *personnage = new QTreeWidgetItem(listeItem[0]);
        // Nom et identifiant du personnage
        personnage->setText(0, nomPerso);
        personnage->setText(1, idPerso);
        // Couleur du personnage
        QImage image(15, 10, QImage::Format_ARGB32_Premultiplied);
        image.fill(couleur.rgb());
        QIcon icone(QPixmap::fromImage(image));
        personnage->setIcon(0, icone);
        // S'il s'agit du joueur local, le personnage est editable et selectionnable
        if (joueurLocal)
        {
                // Le personnage n'est cochable que si la fenetre active est une CarteFenetre
                if (G_carteFenetreActive)
                        personnage->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                else
                        personnage->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                personnage->setToolTip(0, tr("Cliquez sur la couleur pour en changer"));
                personnage->setText(2, "Personnage local");
        }
        else
        {
                // Le personnage n'est cochable que si la fenetre active est une CarteFenetre
                if (G_carteFenetreActive)
                        personnage->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                else
                        personnage->setFlags(Qt::ItemIsEnabled);
                personnage->setText(2, "Personnage");
        }
        // La case est decochee
        personnage->setCheckState(0, Qt::Unchecked);
        personnage->setText(3, "decochee");

        if (joueurLocal)
        {
                // On developpe l'arbre du joueur
                treeWidget->setItemExpanded(listeItem[0], true);
                // L'element est selectionne et ouvert
                treeWidget->editItem(personnage, 0);
        }

        // Si creerDessins = true on cree les dessins du PJ dans toutes les cartes
        if (creerDessins)
                emit ajouterPj(idPerso, nomPerso, couleur);

        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;

        // Tout c'est passe correctement
        return true;
}

/********************************************************************/
/* Change le nom d'un joueur                                        */
/********************************************************************/
void ListeUtilisateurs::ModifierNomJoueur(QString idJoueur, QString nomJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (changerCouleurJoueur - ListeUtilisateurs.cpp)");
                return;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (changerCouleurJoueur - ListeUtilisateurs.cpp)");
                return;
        }

        // Affichage d'un message dans le log utilisateur
        ecrireLogUtilisateur(listeItem[0]->text(0) + tr(" se nomme désormais ") + nomJoueur);

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;
        // On change le nom du joueur
        listeItem[0]->setText(0, nomJoueur);
        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Change la couleur d'un joueur                                    */
/********************************************************************/
void ListeUtilisateurs::ModifierCouleurJoueur(QString idJoueur, QColor CouleurJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (ModifierCouleurJoueur - ListeUtilisateurs.cpp)");
                return;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (ModifierCouleurJoueur - ListeUtilisateurs.cpp)");
                return;
        }

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;
        // Changement de la couleur du joueur
        QImage image(15, 10, QImage::Format_ARGB32_Premultiplied);
        image.fill(CouleurJoueur.rgb());
        QIcon icone(QPixmap::fromImage(image));
        listeItem[0]->setIcon(0, icone);
        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;

        // On affiche un message dans la fenetre de log utilisateur
        ecrireLogUtilisateur(listeItem[0]->text(0) + tr(" vient de changer sa couleur"));
}

/********************************************************************/
/* Change le nom d'un personnage                                    */
/********************************************************************/
void ListeUtilisateurs::ModifierNomPerso(QString idPerso, QString nomPerso)
{
        int nbrPersos;
        int j, p;
        QTreeWidgetItem *personnage, *joueur;
        bool trouve = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos && !trouve; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);
                        // Verification de l'identifiant
                        if (personnage->text(1) == idPerso)
                                trouve = true;
                }
        }

        // Si le personnage n'a pas ete trouve
        if (!trouve)
                qWarning("Le personnage recherche n'existe pas (ModifierNomPerso - ListeUtilisateurs.cpp)");

        // Si on l'a trouve on change son nom
        else
        {
                // Affichage d'un message dans le log utilisateur
                ecrireLogUtilisateur(tr("Le personnage ") + personnage->text(0) + tr(" de ") + joueur->text(0) + tr(" se nomme désormais ") + nomPerso);

                // On interdit la reception des signaux dans changementEtatItem
                autoriserSignauxListe = false;
                // On change le nom du joueur
                personnage->setText(0, nomPerso);
                // On autorise la reception des signaux dans changementEtatItem
                autoriserSignauxListe = true;

                // On demande aux cartes de renommer le PJ
                emit renommerPj(personnage->text(1), nomPerso);
        }
}

/********************************************************************/
/* Change la couleur du personnage                                  */
/********************************************************************/
void ListeUtilisateurs::ModifierCouleurPerso(QString idPerso, QColor CouleurPerso)
{
        int nbrPersos;
        int j, p;
        QTreeWidgetItem *personnage, *joueur;
        bool trouve = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos && !trouve; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);
                        // Verification de l'identifiant
                        if (personnage->text(1) == idPerso)
                                trouve = true;
                }
        }

        // Si le personnage n'a pas ete trouve
        if (!trouve)
                qWarning("Le personnage recherche n'existe pas (ModifierCouleurPerso - ListeUtilisateurs.cpp)");

        // Si on l'a trouve on change sa couleur
        else
        {
                // On interdit la reception des signaux dans changementEtatItem
                autoriserSignauxListe = false;
                // Changement de la couleur du personnage
                QImage image(15, 10, QImage::Format_ARGB32_Premultiplied);
                image.fill(CouleurPerso.rgb());
                QIcon icone(QPixmap::fromImage(image));
                personnage->setIcon(0, icone);
                // On autorise la reception des signaux dans changementEtatItem
                autoriserSignauxListe = true;

                // Emission d'une demande de changement de couleur du PJ vers les cartes
                emit changerCouleurPerso(personnage->text(1), CouleurPerso);

                // On affiche un message dans la fenetre de log utilisateur
                ecrireLogUtilisateur(joueur->text(0) + tr(" vient de changer la couleur de son personnage ") + personnage->text(0));
        }
}

/********************************************************************/
/* M.a.j du selecteur de taille des PJ                              */
/********************************************************************/
void ListeUtilisateurs::majTaillePj(int taille)
{
        selecteurTaillePj->setValue(taille);
}

/********************************************************************/
/* Creation d'un nouveau PJ en local                                */
/********************************************************************/
void ListeUtilisateurs::nouveauPj(bool checked)
{
        // Creation de l'identifiant
        QString idPerso = QUuid::createUuid().toString();
        // Generation d'une couleur aleatoire
        QColor couleur;
        couleur.setHsv(rand()%360, rand()%200 + 56, rand()%190 + 50);
        // Nom du personnage
        QString nomPerso("Nouveau personnage");
        // Ajout du personnage
        ajouterPersonnage(G_idJoueurLocal, idPerso, nomPerso, couleur, true);

        // Emission d'une demande de creation de PJ vers le serveur ou les clients

        // Taille des donnees
        quint32 tailleCorps =
                // Taille de l'identifiant du joueur
                sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
                // Taille de l'identifiant du PJ
                sizeof(quint8) + idPerso.size()*sizeof(QChar) +
                // Taille du nom du personnage
                sizeof(quint16) + nomPerso.size()*sizeof(QChar) +
                // Taille de la couleur
                sizeof(QRgb) +
                // Taille de l'information indiquant si les DesinPerso associes doivent etre ajoutes (1) ou pas (0) dans les cartes
                sizeof(quint8);

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = persoJoueur;
        uneEntete->action = ajouterPersoJoueur;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant du joueur
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'identifiant du PJ
        quint8 tailleIdPj = idPerso.size();
        memcpy(&(donnees[p]), &tailleIdPj, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idPerso.data(), tailleIdPj*sizeof(QChar));
        p+=tailleIdPj*sizeof(QChar);
        // Ajout du nom du PJ
        quint16 tailleNom = nomPerso.size();
        memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), nomPerso.data(), tailleNom*sizeof(QChar));
        p+=tailleNom*sizeof(QChar);
        // Ajout de la couleur
        QRgb rgb = couleur.rgb();
        memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
        p+=sizeof(QRgb);
        // Ajout d'une information indiquant si les DesinPerso associes doivent etre ajoutes (1) ou pas (0) dans les cartes
        quint8 dessin = 1;
        memcpy(&(donnees[p]), &dessin, sizeof(quint8));
        p+=sizeof(quint8);

        // Emission de la demande de creation de PJ au serveur ou a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
}

/********************************************************************/
/* Suppression du PJ selectionne                                    */
/********************************************************************/
void ListeUtilisateurs::supprimerPjSelectionne(bool checked)
{
        // Recuperation de la liste des elements selectionnees
        QList<QTreeWidgetItem *> liste = treeWidget->selectedItems();

        // Si aucun element n'est selectionne, on quitte (plus d'un seul element : ne doit jamais arriver)
        if (liste.size() != 1)
                return;

        // Recuperation de l'identifiant du PJ
        QString idPerso = liste[0]->text(1);

        // On supprime le PJ de la liste et des cartes
        supprimerPj(liste[0]);

        // On emet l'identifiant du PJ a supprimer

        // Taille des donnees
        quint32 tailleCorps =
                // Taille de l'identifiant du PJ
                sizeof(quint8) + idPerso.size()*sizeof(QChar);

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = persoJoueur;
        uneEntete->action = supprimerPersoJoueur;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant du PJ
        quint8 tailleIdPj = idPerso.size();
        memcpy(&(donnees[p]), &tailleIdPj, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idPerso.data(), tailleIdPj*sizeof(QChar));
        p+=tailleIdPj*sizeof(QChar);

        // Emission de la demande de suppression de PJ au serveur ou a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
}

/********************************************************************/
/* Supprime le PJ dont l'identifiant est passe en parametre         */
/********************************************************************/
bool ListeUtilisateurs::supprimerPersonnage(QString idPerso)
{
        int nbrPersos;
        int j, p;
        QTreeWidgetItem *personnage, *joueur;
        bool trouve = false, result;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos && !trouve; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);
                        // Verification de l'identifiant
                        if (personnage->text(1) == idPerso)
                                trouve = true;
                }
        }

        // Si le personnage n'a pas ete trouve
        if (!trouve)
        {
                qWarning("Le personnage recherche n'existe pas (supprimerPersonnage - ListeUtilisateurs.cpp)");
                // Une erreur s'est produite
                result = false;
        }

        // Si on l'a trouve on le supprime de la liste et des cartes
        else
        {
                // Message dans le log utilisateur
                ecrireLogUtilisateur(joueur->text(0) + tr(" vient de supprimer le personnage ") + personnage->text(0));
                // Suppression du PJ
                supprimerPj(personnage);
                // tout c'est passe correctement
                result = true;
        }

        return result;
}

/********************************************************************/
/* Supprime de la liste et des cartes le PJ passe en parametre      */
/********************************************************************/
void ListeUtilisateurs::supprimerPj(QTreeWidgetItem *item)
{
        // Demande de suppression du PJ dans l'ensemble des cartes
        emit effacerPj(item->text(1));
        // On recupere le parent de l'item
        QTreeWidgetItem *parent = item->parent();
        // On recupere le numero d'index du parent
        int index = parent->indexOfChild(item);
        // On supprime le personnage de l'arbre
        parent->takeChild(index);
        // Destruction du PJ dans l'arbre
        item->~QTreeWidgetItem();
}

/********************************************************************/
/* Ajoute tous les PJ de la liste a une nouvelle carte              */
/********************************************************************/
void ListeUtilisateurs::ajouterTousLesPj(Carte *carte)
{
        int nbrPersos;
        int j, p;
        QTreeWidgetItem *personnage, *joueur;
        QString nomPerso, idPerso;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);
                        // Recuperation du nom et de l'identifiant
                        nomPerso = personnage->text(0);
                        idPerso = personnage->text(1);
                        // Recuperation de la couleur
                        QColor couleur(personnage->icon(0).pixmap(20,12).toImage().pixel(0,0));

                        // Ajout du personnage
                        carte->ajouterPj(idPerso, nomPerso, couleur);
                }
        }
}

/********************************************************************/
/* Recherche dans la carte tous les PJ presents dans la liste et    */
/* verifie s'ils sont affiches ou masques. L'etat d'affichage est   */
/* mis a jour en fonction                                           */
/********************************************************************/
void ListeUtilisateurs::majAffichagePj(Carte *carte)
{
        int nbrPersos;
        int j, p;
        Qt::ItemFlags flags;
        bool affiche;
        QTreeWidgetItem *personnage, *joueur;
        QString idPerso;

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos; p++)
                {
                        // Recuperation du personnage et de son identifiant
                        personnage = joueur->child(p);
                        idPerso = personnage->text(1);

                        // La case est affichee et cochable
                        if (joueur->text(1) == G_idJoueurLocal)
                                personnage->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
                        else
                                personnage->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

                        // Quel est l'etat d'affichage du PJ dans la carte?
                        affiche = carte->pjAffiche(idPerso);
                        // On met a jour la case du PJ dans la liste
                        if (affiche)
                        {
                                personnage->setCheckState(0, Qt::Checked);
                                personnage->setText(3, "cochee");
                        }
                        else
                        {
                                personnage->setCheckState(0, Qt::Unchecked);
                                personnage->setText(3, "decochee");
                        }
                }
        }
        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Les cases d'affichage/masquage des PJ deviennent impossibles a   */
/* selectionner                                                     */
/********************************************************************/
void ListeUtilisateurs::casesPjNonSelectionnables()
{
        int nbrPersos;
        int j, p;
        Qt::ItemFlags flags;
        bool affiche;
        QTreeWidgetItem *personnage, *joueur;
        QString idPerso;

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);

                        // La case est affichee mais impossible a cocher
                        if (joueur->text(1) == G_idJoueurLocal)
                                personnage->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                        else
                                personnage->setFlags(Qt::ItemIsEnabled);

                        // La case est decochee
                        personnage->setCheckState(0, Qt::Unchecked);
                        personnage->setText(3, "decochee");
                }
        }
        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Code/decoche un PJ                                               */
/********************************************************************/
void ListeUtilisateurs::cocherDecocherPj(QString idPerso, bool cocher)
{
        int nbrPersos;
        int j, p;
        QTreeWidgetItem *personnage, *joueur;
        bool trouve = false, result;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (p=0; p<nbrPersos && !trouve; p++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(p);
                        // Verification de l'identifiant
                        if (personnage->text(1) == idPerso)
                                trouve = true;
                }
        }

        // Si le personnage n'a pas ete trouve
        if (!trouve)
                qWarning("Le personnage recherche n'existe pas (cocherDecocherPj - ListeUtilisateurs.cpp)");

        else
        {
                // On interdit la reception des signaux dans changementEtatItem
                autoriserSignauxListe = false;

                // On m.a.j la case du PJ
                if (cocher)
                {
                        personnage->setCheckState(0, Qt::Checked);
                        personnage->setText(3, "cochee");
                }
                else
                {
                        personnage->setCheckState(0, Qt::Unchecked);
                        personnage->setText(3, "decochee");
                }

                // On autorise la reception des signaux dans changementEtatItem
                autoriserSignauxListe = true;
        }
}

/********************************************************************/
/* Verifie le type d'item dont l'etat a change. S'il s'agit d'un    */
/* joueur on affiche/masque la fenetre de tchat associee. S'il      */
/* s'agit d'un PJ on l'affiche/masque dans la fenetre active.       */
/* Permet egalement de changer le nom du joueur et des PJ locaux    */
/********************************************************************/
void ListeUtilisateurs::changementEtatItem(QTreeWidgetItem *item, int colonne)
{
        // Si les signaux ne sont pas autorises, on quitte la fonction
        if (autoriserSignauxListe == false)
                return;

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // Modification d'un joueur distant
        if (item->text(2) == "Joueur")
        {
                // Si la case a ete cochee on affiche le tchat
                if (item->checkState(0)==Qt::Checked)
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "cochee");
                        // Affichage du tchat
                        G_mainWindow->afficherTchat(item->text(1));
                }
                // Si la case a ete decochee on masque le tchat
                else if (item->checkState(0)==Qt::Unchecked)
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "decochee");
                        // Masquage du tchat
                        G_mainWindow->masquerTchat(item->text(1));
                }
        }

        // Modification du joueur local
        else if (item->text(2) == "Joueur local")
        {
                // Si la case a ete cochee on affiche l'editeur de notes
                if (item->checkState(0)==Qt::Checked && item->text(3)=="decochee")
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "cochee");
                        G_mainWindow->afficherEditeurNotes(true, true);
                }
                // Si la case a ete decochee on masque l'editeur de notes
                else if (item->checkState(0)==Qt::Unchecked && item->text(3)=="cochee")
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "decochee");
                        G_mainWindow->afficherEditeurNotes(false, true);
                }

                // Si le nom du joueur local a ete modifie on previent les autres joueurs
                else
                {
                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille du nom
                                sizeof(quint16) + item->text(0).size()*sizeof(QChar) +
                                // Taille de l'identifiant
                                sizeof(quint8) + item->text(1).size()*sizeof(QChar);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = joueur;
                        uneEntete->action = changerNomJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout du nom
                        quint16 tailleNom = item->text(0).size();
                        memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
                        p+=sizeof(quint16);
                        memcpy(&(donnees[p]), item->text(0).data(), tailleNom*sizeof(QChar));
                        p+=tailleNom*sizeof(QChar);
                        // Ajout de l'identifiant
                        quint8 tailleId = item->text(1).size();
                        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), item->text(1).data(), tailleId*sizeof(QChar));
                        p+=tailleId*sizeof(QChar);

                        // Emission de la demande de changement de nom au serveur ou a l'ensemble des clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
        }

        // Modification d'un personnage
        else if (item->text(2) == "Personnage")
        {
                // Si la case a ete cochee on affiche le personnage dans la carte
                if (item->checkState(0)==Qt::Checked)
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "cochee");
                        G_mainWindow->affichageDuPj(item->text(1), true);
                }
                // Si la case a ete decochee on masque le personnage dans la carte
                else if (item->checkState(0)==Qt::Unchecked)
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "decochee");
                        G_mainWindow->affichageDuPj(item->text(1), false);
                }
        }

        // Modification du personnage local
        else if (item->text(2) == "Personnage local")
        {
                // Si la case a ete cochee on affiche le personnage dans la carte
                if (item->checkState(0)==Qt::Checked && item->text(3)=="decochee")
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "cochee");
                        G_mainWindow->affichageDuPj(item->text(1), true);
                }
                // Si la case a ete decochee on masque le personnage dans la carte
                else if (item->checkState(0)==Qt::Unchecked && item->text(3)=="cochee")
                {
                        // M.a.j de la 4eme colonne
                        item->setText(3, "decochee");
                        G_mainWindow->affichageDuPj(item->text(1), false);
                }

                // Si le nom du PJ local a ete modifie on le m.a.j sur la carte et sur le serveur
                else
                {
                        // On demande aux cartes de renommer le PJ (on passe l'ID et le nouveau nom)
                        emit renommerPj(item->text(1), item->text(0));

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille du nom
                                sizeof(quint16) + item->text(0).size()*sizeof(QChar) +
                                // Taille de l'identifiant
                                sizeof(quint8) + item->text(1).size()*sizeof(QChar);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = changerNomPersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout du nom
                        quint16 tailleNom = item->text(0).size();
                        memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
                        p+=sizeof(quint16);
                        memcpy(&(donnees[p]), item->text(0).data(), tailleNom*sizeof(QChar));
                        p+=tailleNom*sizeof(QChar);
                        // Ajout de l'identifiant
                        quint8 tailleId = item->text(1).size();
                        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), item->text(1).data(), tailleId*sizeof(QChar));
                        p+=tailleId*sizeof(QChar);

                        // Emission de la demande de changement de nom au serveur ou a l'ensemble des clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
        }

        // Ne devrait jamais arriver
        else
                qWarning("Entree inexpliquee dans la fonction (changementEtatItem - ListeUtilisateurs.cpp)");

        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Verifie que l'item est un joueur local ou PJ local. Si oui,  un  */
/* changement de couleur peut avoir lieu                            */
/********************************************************************/
void ListeUtilisateurs::clicSurItem(QTreeWidgetItem *item, int colonne)
{
        // Si l'item n'est pas un joueur local ou un personnage local, on quitte
        if (item->text(2) != "Joueur local" && item->text(2) != "Personnage local")
                return;

        // On interdit la reception des signaux dans changementEtatItem
        autoriserSignauxListe = false;

        // On recupere le rectangle de l'item
        QRect rect = treeWidget->visualItemRect(item);
        // On calcule la position de la souris par rapport au rectangle
        QPoint souris = treeWidget->mapFromGlobal(QCursor::pos()) - rect.topLeft();

        // Si la souris est dans l'icone de couleur, on peut changer la couleur
#ifdef WIN32
        if (QRect(25, 3, 15, 10).contains(souris))
#elif defined (Q_WS_MAC)
        if (QRect(37, 8, 15, 10).contains(souris))
#elif defined (Q_WS_X11)
        if (QRect(25, 3, 15, 10).contains(souris))
#endif
        {
                // On fait appel au selecteur de couleur
                QColor couleur = QColorDialog::getColor(item->icon(0).pixmap(20,12).toImage().pixel(0,0));
                // Si l'utilisateur a clique sur OK on recupere la nouvelle couleur
                if (couleur.isValid())
                {
                        // Changement de la couleur du personnage / joueur
                        QImage image(15, 10, QImage::Format_ARGB32_Premultiplied);
                        image.fill(couleur.rgb());
                        QIcon icone(QPixmap::fromImage(image));
                        item->setIcon(0, icone);
                        // S'il s'agit d'un personnage local
                        if (item->text(2) == "Personnage local")
                        {
                                // Emission d'une demande de changement de couleur du PJ vers les cartes
                                emit changerCouleurPerso(item->text(1), couleur);
                        }

                        // Envoie d'un message indiquant un changement de couleur d'un joueur ou d'un personnage

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant
                                sizeof(quint8) + item->text(1).size()*sizeof(QChar) +
                                // Taille de la couleur
                                sizeof(QRgb);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];
                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->tailleDonnees = tailleCorps;
                        // S'il s'agit d'un personnage local
                        if (item->text(2) == "Personnage local")
                        {
                                uneEntete->categorie = persoJoueur;
                                uneEntete->action = changerCouleurPersoJoueur;
                        }
                        // S'il s'agit d'un joueur local
                        else
                        {
                                uneEntete->categorie = joueur;
                                uneEntete->action = changerCouleurJoueur;
                        }
                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant
                        quint8 tailleId = item->text(1).size();
                        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), item->text(1).data(), tailleId*sizeof(QChar));
                        p+=tailleId*sizeof(QChar);
                        // Ajout de la couleur
                        QRgb rgb = couleur.rgb();
                        memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
                        p+=sizeof(QRgb);

                        // Emission de la demande de changement de nom au serveur ou a l'ensemble des clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;

                }

                #ifdef WIN32
                        // Mise a jour des couleurs personnelles du selecteur de la barre d'outils
                        G_mainWindow->majCouleursPersonnelles();
                #endif
        }

        // On autorise la reception des signaux dans changementEtatItem
        autoriserSignauxListe = true;
}

/********************************************************************/
/* Renvoie la taille actuelle des PJ                                */
/********************************************************************/
int ListeUtilisateurs::taillePj()
{
        return selecteurTaillePj->value();
}

/********************************************************************/
/* Renvoie la liste de tous les utilisateurs connectes              */
/********************************************************************/
QList<utilisateur> ListeUtilisateurs::tousLesUtilisateurs()
{
        QList<utilisateur> utilisateursConnectes;
        QTreeWidgetItem *joueur;
        utilisateur util;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (int j=0; j<nbrJoueurs; j++)
        {
                // Recuperation du joueur
                joueur = treeWidget->topLevelItem(j);
                // Recuperation des nom et identifiant du joueur
                util.nomJoueur = joueur->text(0);
                util.idJoueur = joueur->text(1);
                // Recuperation de la couleur
                util.couleurJoueur = joueur->icon(0).pixmap(20,12).toImage().pixel(0,0);
                // Recuperation de la nature du joueur : MJ ou joueur
                util.mj = joueur->text(4) == "MJ" ? true : false;
                // Ajout de l'utilisateur a la liste
                utilisateursConnectes.append(util);
        }

        return utilisateursConnectes;
}

/********************************************************************/
/* Renvoie l'identifiant de l'utilisateur dont l'index est passe    */
/* en parametre                                                     */
/********************************************************************/
QString ListeUtilisateurs::indentifiantUtilisateur(int index)
{
        // Recuperation du joueur
        QTreeWidgetItem *joueur = treeWidget->topLevelItem(index);

        // On renvoie d'identifiant du joueur
        if (joueur)
                return joueur->text(1);
        else
                return "";
}

/********************************************************************/
/* Renvoie le nom de l'utilisateur dont l'identifiant est passe en  */
/* parametre                                                        */
/********************************************************************/
QString ListeUtilisateurs::nomUtilisateur(QString idJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (nomUtilisateur - ListeUtilisateurs.cpp)");
                return "";
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (nomUtilisateur - ListeUtilisateurs.cpp)");
                return "";
        }

        // On renvoie le nom du joueur
        return listeItem[0]->text(0);
}

/********************************************************************/
/* Renvoie le numero d'index de l'utilisateur dont l'identifiant    */
/* est passe en parametre                                           */
/********************************************************************/
int ListeUtilisateurs::numeroUtilisateur(QString idJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (numeroUtilisateur - ListeUtilisateurs.cpp)");
                return -1;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (numeroUtilisateur - ListeUtilisateurs.cpp)");
                return -1;
        }

        // On renvoie l'index du joueur
        return treeWidget->indexOfTopLevelItem(listeItem[0]);
}

/********************************************************************/
/* Emet tous les personnages joueurs de la liste vers la liaison    */
/* passee en parametre                                              */
/********************************************************************/
void ListeUtilisateurs::emettreTousLesPj(QString idJoueur)
{
        int nbrPersos;
        int j, t;
        QTreeWidgetItem *personnage, *joueur;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs; j++)
        {
                // Recuperation du nbr de personnages du joueur
                joueur = treeWidget->topLevelItem(j);
                nbrPersos = joueur->childCount();
                // Parcours des personnages
                for (t=0; t<nbrPersos; t++)
                {
                        // Recuperation du personnage
                        personnage = joueur->child(t);

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant du joueur
                                sizeof(quint8) + joueur->text(1).size()*sizeof(QChar) +
                                // Taille de l'identifiant du PJ
                                sizeof(quint8) + personnage->text(1).size()*sizeof(QChar) +
                                // Taille du nom du personnage
                                sizeof(quint16) + personnage->text(0).size()*sizeof(QChar) +
                                // Taille de la couleur
                                sizeof(QRgb) +
                                // Taille de l'information indiquant si les DesinPerso associes doivent etre ajoutes (1) ou pas (0) dans les cartes
                                sizeof(quint8);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = ajouterPersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant du joueur
                        quint8 tailleIdJoueur = joueur->text(1).size();
                        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), joueur->text(1).data(), tailleIdJoueur*sizeof(QChar));
                        p+=tailleIdJoueur*sizeof(QChar);
                        // Ajout de l'identifiant du PJ
                        quint8 tailleIdPj = personnage->text(1).size();
                        memcpy(&(donnees[p]), &tailleIdPj, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), personnage->text(1).data(), tailleIdPj*sizeof(QChar));
                        p+=tailleIdPj*sizeof(QChar);
                        // Ajout du nom du PJ
                        quint16 tailleNom = personnage->text(0).size();
                        memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
                        p+=sizeof(quint16);
                        memcpy(&(donnees[p]), personnage->text(0).data(), tailleNom*sizeof(QChar));
                        p+=tailleNom*sizeof(QChar);
                        // Ajout de la couleur
                        QColor couleur(personnage->icon(0).pixmap(20,12).toImage().pixel(0,0));
                        QRgb rgb = couleur.rgb();
                        memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
                        p+=sizeof(QRgb);
                        // Ajout d'une information indiquant si les DesinPerso associes doivent etre ajoutes (1) ou pas (0) dans les cartes
                        quint8 dessin = 0;
                        memcpy(&(donnees[p]), &dessin, sizeof(QRgb));
                        p+=sizeof(quint8);

                        // On recupere le numero de liaison correspondant a l'identifiant du joueur
                        // (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
                        // n'a pas de liaison associee)
                        int numeroLiaison = numeroUtilisateur(idJoueur) - 1;

                        // Emission de la demande de creation de PJ vers la liaison indiquee
                        emettre(donnees, tailleCorps + sizeof(enteteMessage), numeroLiaison);
                        // Liberation du buffer d'emission
                        delete[] donnees;

                }	// Fin de parcours des personnages
        }		// Fin de parcours des joueurs
}

/********************************************************************/
/* Releve la valeur du selecteur de taille et demande l'emission    */
/* de cette valeur aux clients ou au serveur                        */
/********************************************************************/
void ListeUtilisateurs::emettreChangementTaillePj()
{
        // Recuperation de la taille des PJ
        int taillePj = selecteurTaillePj->value();
        // Emission de la demande de changement de taille des PJ pour la carte active
        G_mainWindow->emettreChangementTaillePj(taillePj);
}

/********************************************************************/
/* Affiche le tchat commun                                          */
/********************************************************************/
void ListeUtilisateurs::afficherTchatCommun()
{
        // Demande d'affichage du tchat dont l'ID est vide
        G_mainWindow->afficherTchat("");
        // Le tchat commun devient la fenetre active
        G_mainWindow->devientFenetreActive(G_mainWindow->trouverTchat(""));
        // On met a jour l'action du sous-menu Tchats
        G_mainWindow->cocherActionTchatCommun();
}

/********************************************************************/
/* Coche la case de tchat de l'utilisateur dont l'ID est passe en   */
/* parametre                                                        */
/********************************************************************/
void ListeUtilisateurs::cocherCaseTchat(QString idJoueur)
{
        int j;
        QTreeWidgetItem *joueur;
        bool trouve = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du joueur
                joueur = treeWidget->topLevelItem(j);
                // Verification de l'identifiant
                if (joueur->text(1) == idJoueur)
                        trouve = true;
        }

        // Si le joueur n'a pas ete trouve
        if (!trouve)
                qWarning("Le joueur recherche n'existe pas (cocherCaseTchat - ListeUtilisateurs.cpp)");

        else
        {
                // On interdit la reception des signaux dans changementEtatItem
                autoriserSignauxListe = false;

                // On decoche la case du joueur
                joueur->setCheckState(0, Qt::Checked);
                joueur->setText(3, "cochee");

                // On autorise la reception des signaux dans changementEtatItem
                autoriserSignauxListe = true;
        }
}

/********************************************************************/
/* Decoche la case de tchat de l'utilisateur dont l'ID est passe en */
/* parametre                                                        */
/********************************************************************/
void ListeUtilisateurs::decocherCaseTchat(QString idJoueur)
{
        int j;
        QTreeWidgetItem *joueur;
        bool trouve = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du joueur
                joueur = treeWidget->topLevelItem(j);
                // Verification de l'identifiant
                if (joueur->text(1) == idJoueur)
                        trouve = true;
        }

        // Si le joueur n'a pas ete trouve
        if (!trouve)
                qWarning("Le joueur recherche n'existe pas (decocherCaseTchat - ListeUtilisateurs.cpp)");

        else
        {
                // On interdit la reception des signaux dans changementEtatItem
                autoriserSignauxListe = false;

                // On decoche la case du joueur
                joueur->setCheckState(0, Qt::Unchecked);
                joueur->setText(3, "decochee");

                // On autorise la reception des signaux dans changementEtatItem
                autoriserSignauxListe = true;
        }
}

/********************************************************************/
/* Renvoie la couleur du joueur dont l'ID est passe en parametre    */
/********************************************************************/
QColor ListeUtilisateurs::couleurUtilisateur(QString idJoueur)
{
        // Recherche du joueur dans la liste, en se basant sur l'idJoueur
        QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
        // Si la liste n'a pas un seul element, alors il y a un probleme
        if (listeItem.size() == 0)
        {
                qWarning("Le joueur recherche n'existe pas (couleurUtilisateur - ListeUtilisateurs.cpp)");
                return Qt::white;
        }
        else if (listeItem.size() > 1)
        {
                qWarning("Plusieurs joueurs possedent l'identifiant recherche (couleurUtilisateur - ListeUtilisateurs.cpp)");
                return Qt::white;
        }

        // Recuperation et emission de la couleur
        QColor couleur(listeItem[0]->icon(0).pixmap(20,12).toImage().pixel(0,0));
        return couleur;
}

/********************************************************************/
/* Fait clignoter l'utilisateur dont l'ID est passe en parametre    */
/********************************************************************/
void ListeUtilisateurs::faireClignoter(QString idJoueur)
{
        // Si l'ID est vide, il s'agit du tchat commun : on fait clignoter le bouton
        if (idJoueur.isEmpty())
        {
                // Si le bouton clignote deja, on quitte
                if (boutonTchatClignote)
                        return;
                // Sinon on indique qu'il clignote
                boutonTchatClignote = true;
                // On change la couleur du bouton
                QPalette palette(boutonTchatCommun->palette());
                palette.setColor(QPalette::Normal, QPalette::Button, QColor(255,200,0));
                boutonTchatCommun->setPalette(palette);
        }

        // L'ID n'est pas vide : il s'agit d'un joueur normal
        else
        {
                // Recherche du joueur dans la liste, en se basant sur l'idJoueur
                QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
                // Si la liste n'a pas un seul element, alors il y a un probleme
                if (listeItem.size() == 0)
                {
                        qWarning("Le joueur recherche n'existe pas (faireClignoter - ListeUtilisateurs.cpp)");
                        return;
                }
                else if (listeItem.size() > 1)
                {
                        qWarning("Plusieurs joueurs possedent l'identifiant recherche (faireClignoter - ListeUtilisateurs.cpp)");
                        return;
                }

                // Si le joueur clignote deja, on quitte
                if (listeItem[0]->text(5) == "Clignotant")
                        return;

                // Sinon on indique qu'il clignote
                listeItem[0]->setText(5, "Clignotant");
                // On change la couleur du fond
                listeItem[0]->setBackground(0, QColor(255,200,0));
        }

        // On ajoute l'utilisateur dans la liste des joueurs a faire clignoter
        listeClignotants.append(idJoueur);
        // On arme un timer pour faire clignoter le fond
        QTimer::singleShot(300, this, SLOT(clignote()));
}

/********************************************************************/
/* Verifie si l'utilisateur dont l'ID est passe en parametre est en */
/* train de clignoter, et si oui, demande l'arret du clignotement   */
/********************************************************************/
void ListeUtilisateurs::nePlusFaireClignoter(QString idJoueur)
{
        // Si l'ID est vide, il s'agit du tchat commun
        if (idJoueur.isEmpty())
        {
                // Si le bouton ne clignote pas, on quitte
                if (!boutonTchatClignote)
                        return;
                // Sinon on demande l'arret du clignotement
                boutonTchatClignote = false;
                // On remet sa couleur d'origine
                QPalette palette(boutonTchatCommun->palette());
                palette.setColor(QPalette::Normal, QPalette::Button, QColor(240,240,255));
                boutonTchatCommun->setPalette(palette);
        }

        // Sinon il s'agit d'un joueur normal
        else
        {
                // Recherche du joueur dans la liste, en se basant sur l'idJoueur
                QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
                // Si la liste n'a pas un seul element, alors il y a un probleme
                if (listeItem.size() == 0)
                {
                        qWarning("Le joueur recherche n'existe pas (nePlusFaireClignoter - ListeUtilisateurs.cpp)");
                        return;
                }
                else if (listeItem.size() > 1)
                {
                        qWarning("Plusieurs joueurs possedent l'identifiant recherche (nePlusFaireClignoter - ListeUtilisateurs.cpp)");
                        return;
                }

                // Si le joueur ne clignote pas, on quitte
                if (listeItem[0]->text(5) == "Fixe")
                        return;
                // Sinon on indique qu'il ne clignote plus
                listeItem[0]->setText(5, "Fixe");
                // On lui rend sa couleur l'origine
                listeItem[0]->setBackground(0, listeItem[0]->text(4) == "Joueur" ? Qt::white : QColor(200, 255, 200));
        }
}

/********************************************************************/
/* Change la couleur de fond du 1er utilisateur contenu dans la     */
/* liste des clignotants                                            */
/********************************************************************/
void ListeUtilisateurs::clignote()
{
        // Ne devrait jamais arriver
        if (!listeClignotants.size())
        {
                qWarning("Demande de clignotement d'un joueur alors que la liste est vide (clignote - ListeUtilisateurs.cpp)");
                return;
        }
        // On recupere le 1er element de la liste des clignotants
        QString idJoueur = listeClignotants.takeFirst();

        // Si l'ID est vide, il s'agit du tchat commun
        if (idJoueur.isEmpty())
        {
                // Si le bouton ne doit plus clignoter, on quitte la fonction sans rearmer le timer
                if (!boutonTchatClignote)
                        return;

                // Sinon on change sa couleur
                QPalette palette(boutonTchatCommun->palette());
                if (palette.color(QPalette::Normal, QPalette::Button) == QColor(255,200,0))
                        palette.setColor(QPalette::Normal, QPalette::Button, QColor(240,240,255));
                else
                        palette.setColor(QPalette::Normal, QPalette::Button, QColor(255,200,0));
                boutonTchatCommun->setPalette(palette);
        }

        // L'ID n'est pas vide, il s'agit d'un joueur normal
        else
        {
                // Recherche du joueur dans la liste, en se basant sur l'idJoueur
                QList<QTreeWidgetItem *> listeItem = treeWidget->findItems(idJoueur, Qt::MatchExactly, 1);
                // Si la liste n'a pas un seul element, alors il y a un probleme
                if (listeItem.size() == 0)
                {
                        qWarning("Le joueur recherche n'existe pas (clignote - ListeUtilisateurs.cpp)");
                        return;
                }
                else if (listeItem.size() > 1)
                {
                        qWarning("Plusieurs joueurs possedent l'identifiant recherche (clignote - ListeUtilisateurs.cpp)");
                        return;
                }

                // Si le joueur ne doit plus clignoter, on quitte la fonction sans rearmer le timer
                if (listeItem[0]->text(5) == "Fixe")
                        return;

                // Sinon on change la couleur de fond du joueur
                if (listeItem[0]->background(0) == QColor(255,200,0))
                        listeItem[0]->setBackground(0, listeItem[0]->text(4) == "Joueur" ? Qt::white : QColor(200, 255, 200));
                else
                        listeItem[0]->setBackground(0, QColor(255,200,0));
        }

        // On remet l'ID du joueur en fin de liste
        listeClignotants.append(idJoueur);
        // On rearme le timer pour faire clignoter le fond
        QTimer::singleShot(300, this, SLOT(clignote()));
}

/********************************************************************/
/* Renvoie true si l'utilisateur est un MJ, false s'il est un       */
/* joueur ou s'il n'a pas ete trouve                                */
/********************************************************************/
bool ListeUtilisateurs::estUnMj(QString idJoueur)
{
        // On recupere le numero d'index du joueur
        int index = numeroUtilisateur(idJoueur);

        // Si le joueur n'a pas ete trouve on renvoie false
        if (index == -1)
                return false;

        // Sinon on verifie s'il s'agit d'un MJ ou pas
        else
                return estUnMj(index);
}

/********************************************************************/
/* Renvoie true si l'utilisateur est un MJ, false s'il est un       */
/* joueur ou s'il n'a pas ete trouve                                */
/********************************************************************/
bool ListeUtilisateurs::estUnMj(int index)
{
        // Recuperation du joueur
        QTreeWidgetItem *joueur = treeWidget->topLevelItem(index);

        // On renvoie true si l'utilisateur est MJ
        if (joueur)
                return joueur->text(4) == "MJ";
        else
                return false;
}

/********************************************************************/
/* Renvoie true si un MJ est present dans la liste, false sinon     */
/********************************************************************/
bool ListeUtilisateurs::mjDansLaListe()
{
        int j;
        QTreeWidgetItem *joueur;
        bool trouve = false;

        // Recuperation du nbr de joueurs
        int nbrJoueurs = treeWidget->topLevelItemCount();

        // Parcours des joueurs
        for (j=0; j<nbrJoueurs && !trouve; j++)
        {
                // Recuperation du joueur
                joueur = treeWidget->topLevelItem(j);
                // On regarde si le joueur est MJ
                if (joueur->text(4) == "MJ")
                        trouve = true;
        }

        // On renvoie le resultat de la recherche
        return trouve;
}


