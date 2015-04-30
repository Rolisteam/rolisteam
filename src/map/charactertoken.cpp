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
#include <QVBoxLayout>


#include <math.h>

#include "variablesGlobales.h"
#include "map/charactertoken.h"

// Liste des etats de sante des PJ/PNJ (initialisee dans MainWindow.cpp)
//QList<DessinPerso::etatDeSante> G_etatsDeSante;


DessinPerso::DessinPerso(QWidget *parent, QString persoId, QString nom, QColor couleurPerso, int taille, QPoint position, typePersonnage leType,bool showNpcNumber,bool showName, int numero,bool isLocal)
    : QWidget(parent),m_localPerso(isLocal)
{

	AddHealthState(Qt::black, tr("healthy"));
	AddHealthState(QColor(255, 100, 100),tr("lightly wounded"));
	AddHealthState(QColor(255, 0, 0),tr("seriously injured"));
	AddHealthState(Qt::gray,tr("Dead"));
	AddHealthState(QColor(80, 80, 255),tr("Sleeping"));
	AddHealthState(QColor(0, 200, 0),tr("Bewitched"));

    // Initiatialisation des variables
    nomPerso = nom;
    numeroPnj = (uchar) numero;
    couleur = couleurPerso;
    identifiant = persoId;
    orientation = QPoint(45, 0);
    visible = false;
    orientationAffichee = false;
    numeroEtat = 0;
    diametre = taille;
    etat = m_healtStateList[numeroEtat];
    type = leType;

    if (type == pj)
    {
        m_showNpcNumber = false;
        m_showPcName = showName;
    }
    else
    {
        m_showNpcNumber = showNpcNumber;
        m_showNpcName = showName;
    }

    // Creation du label contenant le dessin du personnage
    disquePerso = new QLabel(this);
    // On donne un nom au label (pour pouvoir ensuite differencier les enfants de la carte)
    disquePerso->setObjectName("disquePerso");
    // M.a.j de la taille du label
    disquePerso->resize(QSize(diametre+4, diametre+4));
    // Ajout du tooltip au label
    if (type == pj)
        disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
    else
        disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");

    // Dessin du personnage et ajout de l'image au label
    dessinerPersonnage();

    // Initialisation du tableau de points formant le contour du disque
    QImage image = disquePerso->pixmap()->toImage();
    initialiserContour(image);

    // Creation du label contenant le nom et le numero du personnage
    intitulePerso = new QLabel(this);
    // On donne un nom au label (pour pouvoir ensuite differencier les enfants de la carte)
    intitulePerso->setObjectName("intitulePerso");
    // Met l'intitule a jour et dimensionne DessinPerso en fonction
    mettreIntituleAJour();

    // Creation du layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(disquePerso);
    layout->addWidget(intitulePerso);
    layout->setAlignment(disquePerso, Qt::AlignCenter);
    layout->setAlignment(intitulePerso, Qt::AlignCenter);
    setLayout(layout);

    // Connexion des signaux
    if (type == pj)
    {
        connect(parent, SIGNAL(afficherNomsPj(bool)), this, SLOT(afficherNomsPj(bool)));
        connect(parent, SIGNAL(changerTaillePj(int)), this, SLOT(changerTaillePj(int)));
    }
    else
    {
        connect(parent, SIGNAL(afficherNomsPnj(bool)), this, SLOT(afficherNomsPnj(bool)));
        connect(parent, SIGNAL(afficherNumerosPnj(bool)), this, SLOT(afficherNumerosPnj(bool)));
    }

    // On donne un nom au widget (pour pouvoir ensuite differencier les enfants de la carte)
    setObjectName("DessinPerso");
    // Deplacement du widget
    move(position.x()-width()/2, position.y()-disquePerso->height()/2);
    // On memorise la position du centre
    centre = QPoint(x()+width()/2, y()+disquePerso->height()/2);
    // A la fin du constructeur le widget n'est pas affiche
    hide();
}

void DessinPerso::deplacePerso(QPoint position)
{
    move(position);
}

void DessinPerso::deplacePerso(int x, int y)
{
    move(x, y);
}

void DessinPerso::deplaceCentrePerso(QPoint position)
{
    // On calcule la nouvelle position de disquePerso par rapport a DessinPerso
    QPoint nouvellePosition = mapFromParent( position - QPoint(disquePerso->width()/2, disquePerso->height()/2) );
    // Puis on regarde de combien il a bouge par rapport a son ancienne position dans DessinPerso
    QPoint delta = disquePerso->pos() - nouvellePosition;
    // On deplace DessinPerso du delta
    move (pos()-delta);
}


