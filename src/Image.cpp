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
#include <QDebug>
#include <QScrollBar>
#include <QShortcut>

#include "Image.h"

#include "Liaison.h"
#include "mainwindow.h"
#include "network/networkmessagewriter.h"

#include "variablesGlobales.h"

/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
Image::Image(MainWindow* mainWindow,QString identImage, QString identJoueur, QImage *image, QAction *action, WorkspaceAmeliore *parent)
: QScrollArea(parent),m_mainWindow(mainWindow),m_NormalSize(0,0)
{

	setObjectName("Image");
    m_zoomLevel = 1;
    m_parent=parent;
    setWindowIcon(QIcon(":/resources/icones/image.png"));

    createActions();

    m_prefManager = PreferencesManager::getInstance();

	labelImage = new QLabel(this);
    m_pixMap = QPixmap::fromImage(*image);
    labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
	labelImage->resize(image->width(), image->height());
    labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    labelImage->setScaledContents(true);
    labelImage->resize(m_pixMap.size());

    m_internalAction = action;
    idImage = identImage;
	idJoueur = identJoueur;
    setAlignment(Qt::AlignCenter);
    setWidget(labelImage);

    m_ratioImage = (double)m_pixMap.size().width()/m_pixMap.size().height();
    m_ratioImageBis = (double)m_pixMap.size().height()/m_pixMap.size().width();
    //labelImage->installEventFilter(this);
    if(NULL!=m_parent)
    {
        fitWorkSpace();
    }

    m_fitWindowAct->setChecked(m_prefManager->value("PictureAdjust",true).toBool());
    fitWindow();

    //resize(image->width()+2, image->height()+2);
}

Image::~Image()
{
    /// @todo fix this, cause crash at the destruction of the application.
    //m_mainWindow->enleverImageDeLaListe(idImage);
}
QAction* Image::getAssociatedAction() const
{
    return m_internalAction;
}
void Image::setInternalAction(QAction *action)
{
    m_internalAction = action;
}
bool Image::proprietaireImage()
{
	return idJoueur == G_idJoueurLocal;
}


QString Image::getImageId()
{
	return idImage;
}
QString Image::getImageTitle()
{
    return m_title;
}
void Image::setImageTitle(QString title)
{
    m_title=title;
}

void Image::fill(NetworkMessageWriter & message) const
{
	QByteArray baImage;
	QBuffer bufImage(&baImage);
    if (!labelImage->pixmap()->save(&bufImage, "jpg", 70))
    {
        qDebug() << "png size:" << bufImage.size();
    }



    message.reset();
    message.string16(m_title);
    message.string8(idImage);
    message.string8(idJoueur);
    message.byteArray32(baImage);
}


void Image::sauvegarderImage(QDataStream &out, QString titre)
{
    bool ok;

    // compression is done in PNG allowing transparency.
    QByteArray baImage;
    QBuffer bufImage(&baImage);
    ok = labelImage->pixmap()->save(&bufImage, "jpg", 70);
    if (!ok)
        qWarning() <<(tr("Image Compression fails (sauvegarderImage - Image.cpp)"));

    // Ecriture de l'image dans le fichier
    out<< titre;
    out << pos();
    out << size();
    out << baImage;
}

