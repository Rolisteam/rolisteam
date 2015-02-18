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
#include <QPushButton>
#include <QImageReader>
#include <QFontDatabase>

#include "ui_preferencesdialogbox.h"

/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox())
{
    ui->setupUi(this);

    m_preferences = PreferencesManager::getInstance();

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->m_startDiag,SIGNAL(clicked()),this,SLOT(performDiag()));
    ui->m_fogColor->setTransparency(true);

    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(QString("%1 - %2").arg(m_preferences->value("Application_Name","rolisteam").toString(),tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);
}

PreferencesDialog::~PreferencesDialog()
{
    // QObject should do it right for us allready.
}

void PreferencesDialog::show()
{
    load();
    QDialog::show();
}

void PreferencesDialog::load()
{
    ui->m_musicDirGM->setDirName(m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString());
    ui->m_musicDirPlayer->setDirName(m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString());
    ui->m_pictureDir->setDirName(m_preferences->value("ImageDirectory",QDir::homePath()).toString());
    ui->m_mapDir->setDirName(m_preferences->value("MapDirectory",QDir::homePath()).toString());
    ui->m_scenarioDir->setDirName(m_preferences->value("SessionDirectory",QDir::homePath()).toString());
    ui->m_minuteDir->setDirName(m_preferences->value("MinutesDirectory",QDir::homePath()).toString());
    ui->m_chatDir->setDirName(m_preferences->value("ChatDirectory",QDir::homePath()).toString());
    ui->m_checkUpdate->setChecked(m_preferences->value("MainWindow_MustBeChecked",true).toBool());
    ui->m_fogColor->setColor(m_preferences->value("Fog_color",Qt::black).value<QColor>());
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust",true).toBool());
   // ui->m_transColor->setColor(m_preferences->value("TransparencyColor",QColor(200,200,200,120)).value<QColor>());
}

void PreferencesDialog::save() const
{

    m_preferences->registerValue("MusicDirectoryGM",ui->m_musicDirGM->dirName());
    m_preferences->registerValue("MusicDirectoryPlayer",ui->m_musicDirPlayer->dirName());
    m_preferences->registerValue("ImageDirectory",ui->m_pictureDir->dirName());
    m_preferences->registerValue("MapDirectory",ui->m_mapDir->dirName());
    m_preferences->registerValue("SessionDirectory",ui->m_scenarioDir->dirName());
    m_preferences->registerValue("MinutesDirectory",ui->m_minuteDir->dirName());
    m_preferences->registerValue("ChatDirectory",ui->m_chatDir->dirName());
    m_preferences->registerValue("MainWindow_MustBeChecked",ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("Fog_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust",ui->m_pictureAdjust->isChecked());
   // m_preferences->registerValue("TransparencyColor",ui->m_transColor->color());

}
void PreferencesDialog::performDiag()
{
    ui->m_diagDisplay->clear();
    
    QList<QByteArray> format(QImageReader::supportedImageFormats());
    QString chaine=tr("Image Format : %1 value %2");
    ui->m_diagDisplay->setText(tr("Supported Image Formats:"));
    for(int i=0 ; i < format.size() ; ++i)
    {
        ui->m_diagDisplay->append(chaine.arg(i).arg(QString(format.at(i))));
    }
    ui->m_diagDisplay->append(tr("End of Image Format"));

    QFontDatabase database;
    ui->m_diagDisplay->setText(tr("Font families:"));
    foreach (const QString &family, database.families())
    {
          ui->m_diagDisplay->append(family);
    }
    ui->m_diagDisplay->setText(tr("End of Font families:"));
}