QPoint DessinPerso::positionCentrePerso()
{
    // On recupere la position de disquePerso par rapport aux coordonnees de la carte
    // et on y ajoute la moitie de la largeur et de la hauteur de disquePerso
    return mapTo(parentWidget(), disquePerso->pos()) + QPoint(disquePerso->width()/2, disquePerso->height()/2);
}

void DessinPerso::diametreCouleurNom(int *diam, QColor *coul, QString *nom)
{
    *diam = diametre;
    *coul = couleur;
    *nom = nomPerso;
}

bool DessinPerso::dansPartieTransparente(QPoint position)
{
    // On translate le point dans l'espace des coordonnees de disquePerso
    QPoint positionDansDisquePerso = disquePerso->mapFrom(parentWidget(), position);
    // Recuperation de la pixmap de disquePerso et conversion en image
    QImage image = (disquePerso->pixmap())->toImage();

    return qAlpha(image.pixel(positionDansDisquePerso.x(), positionDansDisquePerso.y())) == 0;
}

void DessinPerso::afficherNomsPj(bool afficher)
{
    m_showPcName = afficher;
    mettreIntituleAJour();
}

void DessinPerso::afficherNomsPnj(bool afficher)
{
    m_showNpcName = afficher;
    mettreIntituleAJour();
}

void DessinPerso::afficherNumerosPnj(bool afficher)
{
    m_showNpcNumber = afficher;
    mettreIntituleAJour();
}

bool DessinPerso::estVisible()
{
    return visible;
}

void DessinPerso::afficherPerso()
{
    visible = true;
    show();
    // On remet le personnage a sa position d'avant le masquage
    deplaceCentrePerso(centre);
}

void DessinPerso::cacherPerso()
{
    // On memorise la position du centre avant de cacher le personnage
    centre = positionCentrePerso();
    visible = false;
    hide();
}

void DessinPerso::mettreIntituleAJour()
{
    // Recuperation de la position centrale du perso
    QPoint position = positionCentrePerso();
    // recuperation de l'ancien texte du label
    QString ancienTexte = intitulePerso->text();

    QString nameText="";
    switch(type)
    {
    case pj:
        nameText=m_showPcName?nomPerso:"";
        break;
    case pnj:
         nameText=m_showNpcName?nomPerso:"";
        break;
    }


    QString texteNumero = m_showNpcNumber?QString::number(numeroPnj):"";

    // Ajout du texte au label
    intitulePerso->setText(nameText + (nameText.isEmpty() || texteNumero.isEmpty()?"":" - ") + texteNumero);

    // Parametrage de la fonte (taille et couleur)
    QFont font;
    font.setPixelSize(12);
    intitulePerso->setFont(font);
    intitulePerso->setPalette(QPalette(couleur));
    intitulePerso->setForegroundRole(QPalette::Window);

    // Calcul de l'espace occupe par le texte
    QFontMetrics fm(font);
    QSize tailleTexte = fm.size(Qt::TextSingleLine, intitulePerso->text());
    int largeurWidget = tailleTexte.width()<disquePerso->width()?disquePerso->width():tailleTexte.width();

    // Redimensionnement du widget
    if ( (intitulePerso->text()).isEmpty() )
    {
        // On masque l'intitule
        intitulePerso->hide();
        // redimensionnement du widget
        resize(largeurWidget, disquePerso->height());
        // Repositionne le perso au meme endroit
        deplaceCentrePerso(position);
    }
    else
    {
        resize(largeurWidget, disquePerso->height() + tailleTexte.height());
        // Le repositionnement ne fonctionne correctement qui si on le place apres le show()
        // mais on a un clignotement a l'affichage. Si on le place avant le show() le widget s'affiche
        // plus haut qu'a la normale (il se decale progressivement). Cela est certainement du
        // au fait que l'evenement resizeEvent n'a pas lieu tant que le widget intitulePerso est cache.
        // Qt ne doit donc pas etre en mesure de calculer correctement les changements de
        // referentiel pour les coordonnees dans deplaceCentrePerso.
        // Le pb n'arrive que lorsque l'on reaffiche le label apres l'avoir masque (donc quand
        // l'acien texte est vide)
        // Pour corriger le pb on rajoute la moitie de la hauteur du texte (empirique).
        if (ancienTexte.isEmpty())
            deplaceCentrePerso( QPoint(position.x(), position.y()+tailleTexte.height()/2) );
        // Sinon on repositionne le perso au meme endroit
        else
            deplaceCentrePerso(position);
        // On affiche l'intitule
        intitulePerso->show();
    }
}


