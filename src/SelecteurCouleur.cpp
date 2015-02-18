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


#include "SelecteurCouleur.h"


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
	majCouleursPersonnelles();

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
	couleurEfface = new QLabel(this);
	couleurEfface->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurEfface->setLineWidth(0);
	couleurEfface->setMidLineWidth(1);
	couleurEfface->setFixedHeight(15);
        efface_pix = new QPixmap(":/resources/icones/efface.png");
	couleurEfface->setPixmap(*efface_pix);
	couleurEfface->setScaledContents(true);
	couleurEfface->setToolTip(tr("Effacer"));
	couleurEfface->setPalette(QPalette(Qt::white));
	couleurEfface->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurEfface);
	
	// Ajout de la couleur speciale qui masque
	couleurMasque = new QLabel(this);
	couleurMasque->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurMasque->setLineWidth(0);
	couleurMasque->setMidLineWidth(1);
	couleurMasque->setFixedHeight(15);
        masque_pix = new QPixmap(":/resources/icones/masque.png");
	couleurMasque->setPixmap(*masque_pix);
	couleurMasque->setScaledContents(true);
	couleurMasque->setPalette(QPalette(Qt::white));
	couleurMasque->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurMasque);

	// Ajout de la couleur speciale qui demasque
	couleurDemasque = new QLabel(this);
	couleurDemasque->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurDemasque->setLineWidth(0);
	couleurDemasque->setMidLineWidth(1);
	couleurDemasque->setFixedHeight(15);
        demasque_pix = new QPixmap(":/resources/icones/demasque.png");
	couleurDemasque->setPixmap(*demasque_pix);
	couleurDemasque->setScaledContents(true);
	couleurDemasque->setPalette(QPalette(Qt::white));
	couleurDemasque->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurDemasque);

	// Taille de la palette
	setFixedHeight(126);
	//setMaximumWidth((TAILLE_ICONES+7)*2);

	// On autorise ou pas la selection des couleurs de masquage/demasquage en fonction de la nature de l'utilisateur (MJ/joueur)
	autoriserOuInterdireCouleurs();
}
void ColorSelector::selectColor(const QColor& color)
{
    //m_currentColorLabel->clear();
    m_currentColorLabel->setPalette(QPalette(color));
    m_currentColor = color;
    emit currentColorChanged(m_currentColor);

}





void ColorSelector::autoriserOuInterdireCouleurs()
{
	// L'utilisateur est un joueur
/*	if (G_joueur)
	{
		// Le masquage est total
		G_couleurMasque = QColor(0,0,0);
		// Message d'interdiction pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer (MJ seulement)"));
		couleurDemasque->setToolTip(tr("Démasquer (MJ seulement)"));
		// Il est impossible de selectionner les couleurs de masquage et de demasquage
		couleurMasque->setEnabled(false);
		couleurDemasque->setEnabled(false);
	}

	// L'utilisateur est un MJ
	else
	{
		// Le masque est transparent
		G_couleurMasque = QColor(50,50,50);
		// Tooltip normaux pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer"));
		couleurDemasque->setToolTip(tr("Démasquer"));
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
void ColorSelector::majCouleursPersonnelles()
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
QColor ColorSelector::donnerCouleurPersonnelle(int numero)
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





