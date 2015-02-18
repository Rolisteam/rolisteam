/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include <QButtonGroup>

#include "colorselector.h"


#include "preferencesmanager.h"

ColorLabel::ColorLabel(QWidget * parent)
    : QWidget(parent)
{}
void ColorLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit clickedColor(this->palette().color(QPalette::Window));
}
void ColorLabel::mouseDoubleClickEvent (QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubledclicked();
}

ColorButton::ColorButton(QPixmap* p,QWidget * parent )
    : QPushButton(parent),m_background(p)
{

    setCheckable(true);
}

void ColorButton::paintEvent( QPaintEvent * event )
{
    QPainter painter(this);
    QRect r=rect();
    r.setBottom(r.bottom()-2);
    r.setTop(r.top()+2);
    r.setLeft(r.left()+2);
    r.setRight(r.right()-2);
    painter.drawPixmap(r,*m_background);
   /* if(isChecked())
        painter.drawRect(rect());*/
    QPushButton::paintEvent(event);
}


ColorSelector::ColorSelector(QWidget *parent)
	: QWidget(parent)
{
    m_options = PreferencesManager::getInstance();

	// Initialisation des couleurs de base
	int rouge[48] = {255,255,128,0,128,0,255,255, 255,255,128,0,0,0,128,255, 128,255,0,0,0,128,128,255, 128,255,0,0,0,0,128,128, 64,128,0,0,0,0,64,64, 0,128,128,128,64,192,64,255};
	int vert[48] = {128,255,255,255,255,128,128,128, 0,255,255,255,255,128,128,0, 64,128,255,128,64,128,0,0, 0,128,128,128,0,0,0,0, 0,64,64,64,0,0,0,0, 0,128,128,128,128,192,64,255};
	int bleu[48] = {128,128,128,128,255,255,192,255, 0,0,0,64,255,192,192,255, 64,64,0,128,128,255,64,128, 0,0,0,64,255,160,128,255, 0,0,0,64,128,64,64,128, 0,0,64,128,128,192,64,255};

	// Creation du layout principale
	QVBoxLayout *selecteurLayout = new QVBoxLayout(this);
	selecteurLayout->setMargin(2);
	selecteurLayout->setSpacing(1);


    m_currentColorLabel = new ColorLabel(this);
    m_currentColorLabel->setFixedSize(45,40);
    m_currentColorLabel->setPalette(QPalette(QColor(rouge[0],vert[0],bleu[0])));
    m_currentColorLabel->setToolTip(tr("Predefine Color 1"));
    m_currentColorLabel->setAutoFillBackground(true);
    connect(m_currentColorLabel,SIGNAL(doubledclicked()),this,SLOT(colorSelectorDialog()));

    m_currentColor = QColor(rouge[0],vert[0],bleu[0]);
    selecteurLayout->addWidget(m_currentColorLabel);
    selecteurLayout->setAlignment(m_currentColorLabel, Qt::AlignHCenter | Qt::AlignTop);
			
	// Création du layout de la grille de couleurs predefinies
	QGridLayout *grillePredef = new QGridLayout();
	grillePredef->setSpacing(1);
	grillePredef->setMargin(1);
	selecteurLayout->addLayout(grillePredef);

	// Creation des widgets pour les couleurs predefinies
	int i=0, x=0, y=0;
	for (; i<48; i++)
	{
		// Initialisation de la couleur
		QColor couleur(rouge[i],vert[i],bleu[i]);
		
		// Creation d'un widget de couleur unie
        couleurPredefinie[i] = new ColorLabel(this);
		couleurPredefinie[i]->setPalette(QPalette(couleur));
		couleurPredefinie[i]->setAutoFillBackground(true);
		couleurPredefinie[i]->setFixedHeight(5);
        couleurPredefinie[i]->setToolTip(tr("Predefined color #%1").arg(i+1));
        connect(couleurPredefinie[i],SIGNAL(clickedColor(QColor)),this,SLOT(selectColor(QColor)));

		QColorDialog::setStandardColor(x*6+y, couleur.rgb());
		
		// Ajout du widget au layout
		grillePredef->addWidget(couleurPredefinie[i], y, x);

		x++;
		y = x>=8?y+1:y;
		x = x>=8?0:x;
	}

	// Ajout d'un separateur entre les couleurs predefinies et les couleurs personnelles
	separateur1 = new QWidget(this);
	separateur1->setMaximumHeight(2);
	selecteurLayout->addWidget(separateur1);

	// Création du layout de la grille de couleurs personnelles
	QGridLayout *grillePerso = new QGridLayout();
	grillePerso->setSpacing(1);
	grillePerso->setMargin(1);
	// Ajout de la grille de couleurs personnelles dans le layout principal
	selecteurLayout->addLayout(grillePerso);

	// Creation des widgets pour les couleurs personnelles
	for (i=0, x=0, y=7; i<16; i++)
	{
		// Creation d'un widget de couleur blanche
        couleurPersonnelle[i] = new ColorLabel(this);
		couleurPersonnelle[i]->setAutoFillBackground(true);
		couleurPersonnelle[i]->setFixedHeight(5);
		couleurPersonnelle[i]->setToolTip(tr("Couleur personnelle ") + QString::number(i+1));

		// Mise a jour des couleurs personnelles de QColorDialog dans le cas ou la variable d'initialisation est utilisable
/*		if (G_initialisation.initialisee == true)
			QColorDialog::setCustomColor(i, G_initialisation.couleurPersonnelle[i].rgb());
		
		// Sinon on met une couleur par defaut
		else
            QColorDialog::setCustomColor(i, QColor(i*16, i*16, i*16).rgb());*/


		grillePerso->addWidget(couleurPersonnelle[i], y, x);

		x++;
		y = x>=8?y+1:y;
		x = x>=8?0:x;
	}

	// On met a jour les widgets des couleurs personnelles
    customColorUpdate();

	// Ajout d'un separateur entre les couleurs personnelles et les couleurs speciales
	separateur2 = new QWidget(this);
	separateur2->setMaximumHeight(3);
	selecteurLayout->addWidget(separateur2);

	// Création du layout des couleurs speciales
	QHBoxLayout *couleursSpeciales = new QHBoxLayout();
	couleursSpeciales->setSpacing(1);
	couleursSpeciales->setMargin(0);
	// Ajout du layout des couleurs specuales dans le layout principal
	selecteurLayout->addLayout(couleursSpeciales);

	// Ajout de la couleur speciale qui efface

    //m_eraseColor->setFrameStyle(QFrame::Box | QFrame::Raised);
    //m_eraseColor->setLineWidth(0);
    //m_eraseColor->setMidLineWidth(1);



    efface_pix = new QPixmap(":/resources/icones/efface.png");
    m_eraseColor = new ColorButton(efface_pix,this);
    m_eraseColor->setFixedHeight(15);
    //m_eraseColor->setPixmap(*efface_pix);
    //m_eraseColor->setScaledContents(true);
    m_eraseColor->setToolTip(tr("Effacer"));
    m_eraseColor->setPalette(QPalette(Qt::white));
    m_eraseColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_eraseColor);
	
	// Ajout de la couleur speciale qui masque

   // HideColor->setFrameStyle(QFrame::Box | QFrame::Raised);
    //HideColor->setLineWidth(0);
   // HideColor->setMidLineWidth(1);

    masque_pix = new QPixmap(":/resources/icones/masque.png");
    m_hideColor = new ColorButton(masque_pix,this);
        m_hideColor->setFixedHeight(15);
   // HideColor->setPixmap(*masque_pix);
   // HideColor->setScaledContents(true);
    m_hideColor->setPalette(QPalette(Qt::white));
    m_hideColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_hideColor);

	// Ajout de la couleur speciale qui demasque

  //  unveilColor->setFrameStyle(QFrame::Box | QFrame::Raised);
  // unveilColor->setLineWidth(0);
   // unveilColor->setMidLineWidth(1);

        demasque_pix = new QPixmap(":/resources/icones/demasque.png");
        m_unveilColor = new ColorButton(demasque_pix,this);
        m_unveilColor->setFixedHeight(15);
   // unveilColor->setPixmap(*demasque_pix);
   // unveilColor->setScaledContents(true);
    m_unveilColor->setPalette(QPalette(Qt::white));
    m_unveilColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_unveilColor);


     QButtonGroup group;
     group.addButton(m_eraseColor);
     group.addButton(m_hideColor);
     group.addButton(m_unveilColor);

	// Taille de la palette
	setFixedHeight(126);
	//setMaximumWidth((TAILLE_ICONES+7)*2);

	// On autorise ou pas la selection des couleurs de masquage/demasquage en fonction de la nature de l'utilisateur (MJ/joueur)
    allowOrForbideColors();
}
void ColorSelector::selectColor(const QColor& color)
{
    //m_currentColorLabel->clear();
    m_currentColorLabel->setPalette(QPalette(color));
    m_currentColor = color;
    emit currentColorChanged(m_currentColor);

}





