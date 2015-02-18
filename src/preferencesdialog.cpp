/*************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include "preferencesdialog.h"
#include "constantesGlobales.h"
#include "variablesGlobales.h"
#include "initialisation.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

/**************
 * DirChooser *
 **************/

DirChooser::DirChooser(QWidget * parent)
 : QWidget(parent)
{
    // Childrens
    m_lineEdit = new QLineEdit(this);
    QPushButton * button = new QPushButton(QString("..."), this);

    // Layout
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(button, 0);
    setLayout(layout);

    // Connections
    connect(button, SIGNAL(pressed()), this, SLOT(browse()));

    // Misc
    button->setMaximumWidth(28);
    setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);
}

DirChooser::~DirChooser()
{
    // QObject should do it right for us allready.
}

void
DirChooser::setDirName(const QString & dirName)
{
    m_lineEdit->setText(dirName);
}

QString
DirChooser::dirName() const
{
    return m_lineEdit->text();
}

void
DirChooser::browse()
{
    QString result = QFileDialog::getExistingDirectory(this,
            tr("Choisir le dossier"),
            m_lineEdit->text(),
            QFileDialog::ShowDirsOnly );
    if (!result.isEmpty())
    {
        m_lineEdit->setText(result);
    }
}


/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
 : QDialog(parent, f)
{
    // Children
#ifndef NULL_PLAYER
    m_gmMusicDir     = new DirChooser;
    m_playerMusicDir = new DirChooser;
#endif
    m_picturesDir    = new DirChooser;
    m_mapsDir        = new DirChooser;
    m_sessionsDir    = new DirChooser;
    m_notesDir       = new DirChooser;
    m_chatsDir       = new DirChooser;

    // Form layout
    QFormLayout * formLayout = new QFormLayout;
#ifndef NULL_PLAYER
    formLayout->addRow(tr("Dossier des musiques (MJ) : "), m_gmMusicDir);
    formLayout->addRow(tr("Dossier des musiques (Joueur) : "), m_playerMusicDir);
#endif
    formLayout->addRow(tr("Dossier des images : "), m_picturesDir);
    formLayout->addRow(tr("Dossier des cartes : "), m_mapsDir);
    formLayout->addRow(tr("Dossier des scénarios : "), m_sessionsDir);
    formLayout->addRow(tr("Dossier des notes : "), m_notesDir);
    formLayout->addRow(tr("Dossier de sauvegarde des tchats : "), m_chatsDir);

    // Buttons
    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Main layout
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addLayout(formLayout);
    layout->addWidget(buttonBox);
    setLayout(layout);

    // Connections
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(this, SIGNAL(accepted()), this, SLOT(save()));

    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(QString("%1 - %2").arg(QString(NOM_APPLICATION),tr("Préférences")));
    setWindowModality(Qt::ApplicationModal);
}

PreferencesDialog::~PreferencesDialog()
{
    // QObject should do it right for us allready.
}

void
PreferencesDialog::show()
{
    load();
    QDialog::show();
}

void
PreferencesDialog::load()
{
#ifndef NULL_PLAYER
    m_gmMusicDir->setDirName(G_initialisation.dossierMusiquesMj);
    m_playerMusicDir->setDirName(G_initialisation.dossierMusiquesJoueur);
#endif
    m_picturesDir->setDirName(G_initialisation.dossierImages);
    m_mapsDir->setDirName(G_initialisation.dossierPlans);
    m_sessionsDir->setDirName(G_initialisation.dossierScenarii);
    m_notesDir->setDirName(G_initialisation.dossierNotes);
    m_chatsDir->setDirName(G_initialisation.dossierTchats);
}

void
PreferencesDialog::save() const
{
#ifndef NULL_PLAYER
    G_initialisation.dossierMusiquesMj = m_gmMusicDir->dirName();
    G_initialisation.dossierMusiquesJoueur = m_playerMusicDir->dirName();
#endif
    G_initialisation.dossierImages = m_picturesDir->dirName();
    G_initialisation.dossierPlans = m_mapsDir->dirName();
    G_initialisation.dossierScenarii = m_sessionsDir->dirName();
    G_initialisation.dossierNotes = m_notesDir->dirName();
    G_initialisation.dossierTchats = m_chatsDir->dirName();
}
