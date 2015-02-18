/*
	Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
	Copyright (C) 2007 - Romain Campioni  Tous droits rservs.

	Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
	modifier suivant les termes de la GNU General Public License telle que
	publie par la Free Software Foundation : soit la version 2 de cette
	licence, soit ( votre gr) toute version ultrieure.

	Ce programme est distribu dans lespoir quil vous sera utile, mais SANS
	AUCUNE GARANTIE : sans mme la garantie implicite de COMMERCIALISABILIT
	ni dADQUATION  UN OBJECTIF PARTICULIER. Consultez la Licence Gnrale
	Publique GNU pour plus de dtails.

	Vous devriez avoir reu une copie de la Licence Gnrale Publique GNU avec
	ce programme ; si ce nest pas le cas, consultez :
	<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

	Par ailleurs ce logiciel est gratuit et ne peut en aucun cas tre
	commercialis, conformment  la "FMOD Non-Commercial License".
*/


	#include <QtGui>

	#include "ClientServeur.h"
	#include "constantesGlobales.h"
	#include "types.h"


	// Inclusion de la librairie FMOD (librairie audio)
	#ifdef WIN32
		#define DLL_EXPORTS

                #include "fmod.h"
        #endif

	// Importation du plugin JPEG
//	Q_IMPORT_PLUGIN(qjpeg)


	// Fenetre de log (utilise seulement dans ce fichier)
	static QTextEdit *Log;


	/********************************************************************/
	/* Variables globales utilisees par tous les elements de            */
	/* l'application                                                    */
	/********************************************************************/
	// Contient tous les parametres extraits du fichier d'initialisation
	initialisation G_initialisation;


	/********************************************************************/
	/* handler d'affichage des messages d'erreur et de debug            */
	/********************************************************************/	
    void handlerAffichageMsg(QtMsgType type, const char *msg)
    {
		QString titre;
		Qt::GlobalColor couleur;
		
		// Parametrage du titre et de la couleur
        switch (type) {
        case QtDebugMsg:
            titre = "Debug : ";
            couleur = Qt::green;
            break;
        case QtWarningMsg:
            titre = "Warning : ";
            couleur = Qt::blue;
            break;
        case QtCriticalMsg:
            titre = "Critical : ";
            couleur = Qt::red;
            break;
        case QtFatalMsg:
            titre = "Fatal : ";
            couleur = Qt::darkGray;
            break;
        default :
			titre = "Type de message inconnu : ";
			couleur = Qt::magenta;
			break;
        }

		// Affichage du texte
		Log->setFontWeight(QFont::Bold);
        Log->setTextColor(couleur);
		Log->append(titre);
 		Log->setFontWeight(QFont::Normal);
    	Log->setTextColor(Qt::black);
		Log->insertPlainText(msg);
		
		// Erreur fatale : on quitte
		if (type == QtFatalMsg)
			abort();
    }

	/********************************************************************/
	/* main                                                             */
	/********************************************************************/	
    int main(int argc, char *argv[])
    {
		// Initialisation du generateur de nombre aleatoire
		srand(clock());
	
		// Creation de l'application
        QApplication app(argc, argv);
               QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

		// Creation de la fenetre de log
		Log = new QTextEdit();
		Log->setWindowTitle("Log");
		Log->setReadOnly(true);
	//	Log->show();

		// Installation du handler de debugage
		qInstallMsgHandler(handlerAffichageMsg);

		// Chargement du traducteur de Qt
		QTranslator qtTranslator;
		qtTranslator.load("qt_" + QLocale::system().name());
		app.installTranslator(&qtTranslator);

		// On charge le fichier de ressources qui contient ttes les images
		QResource::registerResource(QString(NOM_APPLICATION) + ".rcc");

		#ifdef WIN32
			// M.a.j de l'icone de l'application
                        app.setWindowIcon(QIcon(":/resources/icones/" + QString(NOM_APPLICATION) + ".png"));
		#endif

  		// Initialisation de la librairie FMOD
/*		if (!FSOUND_Init(FREQUENCE_AUDIO, 32, 0))
                        qWarning("Probleme d'initialisation de la librairie FMOD (main - main.cpp)");*/

		// Par defaut la variable d'initialisation n'est pas utilisable
		G_initialisation.initialisee = false;

		// Nom du fichier d'initialisation
		#ifdef WIN32
			QString fichierInitialisation = QString(NOM_APPLICATION) + ".ini";
		#elif defined (MACOS)
			// Creation du repertoire ou sont stockees les preferences, si celui-ci n'existe pas
			if (!(QDir(QDir::homePath() + "/." + QString(NOM_APPLICATION)).exists()))
				QDir::home().mkdir("." + QString(NOM_APPLICATION));
                        QString fichierInitialisation = QDir::homePath() + "/." + QString(NOM_APPLICATION) + "/" + QString(NOM_APPLICATION) + ".ini";
                #else
                        QString fichierInitialisation = qApp->applicationDirPath()+"/."+QString(NOM_APPLICATION);
		#endif
 
		// Si le fichier d'initialisation existe, on le charge
		if (QFile::exists(fichierInitialisation))
		{
			// Creation du descripteur de fichier
			QFile file(fichierInitialisation);
			// Ouverture du fichier en lecture seule
			if (!file.open(QIODevice::ReadOnly))
			{
				qWarning("Probleme a l'ouverture du fichier d'initialisation (main - main.cpp)");
			}
			
			// L'ouverture s'est correctement passee
			else
			{
				// On indique que la variable d'initilisation est utilisable
				G_initialisation.initialisee = true;
				// On cree un flux de donnees rattache au fichier
				QDataStream fluxFichier(&file);
				
				// On recupere la version de l'application
				fluxFichier >> G_initialisation.versionApplication;
				// ...le nom de l'utilisateur
				fluxFichier >> G_initialisation.nomUtilisateur;
				// ...la couleur de l'utilisateur
				fluxFichier >> G_initialisation.couleurUtilisateur;
				// ...la nature de l'utilisateur (joueur ou MJ)
				fluxFichier >> G_initialisation.joueur;
				// ...la nature de l'ordinateur local
				fluxFichier >> G_initialisation.client;
				// ...l'adresse IP du serveur
				fluxFichier >> G_initialisation.ipServeur;
				// ...le port du serveur
				fluxFichier >> G_initialisation.portServeur;
				// ...le port de connexion pour les clients
				fluxFichier >> G_initialisation.portClient;
				// ...le chemin pour les musiques
				fluxFichier >> G_initialisation.dossierMusiquesMj;
				// ...le chemin pour les musiques des joueurs
				fluxFichier >> G_initialisation.dossierMusiquesJoueur;
				// ...le chemin pour les images
				fluxFichier >> G_initialisation.dossierImages;
				// ...le chemin pour les plans
				fluxFichier >> G_initialisation.dossierPlans;
				// ...le chemin pour les scenarii
				fluxFichier >> G_initialisation.dossierScenarii;
				// ...le chemin pour les notes
				fluxFichier >> G_initialisation.dossierNotes;
				// ...le chemin pour les tchats
				fluxFichier >> G_initialisation.dossierTchats;
				// ...les couleurs personnelles
				for (int i=0; i<16; i++)
					fluxFichier >> G_initialisation.couleurPersonnelle[i];
				// ...le volume du lecteur audio
				fluxFichier >> G_initialisation.niveauVolume;
				// Fermeture du fichier
				file.close();
			}
		}

		// Creation du client/serveur : si la connexion reussie alors
		// le programme principal est lance
		ClientServeur *clientServeur = new ClientServeur();
		
		// Lancement de la boucle d'application
        return app.exec();
    } 
