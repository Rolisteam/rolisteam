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

#include "MinutesEditor.h"


MinutesEditor::MinutesEditor()
    : SubMdiWindows()
{

    setObjectName("MinutesEditor");
    QWidget* main = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    m_type = SubMdiWindows::TEXT;



    setWindowIcon(QIcon(":/icones/vignette m_minutes.png"));


    m_minutes = new QTextEdit();
    m_minutes->setAcceptRichText(false);


    QObject::connect(m_minutes, SIGNAL(cursorPositionChanged()), this, SLOT(updateSize()));



   // m_styleBar = systemMenu();
    m_styleBar = new QToolBar;

    layout->addWidget(m_styleBar);


    m_styleBar->setIconSize(QSize(16, 16));
    layout->setMargin(0);

        QAction	*act1 = m_styleBar->addAction(QIcon(":/resources/icons/style normal.png"), tr("Normal"), this, SLOT(normalStyle()));
        QAction	*act2 = m_styleBar->addAction(QIcon(":/resources/icons/style gras.png"), tr("Gras"), this, SLOT(boldStyle()));
        QAction	*act3 = m_styleBar->addAction(QIcon(":/resources/icons/style italique.png"), tr("Italique"), this, SLOT(italicStyle()));
        QAction	*act4 = m_styleBar->addAction(QIcon(":/resources/icons/style souligne.png"), tr("Souligné"), this, SLOT(underlineStyle()));
        Q_UNUSED(act1)
        Q_UNUSED(act2)
        Q_UNUSED(act3)
        Q_UNUSED(act4)
    #ifdef MACOS

        QPlastiqueStyle *style = new QPlastiqueStyle();
        m_styleBar->widgetForAction(act1)->setStyle(style);
        m_styleBar->widgetForAction(act2)->setStyle(style);
        m_styleBar->widgetForAction(act3)->setStyle(style);
        m_styleBar->widgetForAction(act4)->setStyle(style);
    #endif


    m_styleBar->addSeparator();


    m_sizeSelector = new QComboBox();
    m_sizeSelector->setEditable(false);
    #ifdef WIN32
        m_sizeSelector->setFixedWidth(40);
    #elif defined (MACOS)
        m_sizeSelector->setFixedWidth(56);
    #endif
    m_sizeSelector->setToolTip(tr("Taille de la police"));

    QStringList listeTailles;
    listeTailles << "8" << "10" << "12" << "14" << "16" << "18" << "20" << "22" << "26" << "30" << "40" << "50" << "70";

    m_sizeSelector->addItems(listeTailles);

    QObject::connect(m_sizeSelector, SIGNAL(activated(int)), this, SLOT(changeSize(int)));


    //m_styleBar->addWidget(m_sizeSelector);
    layout->addWidget(m_minutes);

    layout->setContentsMargins(0,0,0,0);
     main->setLayout(layout);
     setWidget(main);

}


void MinutesEditor::closeEvent(QCloseEvent *event)
{
    event->ignore();
    setVisible(false);
}


void MinutesEditor::normalStyle()
{
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On cree un style normal a appliquer a la zone
    QTextCharFormat styleZone;
    styleZone.setFontItalic(false);
    styleZone.setFontUnderline(false);
    styleZone.setFontWeight(QFont::Normal);
    // On ajoute le style a la zone
    zone.mergeCharFormat(styleZone);
    // Enfin on recopie la zone dans les m_minutes
    m_minutes->setTextCursor(zone);
}


void MinutesEditor::boldStyle()
{
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On cree un style gras a appliquer a la zone
    QTextCharFormat styleZone;
    styleZone.setFontWeight(QFont::Bold);
    // On ajoute le style a la zone
    zone.mergeCharFormat(styleZone);
    // Enfin on recopie la zone dans les m_minutes
    m_minutes->setTextCursor(zone);
}


