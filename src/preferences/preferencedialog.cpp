/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include <QDebug>
#include <QSettings>
#include <QImageWriter>
#include <QFileDialog>
#include <QImageReader>




#ifdef HAVE_PHONON
#include <Phonon>
#include <phonon/BackendCapabilities>
#include <phonon/ObjectDescription>
#endif

#include "preferencedialog.h"
#include "ui_preferencedialog.h"
#include "preferencesmanager.h"
#include "theme.h"
#include "themelistmodel.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    m_options = PreferencesManager::getInstance();

    m_listModel = new ThemeListModel();
    m_currentThemeIndex = 0;
    //init value:
   // initValues();
    addDefaultTheme();

    connect(ui->m_wsBgBrowserButton,SIGNAL(clicked()),this,SLOT(changeBackgroundImage()));
    connect(ui->m_wsBgColorButton,SIGNAL(colorChanged(QColor)),this,SLOT(modifiedSettings()));
    connect(ui->m_addTheme,SIGNAL(clicked()),this,SLOT(addTheme()));
    connect(ui->m_removeTheme,SIGNAL(clicked()),this,SLOT(removeSelectedTheme()));
    connect(ui->m_testStarter,SIGNAL(pressed()),this,SLOT(performCapabilityTests()));

    ui->m_themeList->setModel(m_listModel);

    connect(ui->m_themeList,SIGNAL(pressed(QModelIndex)),this,SLOT(currentChanged()));

    connect(ui->m_buttonbox,SIGNAL(clicked(QAbstractButton * )),this,SLOT(applyAllChanges(QAbstractButton * )));

    ui->m_themeList->setCurrentIndex(m_listModel->index(m_currentThemeIndex));

}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void PreferenceDialog::initValues()
{
     //qDebug() << "Init Value";
    // Look and feel panel
    ui->m_wsBgPathLine->setText(m_options->value("worspace/background/image",":/resources/icons/fond workspace macos.bmp").toString());
    ui->m_wsBgColorButton->setColor(m_options->value("worspace/background/color",QColor(191,191,191)).value<QColor>());



    //General Panel
    ui->m_genUpdateCheck->setChecked(m_options->value("mainwindow/network/checkupdate",true).toBool());

    ui->m_dataChooser->setPath(m_options->value("DataDirectory",QDir::homePath()).toString());
    ui->m_dataChooser->activeDirectory(true);

    ui->m_imageChooser->setPath(m_options->value("ImageDirectory",QDir::homePath()).toString());
    ui->m_imageChooser->activeDirectory(true);

    ui->m_mapsChooser->setPath(m_options->value("MapDirectory",QDir::homePath()).toString());
    ui->m_mapsChooser->activeDirectory(true);

    ui->m_scriptChooser->setPath(m_options->value("ScriptDirectory",QDir::homePath()).toString());
    ui->m_scriptChooser->activeDirectory(true);

    ui->m_minutesChooser->setPath(m_options->value("MinutesDirectory",QDir::homePath()).toString());
    ui->m_minutesChooser->activeDirectory(true);

    ui->m_chatChooser->setPath(m_options->value("TchatDirectory",QDir::homePath()).toString());
    ui->m_chatChooser->activeDirectory(true);

    ui->m_musicChooser->setPath(m_options->value("MusicDirectory",QDir::homePath()).toString());
    ui->m_musicChooser->activeDirectory(true);

}
void PreferenceDialog::resetValues()
{
    ui->m_wsBgPathLine->setText(":/resources/icons/fond workspace macos.bmp");
    ui->m_wsBgColorButton->setColor(QColor(191,191,191));
    ui->m_genUpdateCheck->setChecked(true);

    ui->m_dataChooser->setPath(QDir::homePath());
    ui->m_scriptChooser->setPath(QDir::homePath());
    ui->m_mapsChooser->setPath(QDir::homePath());
    ui->m_minutesChooser->setPath(QDir::homePath());
    ui->m_imageChooser->setPath(QDir::homePath());
    ui->m_chatChooser->setPath(QDir::homePath());
    ui->m_musicChooser->setPath(QDir::homePath());
}
void PreferenceDialog::changeBackgroundImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Rolisteam Background"),".",tr(
        "Supported files (*.jpg *.png *.gif *.svg *.bmp)"));

    if(!fileName.isEmpty())
    {
        ui->m_wsBgPathLine->setText(fileName);
        m_current.setBackgroundImage(fileName);
        modifiedSettings();
    }
}
void PreferenceDialog::modifiedSettings()
{
    setWindowModified(true);
}
void PreferenceDialog::applyAllChanges(QAbstractButton * button)
{
    if(QDialogButtonBox::ApplyRole==ui->m_buttonbox->buttonRole(button))
    {
        //Theme
        /// @todo don't forget to set the new value into the selected theme.
        m_current.setBackgroundImage(ui->m_wsBgPathLine->text());
        m_options->registerValue("worspace/background/image",m_current.backgroundImage());
        m_current.setBackgroundColor(ui->m_wsBgColorButton->color());
        m_options->registerValue("worspace/background/color",m_current.backgroundColor());




        // Others values
        m_options->registerValue("mainwindow/network/checkupdate",ui->m_genUpdateCheck->isChecked());



        m_options->registerValue("DataDirectory",ui->m_dataChooser->getPath());
        m_options->registerValue("ImageDirectory",ui->m_dataChooser->getPath());
        m_options->registerValue("MapDirectory",ui->m_mapsChooser->getPath());
        m_options->registerValue("ScriptDirectory",ui->m_scriptChooser->getPath());
        m_options->registerValue("MinutesDirectory",ui->m_minutesChooser->getPath());
        m_options->registerValue("TchatDirectory",ui->m_chatChooser->getPath());
        m_options->registerValue("MusicDirectory",ui->m_musicChooser->getPath());






        emit preferencesChanged();
    }
    setWindowModified(false);
}
void PreferenceDialog::addDefaultTheme()
{
    m_current.setName(tr("Default"));
    m_current.setBackgroundColor(m_options->value("worspace/background/color",QColor(191,191,191)).value<QColor>());
    m_current.setBackgroundImage(m_options->value("worspace/background/image",":/resources/icones/fond workspace macos.bmp").toString());
    m_listModel->addTheme(m_current);
}

