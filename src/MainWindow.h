/*
Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
Copyright (C) 2007 - Romain Campioni  Tous droits réservés.

Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
modifier suivant les termes de la GNU General Public License telle que
publiée par la Free Software Foundation : soit la version 2 de cette
licence, soit (à votre gré) toute version ultérieure.

Ce programme est distribué dans lespoir quil vous sera utile, mais SANS
AUCUNE GARANTIE : sans même la garantie implicite de COMMERCIALISABILITÉ
ni dADÉQUATION À UN OBJECTIF PARTICULIER. Consultez la Licence Générale
Publique GNU pour plus de détails.

Vous devriez avoir reçu une copie de la Licence Générale Publique GNU avec
ce programme ; si ce nest pas le cas, consultez :
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

Par ailleurs ce logiciel est gratuit et ne peut en aucun cas être
commercialisé, conformément à la "FMOD Non-Commercial License".
*/


/********************************************************************/
/*                                                                  */
/* Fenetre principale, contenant la palette d'outils, les tableaux  */
/* de joueurs et de PJ, les cartes et les fenetres de dialogue.     */
/*                                                                  */
/********************************************************************/


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QWorkspace>

#include "CarteFenetre.h"
#include "Carte.h"
#include "BarreOutils.h"
#include "ListeUtilisateurs.h"
#include "NouveauPlanVide.h"
#include "Tchat.h"
#include "Image.h"
#include "EditeurNotes.h"
#include "WorkspaceAmeliore.h"
#include "LecteurAudio.h"

class MainWindow : public QMainWindow
{
Q_OBJECT

public :
        MainWindow();
        void affichageDuPj(QString idPerso, bool afficher);
        void majCouleursPersonnelles();
        void ajouterCarte(CarteFenetre *carteFenetre, QString titre);
        void ajouterImage(Image *imageFenetre, QString titre);
        void emettreTousLesPlans(QString idJoueur);
        void emettreToutesLesImages(QString idJoueur);
        void mettreAJourEspaceTravail();
        void mettreAJourSelecteurTaille(QString idCarte, int taillePj);
        void emettreChangementTaillePj(int nouvelleTaille);
        void ajouterTchat(QString idJoueur, QString nomJoueur);
        void supprimerTchat(QString idJoueur);
        void devientFenetreActive(QWidget *widget);
        void cocherActionTchatCommun();
        void changementNatureUtilisateur();
        Carte *trouverCarte(QString idCarte);
        CarteFenetre *trouverCarteFenetre(QString idCarte);
        Image *trouverImage(QString idImage);
        Tchat *trouverTchat(QString idJoueur);
        bool estLaFenetreActive(QWidget *widget);
        bool enleverCarteDeLaListe(QString idCarte);
        bool enleverImageDeLaListe(QString idImage);

public slots :
        void changerTaillePj(int nouvelleTaille);
        void creerNouveauPlanVide(QString titre, QString idCarte, QColor couleurFond, quint16 largeur, quint16 hauteur, quint8 taillePj);
        void aucunNouveauPlanVide();
        void afficherTchat(QString id);
        void masquerTchat(QString id);
        void afficherEditeurNotes(bool afficher, bool cocherAction = false);
        void quitterApplication(bool perteConnexion = false);

private :
        QDockWidget* creerLogUtilisateur();
        void creerMenu();
        void associerActionsMenus();
        void autoriserOuInterdireActions();
        void lireCarteEtPnj(QFile &file, bool masquer = false, QString nomFichier = "");
        void lireImage(QFile &file);
        void sauvegarderTousLesPlans(QFile &file);
        void sauvegarderToutesLesImages(QFile &file);
        void sauvegarderTousLesTchats();
        void sauvegarderFichierInitialisation();

        QDockWidget *dockLogUtil;
        WorkspaceAmeliore *workspace;
        QMenu *menuFenetre;
        QMenu *sousMenuTchat;
        BarreOutils *barreOutils;
        NouveauPlanVide *fenetreNouveauPlan;
        EditeurNotes *editeurNotes;
        QList <CarteFenetre *> listeCarteFenetre;
        QList <Image *> listeImage;
        QList <Tchat *> listeTchat;
        LecteurAudio* G_lecteurAudio;
        QAction *actionNouveauPlan;
        QAction *actionOuvrirImage;
        QAction *actionOuvrirPlan;
        QAction *actionOuvrirEtMasquerPlan;
        QAction *actionOuvrirScenario;
        QAction *actionOuvrirNotes;
        QAction *actionFermerPlan;
        QAction *actionSauvegarderPlan;
        QAction *actionSauvegarderScenario;
        QAction *actionSauvegarderNotes;
        QAction *actionPreferences;
        QAction *actionQuitter;

        QAction *actionAfficherNomsPj;
        QAction *actionAfficherNomsPnj;
        QAction *actionAfficherNumerosPnj;
        QAction *actionSansGrille;
        QAction *actionCarre;
        QAction *actionHexagones;

        QAction *actionCascade;
        QAction *actionTuiles;
        QAction *actionEditeurNotes;
        QAction *actionTchatCommun;

        QAction *actionAideLogiciel;
        QAction *actionAPropos;

private slots :
        void afficherNomsPj(bool afficher);
        void afficherNomsPnj(bool afficher);
        void afficherNumerosPnj(bool afficher);
        void changementFenetreActive(QWidget *widget);
        void nouveauPlan();
        void ouvrirPlan(bool masquer = false);
        void ouvrirImage();
        void ouvrirEtMasquerPlan();
        void ouvrirScenario();
        void ouvrirNotes();
        void fermerPlanOuImage();
        void sauvegarderPlan();
        void aPropos();


        /// @brief open the Qt assistant with the rolisteam documentation
        void aideEnLigne();
        bool sauvegarderScenario();
        bool sauvegarderNotes();

protected :
        void closeEvent(QCloseEvent *event);

};

#endif
