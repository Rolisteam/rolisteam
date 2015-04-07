/*************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
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

#include "map/newemptymapdialog.h"
#include "diceparser/diceparser.h"

/*********************
 * PreferencesDialog *
 *********************/

PreferencesDialog::PreferencesDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f), ui(new Ui::PreferencesDialogBox())
{
    ui->setupUi(this);

    ui->m_defaultMapModeCombo->addItems(NewEmptyMapDialog::getPermissionData());
	m_aliasModel = new DiceAliasModel();
	m_diceParser = new DiceParser();
	m_aliasModel->setAliases(m_diceParser->getAliases());

	ui->m_tableViewAlias->setModel(m_aliasModel);
    QHeaderView* horizontalHeader = ui->m_tableViewAlias->horizontalHeader();
    horizontalHeader->setSectionResizeMode(DiceAliasModel::VALUE,QHeaderView::Stretch);

    m_preferences = PreferencesManager::getInstance();

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->m_startDiag,SIGNAL(clicked()),this,SLOT(performDiag()));
    //ui->m_fogColor->setTransparency(true);

	//aliases
	connect(ui->m_addDiceAliasAct,SIGNAL(clicked()),this,SLOT(addAlias()));
    // Misc
    setSizeGripEnabled(true);
    setWindowTitle(QString("%1 - %2").arg(m_preferences->value("Application_Name","rolisteam").toString(),tr("Preferences")));
    setWindowModality(Qt::ApplicationModal);
}

PreferencesDialog::~PreferencesDialog()
{
    // QObject should do it right for us already.
}

void PreferencesDialog::show()
{
    load();
    QDialog::show();
}

void PreferencesDialog::load()
{
    //Direcotry PATH
    ui->m_musicDirGM->setPath(m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString());
    ui->m_musicDirPlayer->setPath(m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString());
    ui->m_pictureDir->setPath(m_preferences->value("ImageDirectory",QDir::homePath()).toString());
    ui->m_mapDir->setPath(m_preferences->value("MapDirectory",QDir::homePath()).toString());
    ui->m_scenarioDir->setPath(m_preferences->value("SessionDirectory",QDir::homePath()).toString());
    ui->m_minuteDir->setPath(m_preferences->value("MinutesDirectory",QDir::homePath()).toString());
    ui->m_chatDir->setPath(m_preferences->value("ChatDirectory",QDir::homePath()).toString());
    ui->m_checkUpdate->setChecked(m_preferences->value("MainWindow_MustBeChecked",true).toBool());

    ////////////////////////
    //MAP
    ///////////////////////
    //Fog of WAR
    QColor fog=m_preferences->value("Fog_color",QColor(Qt::black)).value<QColor>();
    ui->m_opacitySlider->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_opacitySpin->setValue(m_preferences->value("Fog_opacity",fog.red()).toInt());
    ui->m_fogColor->setColor(m_preferences->value("Mask_color",QColor(Qt::darkMagenta)).value<QColor>());

    //Default Permission
    ui->m_defaultMapModeCombo->setCurrentIndex(m_preferences->value("defaultPermissionMap",0).toInt());
    ui->m_pictureAdjust->setChecked(m_preferences->value("PictureAdjust",true).toBool());


    //Background
    ui->m_bgColorPush->setColor(m_preferences->value("BackGroundColor",QColor(191,191,191)).value<QColor>());
    ui->m_backgroundImage->setPath(m_preferences->value("PathOfBackgroundImage",":/resources/icons/workspacebackground.bmp").toString());
    ui->m_backgroundImage->setMode(false);
    ui->m_backgroundImage->setFilter(tr("Images (*.png *.xpm *.jpg *.gif *.bmp)"));

    //DiceSystem
    int size = m_preferences->value("DiceAliasNumber",0).toInt();
    for(int i = 0; i < size ; ++i)
    {
        QString cmd = m_preferences->value(QString("DiceAlias_%1_command").arg(i),"").toString();
        QString value = m_preferences->value(QString("DiceAlias_%1_value").arg(i),"").toString();
        bool replace = m_preferences->value(QString("DiceAlias_%1_type").arg(i),true).toBool();

        DiceAlias* tmpAlias = new DiceAlias(cmd,value,replace);
        m_aliasModel->addAlias(tmpAlias);
    }

}

void PreferencesDialog::save() const
{
    m_preferences->registerValue("MusicDirectoryGM",ui->m_musicDirGM->path());
    m_preferences->registerValue("MusicDirectoryPlayer",ui->m_musicDirPlayer->path());
    m_preferences->registerValue("ImageDirectory",ui->m_pictureDir->path());
    m_preferences->registerValue("MapDirectory",ui->m_mapDir->path());
    m_preferences->registerValue("SessionDirectory",ui->m_scenarioDir->path());
    m_preferences->registerValue("MinutesDirectory",ui->m_minuteDir->path());
    m_preferences->registerValue("ChatDirectory",ui->m_chatDir->path());
    m_preferences->registerValue("MainWindow_MustBeChecked",ui->m_checkUpdate->isChecked());
    m_preferences->registerValue("defaultPermissionMap",ui->m_defaultMapModeCombo->currentIndex());

    QColor color;
    int opacity=ui->m_opacitySlider->value();
    color.setRgb(opacity,opacity,opacity);
    m_preferences->registerValue("Fog_color", color);

    m_preferences->registerValue("Mask_color", ui->m_fogColor->color());
    m_preferences->registerValue("PictureAdjust",ui->m_pictureAdjust->isChecked());

    //Background
    m_preferences->registerValue("PathOfBackgroundImage",ui->m_backgroundImage->path());
    m_preferences->registerValue("BackGroundColor",ui->m_bgColorPush->color());

    //DiceSystem
    QList<DiceAlias*>* aliasList = m_aliasModel->getAliases();
    m_preferences->registerValue("DiceAliasNumber",aliasList->size());
    for(int i = 0; i < aliasList->size() ; ++i)
    {
        DiceAlias* tmpAlias = aliasList->at(i);
        m_preferences->registerValue(QString("DiceAlias_%1_command").arg(i),tmpAlias->getCommand());
        m_preferences->registerValue(QString("DiceAlias_%1_value").arg(i),tmpAlias->getValue());
        m_preferences->registerValue(QString("DiceAlias_%1_type").arg(i),tmpAlias->isReplace());
    }




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
    ui->m_diagDisplay->append(tr("Font families:"));
    foreach (const QString &family, database.families())
    {
          ui->m_diagDisplay->append(family);
    }
    ui->m_diagDisplay->append(tr("End of Font families:"));
}
void PreferencesDialog::addAlias()
{
	m_aliasModel->appendAlias();
}

void PreferencesDialog::delAlias()
{

}
void PreferencesDialog::upAlias()
{

}

void PreferencesDialog::downAlias()
{

}

void PreferencesDialog::moveAliasToTop()
{

}

void PreferencesDialog::moveAliasToBottum()
{
}
