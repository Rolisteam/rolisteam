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

#include "Image.h"
#include "variablesGlobales.h"
#include <QDebug>

/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
Image::Image(QString identImage, QString identJoueur, QImage *image, QAction *action, WorkspaceAmeliore *parent)
: QScrollArea(parent),m_NormalSize(0,0)
{

	setObjectName("Image");
    m_zoomLevel = 1;
    m_parent=parent;
    setWindowIcon(QIcon(":/resources/icones/image.png"));
    createActions();

	labelImage = new QLabel(this);
    m_pixMap = QPixmap::fromImage(*image);
    labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
	labelImage->resize(image->width(), image->height());
    labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    labelImage->setScaledContents(true);
    labelImage->resize(m_pixMap.size());

    actionAssociee = action;
    idImage = identImage;
	idJoueur = identJoueur;
    setAlignment(Qt::AlignCenter);
    setWidget(labelImage);
    //labelImage->installEventFilter(this);
    if(m_parent)
        fitWindow();
    //resize(image->width()+2, image->height()+2);
}


/********************************************************************/	
/* Destructeur                                                      */
/********************************************************************/	
Image::~Image()
{
	// Destruction de l'action associee
	actionAssociee->~QAction();
	// On enleve l'image de la liste des Images existantes
	G_mainWindow->enleverImageDeLaListe(idImage);
}

/********************************************************************/
/* Cache la fenetre au lieu de la detruire                          */
/********************************************************************/	
void Image::closeEvent(QCloseEvent *event)
{
	// Masquage de la fenetre
	hide();
	// Deselection de l'action associee
	actionAssociee->setChecked(false);
	// Arret de la procedure de fermeture		
	event->ignore();
}

/********************************************************************/
/* Associe l'action d'affichage/masquage a la carte                 */
/********************************************************************/	
void Image::associerAction(QAction *action)
{
	actionAssociee = action;
}

/********************************************************************/
/* Renvoie true si l'utilisateur local est le proprietaire de       */
/* l'image                                                          */
/********************************************************************/
bool Image::proprietaireImage()
{
	return idJoueur == G_idJoueurLocal;
}

/********************************************************************/
/* Renvoie l'identifiant de l'image                                 */
/********************************************************************/
QString Image::identifiantImage()
{
	return idImage;
}

/********************************************************************/
/* Emet l'Image vers la liaison passee en parametre                 */
/********************************************************************/
void Image::emettreImage(QString titre, int numeroLiaison)
{
	bool ok;

	// On compresse l'image dans un tableau
	QByteArray baImage;
	QBuffer bufImage(&baImage);
	ok = labelImage->pixmap()->save(&bufImage, "jpeg", 70);
	if (!ok)
		qWarning("Probleme de compression de l'image (emettreImage - Image.cpp)");
	
	// Taille des donnees
	quint32 tailleCorps =
		// Taille du titre
		sizeof(quint16) + titre.size()*sizeof(QChar) +
		// Taille de l'identifiant de l'image
		sizeof(quint8) + idImage.size()*sizeof(QChar) +
		// Taille de l'identifiant du joueur
		sizeof(quint8) + idJoueur.size()*sizeof(QChar) +
		// Taille de l'image
		sizeof(quint32) + baImage.size();
			
	// Buffer d'emission
	char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

	// Creation de l'entete du message
	enteteMessage *uneEntete;
	uneEntete = (enteteMessage *) donnees;
	uneEntete->categorie = image;
	uneEntete->action = chargerImage;
	uneEntete->tailleDonnees = tailleCorps;
	
	// Creation du corps du message
	int p = sizeof(enteteMessage);
	// Ajout du titre
	quint16 tailleTitre = titre.size();
	memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
	p+=sizeof(quint16);
	memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
	p+=tailleTitre*sizeof(QChar);
	// Ajout de l'identifiant de l'image
	quint8 tailleIdImage = idImage.size();
	memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
	p+=sizeof(quint8);
	memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
	p+=tailleIdImage*sizeof(QChar);
	// Ajout de l'identifiant du joueur
	quint8 tailleIdJoueur = idJoueur.size();
	memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
	p+=sizeof(quint8);
	memcpy(&(donnees[p]), idJoueur.data(), tailleIdJoueur*sizeof(QChar));
	p+=tailleIdJoueur*sizeof(QChar);
	// Ajout de l'image
	quint32 tailleImage = baImage.size();
	memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
	p+=sizeof(quint32);
	memcpy(&(donnees[p]), baImage.data(), tailleImage);
	p+=tailleImage;

	// Emission de l'image vers la liaison indiquee
	emettre(donnees, tailleCorps + sizeof(enteteMessage), numeroLiaison);
	// Liberation du buffer d'emission
	delete[] donnees;
}