void PreferenceDialog::addTheme()
{
    Theme tmp;
    tmp.setName(tr("Theme %1").arg(m_listModel->rowCount(QModelIndex())));
    m_listModel->addTheme(tmp);
}
void PreferenceDialog::removeSelectedTheme()
{
    m_listModel->removeSelectedTheme(ui->m_themeList->currentIndex().row());
}
void PreferenceDialog::currentChanged()
{
    m_current=m_listModel->getTheme(ui->m_themeList->currentIndex().row());
    m_currentThemeIndex = ui->m_themeList->currentIndex().row();
    refreshDialogWidgets();
}
void PreferenceDialog::refreshDialogWidgets()
{
     ui->m_wsBgPathLine->setText(m_current.backgroundImage());
     ui->m_wsBgColorButton->setColor(m_current.backgroundColor());
}

void PreferenceDialog::readSettings(QSettings & settings)
{
    //QSettings settings("rolisteam", "rolisteam/currentTheme");
    settings.beginGroup("rolisteam/currentTheme");
    //qDebug() << "read setting in preference dialog";
    m_currentThemeIndex =  settings.value("currentTheme",m_currentThemeIndex).toInt();
    m_listModel->readSettings();

    ui->m_themeList->update();
    settings.endGroup();
}
void PreferenceDialog::writeSettings(QSettings & settings)
{
    settings.beginGroup("rolisteam/currentTheme");
   // qDebug() << "write setting in preference dialog";
    settings.setValue("currentTheme",m_currentThemeIndex);
    m_listModel->writeSettings();
    settings.endGroup();
}
void PreferenceDialog::performCapabilityTests()
{
    ui->m_testResult->clear();
    ui->m_testResult->append(tr("<h1>Capability Test</h1>"));
    ui->m_testResult->append(tr("<p>Informations diplayed here are relevant to your system. Please provide them if you are getting any problem with %1 - version %2</p>"));
    imageFormatTest();
    fontTest();
    phononTest();
}
void PreferenceDialog::imageFormatTest()
{
        ui->m_testResult->append(tr("<h2>Image formats Test</h2>"));
        QList<QByteArray> list = QImageReader::supportedImageFormats();
        QString result;
        result=tr("<h3>Reader</h3>");
        foreach(QByteArray format, list)
        {
            result.append(tr("<span style=\"color:#F00\">%1</span> format is supported in reading<br/>").arg(QString(format)));

        }
        result+=tr("<h3>Writer</h3>");
        list = QImageWriter::supportedImageFormats();
        foreach(QByteArray format, list)
        {
            result.append(tr("<span style=\"color:#F00\">%1</span> format is supported in writing<br/>").arg(QString(format)));
        }
        ui->m_testResult->append(result);
}

void PreferenceDialog::fontTest()
{
    ui->m_testResult->append(tr("<h2>Fonts Test</h2>"));
    QString result;
    QFontDatabase fdb;
    foreach(const QString font,fdb.families())
    {
        result.append(tr("<span style=\"color:#F00\">%1</span> is installed on your system<br/>").arg(QString(font)));
    }
    ui->m_testResult->append(result);
}

void PreferenceDialog::phononTest()
{
#ifdef HAVE_PHONON
   ui->m_testResult->append(tr("<h2>Multimedia Test</h2>"));

   QString result;
   result+=tr("<h3>Effect</h3>");
   QList<Phonon::EffectDescription> list1 = Phonon::BackendCapabilities::availableAudioEffects();
   foreach(Phonon::EffectDescription format, list1)
   {
       result.append(tr("<span style=\"color:#F00\">%1</span>%2<br/>").arg(QString(format.name()).arg(format.description())));
   }
   result+=tr("<h3>Audio Output Devices</h3>");
   QList<Phonon::AudioOutputDevice > list2 = Phonon::BackendCapabilities::availableAudioOutputDevices();
   foreach(Phonon::AudioOutputDevice  device, list2)
   {
       result.append(tr("<span style=\"color:#F00\">%1</span>%2<br/>").arg(QString(device.name()).arg(device.description())));
   }

   result+=tr("<h3>Audio Output Devices</h3>");
   QStringList list3 = Phonon::BackendCapabilities::availableMimeTypes ();
   foreach(QString type, list3)
   {
       result.append(tr("<span style=\"color:#F00\">%1</span> is supported<br/>").arg(QString(type)));
   }


   ui->m_testResult->append(result);
#endif

}