void MinutesEditor::italicStyle()
{
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On cree un style italique a appliquer a la zone
    QTextCharFormat styleZone;
    styleZone.setFontItalic(true);
    // On ajoute le style a la zone
    zone.mergeCharFormat(styleZone);
    // Enfin on recopie la zone dans les m_minutes
    m_minutes->setTextCursor(zone);
}

void MinutesEditor::underlineStyle()
{
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On cree un style souligne a appliquer a la zone
    QTextCharFormat styleZone;
    styleZone.setFontUnderline(true);
    // On ajoute le style a la zone
    zone.mergeCharFormat(styleZone);
    // Enfin on recopie la zone dans les m_minutes
    m_minutes->setTextCursor(zone);
}


void MinutesEditor::updateSize()
{
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On recupere le style de la zone
    QTextCharFormat styleActuel = zone.charFormat();
    // On recupere la taille de la fonte a cet endroit
    QFont fonte = styleActuel.font();
    int tailleFonte = fonte.pointSize();
    // On cherche la taille equivalente dans le selecteur de taille
    int index = m_sizeSelector->findText(QString::number(tailleFonte));

    // Si la taille a ete trouvee on met a jour le selecteur
    if (index != -1)
        m_sizeSelector->setCurrentIndex(index);

    // Dans le cas contraire on insere l'element dans la liste
    else
    {
        int i;
        bool trouve = false;
        int tailleListe = m_sizeSelector->count();
        // On parcourt la liste des taille jusqu'a ce que l'on trouve une taille superieure
        for (i=0; i<tailleListe && !trouve; i++)
            if (m_sizeSelector->itemText(i).toInt() > tailleFonte)
                trouve = true;
        // On met i a jour en fonction du resultat de la recherche
        i = trouve?i-1:i;
        // On insert la nouvelle taille avant l'element en question
        m_sizeSelector->insertItem(i, QString::number(tailleFonte));
        // La nouvelle taille est selectionnee
        m_sizeSelector->setCurrentIndex(i);
    }
}


void MinutesEditor::changeSize(int index)
{
    // On recupere la taille selectionnee
    int tailleFonte = m_sizeSelector->itemText(index).toInt();
    // On recupere la zone du curseur
    QTextCursor zone = m_minutes->textCursor();
    // On recupere le style de la zone
    QTextCharFormat styleActuel = zone.charFormat();
    // On regarde si la fonte est soulignee (l'information n'est pas conservee lors de l'operation)
    bool souligne = styleActuel.fontUnderline();
    // On recupere la fonte a cet endroit
    QFont fonte = styleActuel.font();
    // On met a jour la taille de la fonte
    fonte.setPointSize(tailleFonte);
    // On remet la fonte dans le style
    styleActuel.setFont(fonte);
    // On remet l'etat souligne ou pas
    styleActuel.setFontUnderline(souligne);
    // On met a jour le style de la zone
    zone.mergeCharFormat(styleActuel);
    // Enfin on recopie la zone dans les m_minutes
    m_minutes->setTextCursor(zone);
    // On remet le curseur sur la zone de texte
    m_minutes->setFocus(Qt::OtherFocusReason);
}


void MinutesEditor::openMinutes(QFile &file)
{
    // On cree un flux de donnees rattache au fichier
    QTextStream fichier(&file);
    // On recupere le fichier HTML sous forme de string
    QString html = fichier.readAll();
    // On ecrit le contenu HTML dans les m_minutes
    m_minutes->setHtml(html);
}


void MinutesEditor::saveMinutes(QFile &file)
{
    // On recupere le document contenant les m_minutes
    QTextDocument *document = m_minutes->document();
    // On convertit le document en HTML
    QString html = document->toHtml(QByteArray("UTF-8"));
    // On cree un flux de donnees rattache au fichier
    QTextStream fichier(&file);
    // On envoie le document HTML dans le flux
    fichier << html;
}


QSize MinutesEditor::sizeHint() const
{
    return QSize(600, 600);
}

bool MinutesEditor::defineMenu(QMenu* /*menu*/)
{
    return false;
}
