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

#include <QFontComboBox>

#include "minuteseditor.h"
MinutesEditor::MinutesEditor()
: SubMdiWindows()
{
    /// @todo allocate all the stuff for Minute Editor
}
MinutesEditor::MinutesEditor(CleverURI* uri)
    : SubMdiWindows()
{
    setObjectName("MinutesEditor");
    QWidget* main = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    m_uri = uri;
    setWindowTitle(tr("Minutes Editor"));
    setWindowIcon(QIcon(":/resources/icons/notes.png"));

    //////////////////
    // Style selector
    //////////////////
    m_styleSelector = new QComboBox(this);
    m_styleSelector->addItem(tr("Standard"));
    m_styleSelector->addItem(tr("Bullet List (Disc)"));
    m_styleSelector->addItem(tr("Bullet List (Circle)"));
    m_styleSelector->addItem(tr("Bullet List (Square)"));
    m_styleSelector->addItem(tr("Ordered List (Decimal)"));
    m_styleSelector->addItem(tr("Ordered List (Alpha lower)"));
    m_styleSelector->addItem(tr("Ordered List (Alpha upper)"));

    /////////////////
    // Font selector
    /////////////////
    m_fontSelector = new QFontComboBox;
    m_fontSelector->setEditable(false);
    m_fontSelector->setMaximumWidth(100);


    /////////////////
    // font size
    /////////////////
    QStringList sizeList;
    sizeList << "8" << "10" << "12" << "14" << "16" << "18" << "20" << "22" << "26" << "30" << "40" << "50" << "70";
    m_sizeSelector = new QComboBox;
    m_sizeSelector->addItems(sizeList);





    /////////////////
    // tool bar
    /////////////////
    m_styleBar = new QToolBar;
    m_styleBar->setStyleSheet("QToolBar { border: 0px }");
    m_styleBar->setIconSize(QSize(16, 16));

    m_styleBar->addWidget(m_styleSelector);
    m_styleBar->addWidget(m_fontSelector);
    m_styleBar->addWidget(m_sizeSelector);

    m_styleBar->addSeparator();
    /////////////////
    // Action
    /////////////////
    m_italicAct= m_styleBar->addAction(QIcon(":/resources/icons/textitalic.png"), tr("Italic"), this, SLOT(italicStyle()));
    m_boldAct = m_styleBar->addAction(QIcon(":/resources/icons/textbold.png"), tr("Bold"), this, SLOT(boldStyle()));
    m_underLineAct=m_styleBar->addAction(QIcon(":/resources/icons/textunder.png"), tr("Underline"), this, SLOT(underlineStyle()));
    m_leftAct= m_styleBar->addAction(QIcon(":/resources/icons/textleft.png"), tr("Left"), this, SLOT(boldStyle()));
    m_centerAct = m_styleBar->addAction(QIcon(":/resources/icons/textcenter.png"), tr("Center"), this, SLOT(boldStyle()));
    m_rightAct= m_styleBar->addAction(QIcon(":/resources/icons/textright.png"), tr("Right"), this, SLOT(boldStyle()));
    m_justifyAct= m_styleBar->addAction(QIcon(":/resources/icons/textjustify.png"), tr("Justify"), this, SLOT(boldStyle()));

    m_sizeSelector = new QComboBox();
    m_sizeSelector->setEditable(false);
    m_sizeSelector->setToolTip(tr("Font Size"));

    m_minutes = new QTextEdit();
    m_minutes->setAcceptRichText(true);
    m_fontSelector->setCurrentFont(m_minutes->font());

    layout->setContentsMargins(0,0,0,0);
    layout->setMenuBar(m_styleBar);
    layout->setSpacing(0);
    layout->addWidget(m_minutes);

     main->setLayout(layout);
     setWidget(main);

     connect(m_sizeSelector, SIGNAL(activated(int)), this, SLOT(changeSize(int)));
     connect(m_minutes, SIGNAL(cursorPositionChanged()), this, SLOT(updateSize()));

}


void MinutesEditor::closeEvent(QCloseEvent *event)
{
    event->ignore();
    setVisible(false);
}