void DessinPerso::initialiserContour(QImage &disque)
{
    // Initialisation des 4 points haut, bas, gauche et droite
    contour[0] = QPoint (disque.width()/2, 0);
    contour[1] = QPoint (disque.width()/2, disquePerso->height());
    contour[2] = QPoint (0, disque.height()/2);
    contour[3] = QPoint (disquePerso->width(), disquePerso->height()/2);

    int x, y;
    // Initialisation du point haut gauche
    for (x=0, y=0; qAlpha(disque.pixel(x,y))!=255; x++, y++) ;
    contour[4] = QPoint(x, y);

    // Initialisation du point haut droite
    for (x=disque.width()-1, y=0; qAlpha(disque.pixel(x,y))!=255; x--, y++) ;
    contour[5] = QPoint(x, y);

    // Initialisation du point bas gauche
    for (x=0, y=disque.height()-1; qAlpha(disque.pixel(x,y))!=255; x++, y--) ;
    contour[6] = QPoint(x, y);

    // Initialisation du point bas gauche
    for (x=disque.width()-1, y=disque.height()-1; qAlpha(disque.pixel(x,y))!=255; x--, y--) ;
    contour[7] = QPoint(x, y);
}

void DessinPerso::contourDisque(QPoint *coordonnees)
{
    for (int i=0; i<8; i++)
        coordonnees[i] = disquePerso->mapTo(parentWidget(), contour[i]);
}

void DessinPerso::dessinerPersonnage(QPoint positionSouris)
{
    // Si la position de la souris est passee en parametre
    if (positionSouris != QPoint(0,0))
    {
        // On met a jour l'orientation du personnage
        QPoint centre = positionCentrePerso();
        orientation =  positionSouris - centre;
    }

    // Creation de l'image contenant le dessin du PJ/PNJ
    QImage disqueImage(QSize(diametre+4, diametre+4), QImage::Format_ARGB32_Premultiplied);
    // Image transparente
    disqueImage.fill(qRgba(0, 0, 0, 0));
    // Creation du painter servant a dessiner sur l'image
    QPainter painterDisque(&disqueImage);
    painterDisque.setRenderHint(QPainter::Antialiasing);

    // Dessin d'un disque
    QPen pen;
    pen.setColor(etat.couleurEtat);
    pen.setWidth(4);
    painterDisque.setPen(pen);
    painterDisque.setBrush(couleur);
    painterDisque.drawEllipse(2,2,disqueImage.width()-4, disqueImage.height()-4);
    // Delimitation de la zone de dessin
    QRegion clip(1, 1, disqueImage.width()-2, disqueImage.height()-2, QRegion::Ellipse);
    painterDisque.setClipRegion(clip);

    // Si l'orientation doit etre affichee...
    if (orientationAffichee)
    {
        // Les 2 points de la ligne
        QPoint p1 = QPoint(diametre/2+2, diametre/2+2);
        QPoint p2 = p1 + orientation;
        // Dessin de la ligne d'orientation
        if (etat.couleurEtat.value() >= 128)
            pen.setColor(Qt::black);
        else
            pen.setColor(Qt::white);
        pen.setWidth(3);
        painterDisque.setPen(pen);
        painterDisque.drawLine(p1, p2);
        // Dessin d'un cercle au milieu du disque
        pen.setColor(couleur);
        pen.setWidth(1);
        painterDisque.setPen(pen);
        painterDisque.setBrush(couleur);
        QRectF ellipse(0, 0, (double)diametre/2, (double)diametre/2);
        ellipse.moveCenter(QPointF((double)diametre/2+2, (double)diametre/2+2));
        painterDisque.drawEllipse(ellipse);
    }

    // Si le personnage est un PJ on dessine son centre en noir
    if (type == pj)
    {
        pen.setColor(Qt::black);
        pen.setWidth(1);
        painterDisque.setPen(pen);
        painterDisque.setBrush(Qt::black);
        QRectF ellipse(0, 0, (double)diametre/3, (double)diametre/3);
        ellipse.moveCenter(QPointF((double)diametre/2+2, (double)diametre/2+2));
        painterDisque.drawEllipse(ellipse);
    }

    // Ajout de l'image au label
    disquePerso->setPixmap(QPixmap::fromImage(disqueImage));
    if(m_localPerso)
    {
       disquePerso->raise();
       raise();
    }
    // Ajout du tooltip au label
    if (type == pj)
        disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
    else if (type == pnj)
        disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");
    else
        qWarning()<< (tr("Type de personnage inconnu (dessinerPersonnage - DessinPerso.cpp)"));

}

void DessinPerso::afficheOuMasqueOrientation()
{
    orientationAffichee = !orientationAffichee;
    dessinerPersonnage();
}

void DessinPerso::showOrientation(bool afficher)
{
    orientationAffichee = afficher;
    dessinerPersonnage();
}