void ColorSelector::allowOrForbideColors()
{
	// L'utilisateur est un joueur
/*	if (G_joueur)
	{
		// Le masquage est total
		G_couleurMasque = QColor(0,0,0);
		// Message d'interdiction pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer (MJ seulement)"));
        unveilColor->setToolTip(tr("Démasquer (MJ seulement)"));
		// Il est impossible de selectionner les couleurs de masquage et de demasquage
		couleurMasque->setEnabled(false);
        unveilColor->setEnabled(false);
	}

	// L'utilisateur est un MJ
	else
	{
		// Le masque est transparent
		G_couleurMasque = QColor(50,50,50);
		// Tooltip normaux pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer"));
        unveilColor->setToolTip(tr("Démasquer"));
    }*/
}

/********************************************************************/
/* Change la couleur actuelle                                       */
/********************************************************************/		
void ColorSelector::setCurrentColor(QColor& couleur)
{

    //m_currentColorLabel->clear();
    m_currentColorLabel->setPalette(QPalette(couleur));

    m_currentColorLabel->setToolTip(tr("Red: %1, Green: %2, Blue: %3").arg(couleur.red()).arg(couleur.green()).arg(couleur.blue()));


    m_currentColor = couleur;
    emit currentColorChanged(m_currentColor);
}
QColor& ColorSelector::currentColor()
{
    return m_currentColor;

}

/********************************************************************/	
/* M.a.j des couleurs personnelles                                  */
/********************************************************************/	
void ColorSelector::customColorUpdate()
{
	for (int i=0, j=0; i<16; i++)
	{
		couleurPersonnelle[i]->setPalette(QPalette(QColor(QColorDialog::customColor(j))));
		j+=2;
		j = j<=15?j:1;
	}
}

/********************************************************************/	
/* Renvoie la couleur personnelle dont le numero est passe en       */
/* parametre                                                        */
/********************************************************************/	
QColor ColorSelector::getPersonalColor(int numero)
{
	int numCouleur;

	// On fait la conversion
	if (numero%2)
		numCouleur = (numero-1)/2+8;
	else
		numCouleur = numero/2;

	return (couleurPersonnelle[numCouleur]->palette()).color(QPalette::Window);
}



void ColorSelector::colorSelectorDialog()
{
    QColor color = QColorDialog::getColor(m_currentColor);
    if (color.isValid())
    {
        m_currentColorLabel->setPalette(QPalette(color));
        m_currentColor = color;
        m_currentColorLabel->setToolTip(tr("Red: %1, Green: %2, Blue: %3").arg(color.red()).arg(color.green()).arg(color.blue()));
    }
}