void Image::mousePressEvent(QMouseEvent *event)
{
	// Si l'utilisateur a clique avec la bouton gauche et que l'outil main est selectionne
    if (event->button() == Qt::LeftButton && m_currentTool == ToolBar::main)
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


void Image::mouseReleaseEvent(QMouseEvent *event)
{
	// Si le bouton gauche est relache on interdit le deplacement de la carte
	if (event->button() == Qt::LeftButton)
		deplacementAutorise = false;
}


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
    if(m_zoomLevel<0.2)
    {
        m_zoomLevel=0.2;
    }
    else if(m_zoomLevel>3.0)
    {
         m_zoomLevel=3.0;
    }


    if(m_fitWindowAct->isChecked())
    {
        if(width()>height()*m_ratioImage)
        {
            labelImage->resize(height()*m_ratioImage,height());
        }
        else
        {
            labelImage->resize(width(),width()*m_ratioImageBis);
        }
    }
    else if((m_NormalSize.height()!=0)&&(m_NormalSize.width()!=0))
    {
        labelImage->resize(m_zoomLevel * m_NormalSize);
    }
    else
    {
        labelImage->resize(m_zoomLevel * m_pixMap.size());
        m_NormalSize = widget()->size();
    }
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

void Image::onFitWorkSpace()
{
    labelImage->resize(m_parent->size());
    fitWorkSpace();
    //m_zoomLevel = 1;
}

void Image::fitWorkSpace()
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
void Image::hideEvent(QHideEvent* event)
{
    if(NULL!=m_internalAction)
    {
        m_internalAction->setChecked(false);
    }

}
void Image::showEvent(QShowEvent* event)
{
    if(NULL!=m_internalAction)
    {
        m_internalAction->setChecked(true);
    }
}

void Image::paintEvent ( QPaintEvent * event )
{
    QScrollArea::paintEvent(event);
    if(m_fitWindowAct->isChecked())
    {

    }
    else if((m_NormalSize * m_zoomLevel) != labelImage->size())
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
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    m_actionZoomIn->setIcon(QIcon(":/resources/icons/zoom-in-32.png"));
    connect(m_actionZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    m_zoomInShort = new QShortcut(QKeySequence(tr("Ctrl++", "Zoom In")), this);
    m_zoomInShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomInShort, SIGNAL(activated ()), this, SLOT(zoomIn()));
    m_actionZoomIn->setShortcut(m_zoomInShort->key());

    m_actionZoomOut = new QAction(tr("Zoom out"),this);
    m_actionZoomOut->setIcon(QIcon(":/resources/icons/zoom-out-32.png"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    m_zoomOutShort = new QShortcut(QKeySequence(tr("Ctrl+-", "Zoom Out")), this);
    m_zoomOutShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomOutShort, SIGNAL(activated ()), this, SLOT(zoomOut()));
    m_actionZoomOut->setShortcut(m_zoomOutShort->key());



    m_actionfitWorkspace = new QAction(tr("Fit the workspace"),this);
    m_actionfitWorkspace->setIcon(QIcon(":/fit-page.png"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace,SIGNAL(triggered()),this,SLOT(onFitWorkSpace()));

    m_fitShort = new QShortcut(QKeySequence(tr("Ctrl+m", "Fit the workspace")), this);
    m_fitShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitShort, SIGNAL(activated ()), this, SLOT(onFitWorkSpace()));
    m_actionfitWorkspace->setShortcut(m_fitShort->key());


    m_fitWindowAct =  new QAction(tr("Fit Window"),this);
    m_fitWindowAct->setCheckable(true);
    m_fitWindowAct->setIcon(QIcon(":/fit-window.png"));
    m_fitWindowAct->setToolTip(tr("Image will take the best dimension to fit the window."));
    connect(m_fitWindowAct, SIGNAL(triggered()), this, SLOT(fitWindow()));

    m_fitWindowShort  = new QShortcut(QKeySequence(tr("Ctrl+f", "Fit the window")), this);
    m_fitWindowShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitWindowShort, SIGNAL(activated ()), this, SLOT(fitWindow()));
    m_fitWindowAct->setShortcut(m_fitWindowShort->key());



    m_actionlittleZoom = new QAction(tr("Little"),this);
    m_actionlittleZoom->setToolTip(tr("Set the zoom level at 20% "));
    connect(m_actionlittleZoom,SIGNAL(triggered()),this,SLOT(zoomLittle()));

    m_littleShort = new QShortcut(QKeySequence(tr("Ctrl+l", "Set the zoom level at 20%")), this);
    m_littleShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_littleShort, SIGNAL(activated ()), this, SLOT(zoomLittle()));
    m_actionlittleZoom->setShortcut(m_littleShort->key());

    m_actionNormalZoom = new QAction(tr("Normal"),this);
    m_actionNormalZoom->setToolTip(tr("No Zoom"));
    connect(m_actionNormalZoom,SIGNAL(triggered()),this,SLOT(zoomNormal()));

    m_normalShort = new QShortcut(QKeySequence(tr("Ctrl+n", "Normal")), this);
    m_normalShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_normalShort, SIGNAL(activated ()), this, SLOT(zoomNormal()));
    m_actionNormalZoom->setShortcut(m_normalShort->key());


    m_actionBigZoom = new QAction(tr("Big"),this);
    m_actionBigZoom->setToolTip(tr("Set the zoom level at 400%"));
    connect(m_actionBigZoom,SIGNAL(triggered()),this,SLOT(zoomBig()));

    m_bigShort = new QShortcut(QKeySequence(tr("Ctrl+b", "Zoom Out")), this);
    m_bigShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_bigShort, SIGNAL(activated ()), this, SLOT(zoomBig()));
    m_actionBigZoom->setShortcut(m_bigShort->key());




}
void Image::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(m_actionZoomIn);
    menu.addAction(m_actionZoomOut);
    menu.addSeparator();
    menu.addAction( m_actionfitWorkspace);
    menu.addAction( m_fitWindowAct);
    menu.addSeparator();
    menu.addAction(m_actionlittleZoom);
    menu.addAction(m_actionNormalZoom);
    menu.addAction(m_actionBigZoom);


    menu.exec(event->globalPos ()/*event->pos()*/);

}
void Image::fitWindow()
{
    QObject* senderObj = sender();
    if(senderObj == m_fitWindowShort)
    {
       m_fitWindowAct->setChecked(! m_fitWindowAct->isChecked() );
    }

    resizeLabel();
    if(m_fitWindowAct->isChecked())
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_actionZoomIn->setEnabled(false);
        m_actionZoomOut->setEnabled(false);
        m_actionlittleZoom->setEnabled(false);
        m_actionNormalZoom->setEnabled(false);
        m_actionBigZoom->setEnabled(false);
    }
    else
    {
         setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
         setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
         m_actionZoomIn->setEnabled(true);
         m_actionZoomOut->setEnabled(true);
         m_actionlittleZoom->setEnabled(true);
         m_actionNormalZoom->setEnabled(true);
         m_actionBigZoom->setEnabled(true);
    }

    update();
}
void Image::resizeEvent(QResizeEvent *event)
{
    if(m_fitWindowAct->isChecked())
    {
     resizeLabel();
    }

    QScrollArea::resizeEvent(event);

}

void Image::setParent(WorkspaceAmeliore *parent)
{
    m_parent=parent;
    QScrollArea::setParent(parent);
    if(m_parent)
        fitWorkSpace();
}

void Image::setCurrentTool(ToolBar::Tool tool)
{
    m_currentTool = tool;

    /// @todo code inline to remove useless functions.
    switch(m_currentTool)
    {
    case ToolBar::main :
            pointeurMain();
            break;
    default :
            pointeurNormal();
            break;
    }
}
