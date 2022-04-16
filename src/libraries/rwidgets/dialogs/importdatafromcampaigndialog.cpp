/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "importdatafromcampaigndialog.h"
#include "ui_importdatafromcampaigndialog.h"

#include "core/worker/fileserializer.h"

ImportDataFromCampaignDialog::ImportDataFromCampaignDialog(const QString& campaignRoot, QWidget* parent)
    : QDialog(parent), ui(new Ui::ImportDataFromCampaignDialog), m_root(campaignRoot)
{
    ui->setupUi(this);
}

ImportDataFromCampaignDialog::~ImportDataFromCampaignDialog()
{
    delete ui;
}

void ImportDataFromCampaignDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool ImportDataFromCampaignDialog::validSource() const
{
    return (m_root != source() && campaign::FileSerializer::isValidCampaignDirectory(source(), false));
}

const QString ImportDataFromCampaignDialog::source() const
{
    return ui->m_pathEdit->text();
}

void ImportDataFromCampaignDialog::setSource(const QString& newSource)
{
    ui->m_pathEdit->setText(newSource);

    if(validSource())
    {
        using cf= campaign::FileSerializer;
        using cc= Core::CampaignDataCategory;
        ui->m_antagonistListCb->setEnabled(cf::hasContent(source(), cc::AntagonistList));
        ui->m_audioPlayer1Cb->setEnabled(cf::hasContent(source(), cc::AudioPlayer1));
        ui->m_audioPlayer2Cb->setEnabled(cf::hasContent(source(), cc::AudioPlayer2));
        ui->m_audioPlayer3Cb->setEnabled(cf::hasContent(source(), cc::AudioPlayer3));
        ui->m_characterSheetCb->setEnabled(cf::hasContent(source(), cc::CharacterSheets));
        ui->m_characterStatesCb->setEnabled(cf::hasContent(source(), cc::CharacterStates));
        ui->m_diceCb->setEnabled(cf::hasContent(source(), cc::DiceAlias));
        ui->m_imageCb->setEnabled(cf::hasContent(source(), cc::Images));
        ui->m_mapCb->setEnabled(cf::hasContent(source(), cc::Maps));
        ui->m_mindmapCb->setEnabled(cf::hasContent(source(), cc::MindMaps));
        ui->m_noteCb->setEnabled(cf::hasContent(source(), cc::Notes));
        ui->m_themeCb->setEnabled(cf::hasContent(source(), cc::Themes));
        ui->m_pdfCb->setEnabled(cf::hasContent(source(), cc::PDFDoc));
        ui->m_weblinkCb->setEnabled(cf::hasContent(source(), cc::WebLink));
    }
}

const QVector<Core::CampaignDataCategory> ImportDataFromCampaignDialog::categoryList() const
{
    return m_categoryList;
}