void MinutesEditor::normalStyle()
{
    QTextCursor zone = m_minutes->textCursor();
    QTextCharFormat styleZone;
    styleZone.setFontItalic(false);
    styleZone.setFontUnderline(false);
    styleZone.setFontWeight(QFont::Normal);
    zone.mergeCharFormat(styleZone);
    m_minutes->setTextCursor(zone);
}


void MinutesEditor::boldStyle()
{

    QTextCursor zone = m_minutes->textCursor();

    QTextCharFormat styleZone;
    if(m_minutes->textCursor().charFormat().fontWeight()==QFont::Bold)
        styleZone.setFontWeight(QFont::Normal);
    else
        styleZone.setFontWeight(QFont::Bold);

    zone.mergeCharFormat(styleZone);

    m_minutes->setTextCursor(zone);
}


void MinutesEditor::italicStyle()
{

    QTextCursor zone = m_minutes->textCursor();
    QTextCharFormat styleZone;
    styleZone.setFontItalic(!styleZone.fontItalic());
    zone.mergeCharFormat(styleZone);
    m_minutes->setTextCursor(zone);
}

void MinutesEditor::underlineStyle()
{

    QTextCursor zone = m_minutes->textCursor();

    QTextCharFormat styleZone;
    styleZone.setFontUnderline(true);

    zone.mergeCharFormat(styleZone);

    m_minutes->setTextCursor(zone);
}


void MinutesEditor::updateSize()
{

    QTextCursor zone = m_minutes->textCursor();

    QTextCharFormat styleActuel = zone.charFormat();

    QFont fonte = styleActuel.font();
    int tailleFonte = fonte.pointSize();

    int index = m_sizeSelector->findText(QString::number(tailleFonte));


    if (index != -1)
        m_sizeSelector->setCurrentIndex(index);


    else
    {
        int i;
        bool trouve = false;
        int tailleListe = m_sizeSelector->count();

        for (i=0; i<tailleListe && !trouve; i++)
            if (m_sizeSelector->itemText(i).toInt() > tailleFonte)
                trouve = true;

        i = trouve?i-1:i;

        m_sizeSelector->insertItem(i, QString::number(tailleFonte));

        m_sizeSelector->setCurrentIndex(i);
    }
}


void MinutesEditor::changeSize(int index)
{   
    int tailleFonte = m_sizeSelector->itemText(index).toInt();
    QTextCursor zone = m_minutes->textCursor();
    QTextCharFormat styleActuel = zone.charFormat();

    bool souligne = styleActuel.fontUnderline();
    QFont fonte = styleActuel.font();
    fonte.setPointSize(tailleFonte);
    styleActuel.setFont(fonte);
    styleActuel.setFontUnderline(souligne);
    zone.mergeCharFormat(styleActuel);
    m_minutes->setTextCursor(zone);
    m_minutes->setFocus(Qt::OtherFocusReason);
}

QSize MinutesEditor::sizeHint() const
{
    return QSize(600, 600);
}

bool MinutesEditor::defineMenu(QMenu* /*menu*/)
{
    return false;
}
void MinutesEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = m_minutes->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_minutes->mergeCurrentCharFormat(format);
}
void MinutesEditor::saveFile(const QString & file)
{
    if(!file.isEmpty())
    {
        QFile textfile(file);
        if(textfile.open(QIODevice::WriteOnly))
        {
            QTextDocument *document = m_minutes->document();
            QString html = document->toHtml(QByteArray("UTF-8"));
            QTextStream filewriter(&textfile);
            filewriter << html;
        }
    }
}

void MinutesEditor::openFile(const QString& file)
{
    if(!file.isEmpty())
    {
        QFile textfile(file);
        if(textfile.open(QIODevice::ReadOnly))
        {
            QTextStream fichier(&textfile);
            QString html = fichier.readAll();

            m_minutes->setHtml(html);
        }
    }
}

bool MinutesEditor::hasDockWidget() const
{
    return false;
}
QDockWidget* MinutesEditor::getDockWidget()
{
    return NULL;
}