/********************************************************************/
/* Sauvegarde l'image dans le fichier passe en parametre            */
/********************************************************************/
void Image::sauvegarderImage(QDataStream &out, QString titre)
{
    bool ok;

    // On commence par compresser l'image (format jpeg) dans un tableau
    QByteArray baImage;
    QBuffer bufImage(&baImage);
    ok = labelImage->pixmap()->save(&bufImage, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression de l'image (sauvegarderImage - Image.cpp)");

    // Ecriture de l'image dans le fichier
    // Ecriture du titre
    out<< titre;




    out << pos();
    out << size();

    out << baImage;


}
/********************************************************************/
/* Un bouton de la souris vient d'etre enfonce                      */
/********************************************************************/	
void Image::mousePressEvent(QMouseEvent *event)
{
	// Si l'utilisateur a clique avec la bouton gauche et que l'outil main est selectionne
	if (event->button() == Qt::LeftButton && G_outilCourant == BarreOutils::main)
	{
		// Le deplacement est autorise
		deplacementAutorise = true;
		// On memorise la position du point de depart
		pointDepart = event->pos();
		// On releve les valeurs des barres de defilement
		horizontalDepart = horizontalScrollBar()->value();
		verticalDepart = verticalScrollBar()->value();
	}
}

/********************************************************************/
/* Relache d'un bouton de la souris                                 */
/********************************************************************/	
void Image::mouseReleaseEvent(QMouseEvent *event)
{
	// Si le bouton gauche est relache on interdit le deplacement de la carte
	if (event->button() == Qt::LeftButton)
		deplacementAutorise = false;
}

/********************************************************************/
/* Deplacement de la souris                                         */
/********************************************************************/	
void Image::mouseMoveEvent(QMouseEvent *event)
{
	// Si le deplacement est autorise
	if (deplacementAutorise)
	{
		// On calcule la cifference de position entre le depart et maintenant
		QPoint diff = pointDepart - event->pos();
		// On change la position des barres de defilement
		horizontalScrollBar()->setValue(horizontalDepart + diff.x());
		verticalScrollBar()->setValue(verticalDepart + diff.y());
	}
}
		
void Image::resizeLabel()
{
    //qDebug()<< " cdcec"<< m_pixMap.height() << m_pixMap.width() << labelImage->rect() << geometry();
    if((m_NormalSize.height()!=0)&&(m_NormalSize.width()!=0))
    {
        labelImage->resize(m_zoomLevel * m_NormalSize);
    }
    else
    {
        labelImage->resize(m_zoomLevel * m_pixMap.size());
        m_NormalSize = widget()->size();
    }
    //qDebug()<< m_pixMap.height() << m_pixMap.width() << labelImage->rect() << geometry();
}
void Image::pointeurMain()
{
	labelImage->setCursor(Qt::OpenHandCursor);
}
void Image::pointeurNormal()
{
	labelImage->setCursor(Qt::ForbiddenCursor);
}
void Image::zoomIn()
{
    m_zoomLevel +=0.2;
    resizeLabel();
}

void Image::zoomOut()
{
    m_zoomLevel -=0.2;
    resizeLabel();
}

void Image::onFitWindow()
{
    labelImage->resize(m_parent->size());
    fitWindow();
    //m_zoomLevel = 1;
}

void Image::fitWindow()
{
    QSize windowsize = m_parent->size();//right size
    while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))||(windowsize.width()<(m_zoomLevel * m_pixMap.width())))
    {
        m_zoomLevel -= 0.1;
    }
    labelImage->resize(m_pixMap.size());
    m_NormalSize = QSize(0,0);
    resizeLabel();
    setGeometry(labelImage->rect().adjusted(0,0,4,4));
    m_zoomLevel = 1.0;
}
void Image::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier)
    {
        int delta = event->delta();
        if(delta > 0)//zoomin
        {
            m_zoomLevel +=0.2;
        }
        else//zoomOut
        {
            m_zoomLevel -=0.2;
        }
        resizeLabel();
        event->ignore();

    }
}
void Image::paintEvent ( QPaintEvent * event )
{
    QScrollArea::paintEvent(event);
    if(widgetResizable())
    {

        setWidgetResizable(false);

    }
    if((m_NormalSize * m_zoomLevel) != labelImage->size())
    {
        m_NormalSize = labelImage->size() / m_zoomLevel;
        m_windowSize = size();
    }
}
void Image::setZoomLevel(double zoomlevel)
{

        m_zoomLevel = zoomlevel;
        resizeLabel();

}
void Image::zoomLittle()
{
    m_zoomLevel =0.2;
    resizeLabel();

}

