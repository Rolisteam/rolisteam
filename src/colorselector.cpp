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
    : QAbstractButton(parent)
{
    setMinimumSize(40,40);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
}
void ColorLabel::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit clickedColor(this->palette().color(QPalette::Window));
    QAbstractButton::mousePressEvent(ev);
}
void ColorLabel::resizeEvent(QResizeEvent * event)
{
    /*int length = width()>height() ? height() : width();
    setFixedSize(length,length);*/
    QWidget::resizeEvent(event);
}
void ColorLabel::mouseDoubleClickEvent (QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubledclicked();
    QAbstractButton::mouseDoubleClickEvent(event);
}
void ColorLabel::paintEvent( QPaintEvent * event )
{
   /* QPainter painter(this);
    painter.fillRect(rect(),this->palette().color(QPalette::Window));*/
    QWidget::paintEvent(event);

}

BackgroundButton::BackgroundButton(QPixmap* p,QWidget * parent )
    : QPushButton(parent),m_background(p)
{
   /* setIcon(QIcon(*m_background));
    setIconSize(QSize(rect().width()-2,rect().height()-2));*/
    setCheckable(true);
}

void BackgroundButton::paintEvent( QPaintEvent * event )
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QRect r=rect();
    r.setBottom(r.bottom());
    r.setTop(r.top());
    r.setLeft(r.left());
    r.setRight(r.right());
    painter.drawImage(rect(),m_background->toImage(),m_background->rect());
    if(isChecked())
        painter.drawRect(rect().adjusted(0,0,-1,-1));
    //QPushButton::paintEvent(event);
}


ColorSelector::ColorSelector(QWidget *parent)
	: QWidget(parent)
{
        m_options = PreferencesManager::getInstance();



	// Creation du layout principale
	QVBoxLayout *selecteurLayout = new QVBoxLayout(this);

        m_currentColorLabel = new ColorLabel(this);

        m_currentColorLabel->setPalette(QPalette(QColor(0,0,0)));
        m_currentColorLabel->setToolTip(tr("Predefine Color 1"));
        m_currentColorLabel->setAutoFillBackground(true);
        connect(m_currentColorLabel,SIGNAL(doubledclicked()),this,SLOT(colorSelectorDialog()));

        m_currentColor = QColor(0,0,0);



        m_colorTableChooser = new ColorTableChooser(this);


        selecteurLayout->addWidget(m_currentColorLabel,1);
        //selecteurLayout->setAlignment(m_currentColorLabel, Qt::AlignHCenter | Qt::AlignTop);
        selecteurLayout->addWidget(m_colorTableChooser,1);
        connect(m_colorTableChooser,SIGNAL(currentColorChanged(QColor)),this,SLOT(selectColor(QColor)));

        setLayout(selecteurLayout);





    /*efface_pix = new QPixmap(":/resources/icons/efface.png");
    m_eraseColor = new BackgroundButton(efface_pix,this);
    m_eraseColor->setFixedHeight(17);
    m_eraseColor->setFixedWidth(17);


    m_eraseColor->setToolTip(tr("Effacer"));
    m_eraseColor->setPalette(QPalette(Qt::white));
    m_eraseColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_eraseColor);
	


    masque_pix = new QPixmap(":/resources/icons/masque.png");
    m_hideColor = new BackgroundButton(masque_pix,this);
    m_hideColor->setFixedHeight(17);
    m_hideColor->setFixedWidth(17);*/

    /*m_hideColor->setPalette(QPalette(Qt::white));
    m_hideColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_hideColor);*/



    /*unveil_pix = new QPixmap(":/resources/icons/demasque.png");
    m_unveilColor = new BackgroundButton(unveil_pix,this);
    m_unveilColor->setFixedHeight(17);
    m_unveilColor->setFixedWidth(17);*/


   /* m_unveilColor->setPalette(QPalette(Qt::white));
    m_unveilColor->setAutoFillBackground(true);
    couleursSpeciales->addWidget(m_unveilColor);*/



    /* m_editingModeGroup->addButton(m_eraseColor);
     m_editingModeGroup->addButton(m_hideColor);
     m_editingModeGroup->addButton(m_unveilColor);*/
     //connect(m_editingModeGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(onGroupEdition(QAbstractButton*)));



    //allowOrForbideColors();
}
void ColorSelector::selectColor(const QColor& color)
{

    m_currentColorLabel->setPalette(QPalette(color));
    m_currentColor = color;

   /* QAbstractButton* tmp =  m_editingModeGroup->checkedButton();
    if(tmp != NULL)
        tmp->setChecked(false);*/
    emit currentColorChanged(m_currentColor);
    emit currentModeChanged(NORMAL);
}

void ColorSelector::onGroupEdition(QAbstractButton* b)
{
    if((b == m_eraseColor)&&(m_eraseColor->isChecked()))
    {
        emit currentModeChanged(ERASING);
    }
    else if((b==m_unveilColor)&&(m_unveilColor->isChecked()))
    {
        emit currentModeChanged(UNVEIL);
    }
    else if((b==m_hideColor)&&(m_hideColor->isChecked()))
    {
        emit currentModeChanged(HIDING);
    }
    else
    {
        emit currentModeChanged(NORMAL);
    }
  // enum PAINTINGMODE{NORMAL,HIDING,UNVEIL,ERASING};

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
        /*for (int i=0, j=0; i<16; i++)
	{
		couleurPersonnelle[i]->setPalette(QPalette(QColor(QColorDialog::customColor(j))));
		j+=2;
		j = j<=15?j:1;
        }*/
}

/********************************************************************/	
/* Renvoie la couleur personnelle dont le numero est passe en       */
/* parametre                                                        */
/********************************************************************/	
QColor ColorSelector::getPersonalColor(int numero)
{
        /*int numCouleur;

	// On fait la conversion
	if (numero%2)
		numCouleur = (numero-1)/2+8;
	else
                numCouleur = numero/2;

        return (couleurPersonnelle[numCouleur]->palette()).color(QPalette::Window);*/
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





