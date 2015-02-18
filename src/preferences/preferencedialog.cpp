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

#include "preferencedialog.h"
#include "ui_preferencedialog.h"
#include "preferencesmanager.h"
#include "theme.h"
#include <QFileDialog>
#include "themelistmodel.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    m_options = PreferencesManager::getInstance();

    m_listModel = new ThemeListModel();

    //init value:
    initValues();

    connect(ui->m_wsBgBrowserButton,SIGNAL(clicked()),this,SLOT(changeBackgroundImage()));
    connect(ui->m_wsBgColorButton,SIGNAL(colorChanged(QColor)),this,SLOT(modifiedSettings()));
    connect(ui->m_addTheme,SIGNAL(clicked()),m_listModel,SLOT(addTheme()));
    connect(ui->m_removeTheme,SIGNAL(clicked()),m_listModel,SLOT(removeSelectedTheme()));

    ui->m_themeList->setModel( m_listModel);


    connect(ui->m_buttonbox,SIGNAL(clicked(QAbstractButton * )),this,SLOT(applyAllChanges(QAbstractButton * )));
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
    ui->m_wsBgPathLine->setText(m_options->value("worspace/background/image",":/resources/icones/fond workspace macos.bmp").toString());
    ui->m_wsBgColorButton->setColor(m_options->value("worspace/background/color",QColor(191,191,191)).value<QColor>());
}
void PreferenceDialog::resetValues()
{
    ui->m_wsBgPathLine->setText(":/resources/icones/fond workspace macos.bmp");
    ui->m_wsBgColorButton->setColor(QColor(191,191,191));

}
void PreferenceDialog::changeBackgroundImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Rolisteam Background"),".",tr(
        "Supported files (*.jpg *.png *.gif *.svg)"));

    if(!fileName.isEmpty())
    {
        ui->m_wsBgPathLine->setText(fileName);
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
        m_options->registerValue("worspace/background/image",ui->m_wsBgPathLine->text());
        m_options->registerValue("worspace/background/color",ui->m_wsBgColorButton->color());
        emit preferencesChanged();

    }
    setWindowModified(false);
}