void DessinPerso::changerEtat()
{
    numeroEtat = numeroEtat<m_healtStateList.size()-1?numeroEtat+1:0;
    etat = m_healtStateList[numeroEtat];
    dessinerPersonnage();
}

bool DessinPerso::estUnPj()
{
    return type == pj;
}

bool DessinPerso::orientationEstAffichee()
{
    return orientationAffichee;
}

QString DessinPerso::idPersonnage()
{
    return identifiant;
}

QPoint DessinPerso::orientationPersonnage()
{
    return orientation;
}

void DessinPerso::changerTaillePj(int nouvelleTaille)
{
    // Recuperation de la position centrale du PJ
    QPoint position = positionCentrePerso();
    // M.a.j du diametre
    diametre = nouvelleTaille;
    // M.a.j de la taille du label contenant l'image
    disquePerso->resize(QSize(diametre+4, diametre+4));
    // M.a.j du dessin du PJ
    dessinerPersonnage();
    // M.a.j de l'intitule (redimensionne egalement le widget)
    mettreIntituleAJour();
    // M.a.j des contours du disque
    QImage image = disquePerso->pixmap()->toImage();
    initialiserContour(image);
    // Deplacement du PJ en gardant le meme centre
    deplaceCentrePerso(position);
}


void DessinPerso::renommerPerso(QString nouveauNom)
{
    // M.a.j du nom du perso
    nomPerso = nouveauNom;
    // M.a.j de l'intitule (redimensionne egalement le widget)
    mettreIntituleAJour();
    // M.a.j du tooltip
    if (type == pj)
        disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
    else if (type == pnj)
        disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");
    else
        qWarning()<< (tr("Type de personnage inconnu (renommerPerso - DessinPerso.cpp)"));
}

void DessinPerso::changerCouleurPerso(QColor coul)
{
    couleur = coul;
    // M.a.j du dessin du PJ
    dessinerPersonnage();
    // M.a.j de l'intitule
    mettreIntituleAJour();
}

void DessinPerso::nouvelleOrientation(QPoint uneOrientation)
{
    orientation = uneOrientation;
    // M.a.j du dessin du personnage
    dessinerPersonnage();
}

void DessinPerso::nouvelEtatDeSante(etatDeSante sante, int numeroSante)
{
    numeroEtat = numeroSante;
    etat = sante;
    dessinerPersonnage();
}

void DessinPerso::changerEtatDeSante(int numEtat)
{
    numeroEtat = numEtat<=m_healtStateList.size()-1?numEtat:0;
    etat = m_healtStateList[numeroEtat];
    dessinerPersonnage();
}

int DessinPerso::numeroEtatSante()
{
    return numeroEtat;
}

void DessinPerso::emettrePnj(QString idCarte)
{
    NetworkMessageWriter msg(NetMsg::NPCCategory,NetMsg::addNpc);
    msg.string8(idCarte);
    preparerPourEmission(&msg);
    msg.sendAll();

}
void DessinPerso::write(QDataStream &out)
{
    QString ident = QUuid::createUuid().toString();
    int numeroDuPnj = numeroPnj;
    if (type==pj)
        numeroDuPnj = 0;
    out << nomPerso << ident << pnj << numeroDuPnj << diametre << couleur << positionCentrePerso() << orientation << etat.couleurEtat << etat.nomEtat << numeroEtat << visible << orientationAffichee;
}
int DessinPerso::preparerPourEmission(NetworkMessageWriter* msg, bool convertirEnPnj)
{

    QString ident;
    quint8 characterType;
    quint8 characterNum = numeroPnj;
    if (convertirEnPnj)
    {
        ident = QUuid::createUuid().toString();
        characterType = pnj;
        characterNum = type==pj ? 0 : numeroPnj;
    }
    else
    {
        ident = identifiant;
        characterType = type;
    }

    msg->string16(nomPerso);
    msg->string8(ident);
    msg->uint8(characterType);
    msg->uint8(characterNum);
    msg->uint8(diametre);
    msg->rgb(couleur);

    msg->uint16(positionCentrePerso().x());
    msg->uint16(positionCentrePerso().y());

    msg->int16(orientation.x());
    msg->int16(orientation.y());

    msg->rgb(etat.couleurEtat);

    msg->string16(etat.nomEtat);
    msg->uint16(numeroEtat);
    msg->uint8( visible);
    msg->uint8(orientationAffichee);
}
void DessinPerso::AddHealthState(const QColor &color, const QString &label)
{
	DessinPerso::etatDeSante state;
	state.couleurEtat = color;
	state.nomEtat = label;
	m_healtStateList.append(state);
}