void Image::zoomNormal()
{

    m_zoomLevel =1.0;
    resizeLabel();

}
void Image::zoomBig()
{
    m_zoomLevel =4.0;
    resizeLabel();
}
void Image::createActions()
{
    m_actionZoomIn = new QAction(tr("Zoom In"),this);
    m_actionZoomIn->setShortcut(tr("Ctrl++"));
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    m_actionZoomIn->setIcon(QIcon(":/resources/icons/zoom-in-32.png"));
    connect(m_actionZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    m_actionZoomOut = new QAction(tr("Zoom out"),this);
    m_actionZoomOut->setShortcut(tr("Ctrl+-"));
    m_actionZoomOut->setIcon(QIcon(":/resources/icons/zoom-out-32.png"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    m_actionfitWorkspace = new QAction(tr("Fit the workspace"),this);
    m_actionfitWorkspace->setIcon(QIcon(":/resources/icons/fit-page-32.png"));
    m_actionfitWorkspace->setShortcut(tr("Ctrl+5"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace,SIGNAL(triggered()),this,SLOT(onFitWindow()));

    m_actionlittleZoom = new QAction(tr("Little"),this);
    m_actionlittleZoom->setShortcut(tr("Ctrl+1"));
    m_actionZoomIn->setToolTip(tr("Set the zoom level at 20% "));
    connect(m_actionlittleZoom,SIGNAL(triggered()),this,SLOT(zoomLittle()));

    m_actionNormalZoom = new QAction(tr("Normal"),this);
    m_actionNormalZoom->setShortcut(tr("Ctrl+0"));
    m_actionZoomIn->setToolTip(tr("No Zoom"));
    connect(m_actionNormalZoom,SIGNAL(triggered()),this,SLOT(zoomNormal()));


    m_actionBigZoom = new QAction(tr("Big"),this);
    m_actionBigZoom->setShortcut(tr("Ctrl+2"));
    m_actionZoomIn->setToolTip(tr("Set the zoom level at 400%"));
    connect(m_actionBigZoom,SIGNAL(triggered()),this,SLOT(zoomBig()));
}
void Image::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(m_actionZoomIn);
    menu.addAction(m_actionZoomOut);
    menu.addSeparator();
    menu.addAction( m_actionfitWorkspace);
    menu.addSeparator();
    menu.addAction(m_actionlittleZoom);
    menu.addAction(m_actionNormalZoom);
    menu.addAction(m_actionBigZoom);


    menu.exec(event->globalPos ()/*event->pos()*/);

}
void Image::setParent(WorkspaceAmeliore *parent)
{
    m_parent=parent;
    QScrollArea::setParent(parent);
    if(m_parent)
        fitWindow();
}
