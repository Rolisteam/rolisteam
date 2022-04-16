/***************************************************************************
 *   Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "campaigndock.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QTreeView>

#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "model/mediamodel.h"

#include "ui_campaigndock.h"

namespace campaign
{
CampaignDock::CampaignDock(CampaignEditor* ctrl, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui::CampaignDock)
    , m_campaignEditor(ctrl)
    , m_model(new MediaModel(ctrl->campaign()))
    , m_filteredModel(new MediaFilteredModel)
{
    m_ui->setupUi(this);
    setObjectName("CampaignDock");
    m_ui->m_view->setCampaign(ctrl->campaign());

    setWindowTitle(tr("Campaign Content"));

    m_ui->m_typeCombobox->addItem(tr("All"), QVariant::fromValue(Core::MediaType::Unknown));
    m_ui->m_typeCombobox->addItem(tr("AudioFile"), QVariant::fromValue(Core::MediaType::AudioFile));
    m_ui->m_typeCombobox->addItem(tr("CharacterSheet"), QVariant::fromValue(Core::MediaType::CharacterSheetFile));
    m_ui->m_typeCombobox->addItem(tr("Image"), QVariant::fromValue(Core::MediaType::ImageFile));
    m_ui->m_typeCombobox->addItem(tr("Map"), QVariant::fromValue(Core::MediaType::MapFile));
    m_ui->m_typeCombobox->addItem(tr("Mindmap"), QVariant::fromValue(Core::MediaType::MindmapFile));
    m_ui->m_typeCombobox->addItem(tr("PDF"), QVariant::fromValue(Core::MediaType::PdfFile));
    m_ui->m_typeCombobox->addItem(tr("PlayList"), QVariant::fromValue(Core::MediaType::PlayListFile));
    m_ui->m_typeCombobox->addItem(tr("Text"), QVariant::fromValue(Core::MediaType::TextFile));
    m_ui->m_typeCombobox->addItem(tr("Token"), QVariant::fromValue(Core::MediaType::TokenFile));
    m_ui->m_typeCombobox->addItem(tr("WebPage"), QVariant::fromValue(Core::MediaType::WebpageFile));

    connect(this, &CampaignDock::campaignChanged, this,
            [this]() { m_model->setCampaign(m_campaignEditor->campaign()); });
    connect(m_ui->m_view, &CampaignView::openAs, this, &CampaignDock::openResource);
    connect(m_ui->m_view, &CampaignView::removeSelection, this, &CampaignDock::removeFile);

    m_filteredModel->setSourceModel(m_model.get());
    m_ui->m_view->setModel(m_filteredModel.get());

    connect(m_ui->m_patternEdit, &QLineEdit::textEdited, this,
            [this]() { m_filteredModel->setPattern(m_ui->m_patternEdit->text()); });

    connect(m_ui->m_typeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this]() { m_filteredModel->setType(m_ui->m_typeCombobox->currentData().value<Core::MediaType>()); });

    connect(m_model.get(), &MediaModel::performCommand, m_campaignEditor, &CampaignEditor::doCommand);

    m_ui->m_view->setDragEnabled(true);
    m_ui->m_view->setAcceptDrops(true);
    m_ui->m_view->setDropIndicatorShown(true);
    m_ui->m_view->setDefaultDropAction(Qt::MoveAction);
    m_ui->m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_ui->m_view->setAlternatingRowColors(true);
}
CampaignDock::~CampaignDock() {}

void CampaignDock::setCampaign(CampaignEditor* campaign)
{
    if(m_campaignEditor == campaign)
        return;
    m_campaignEditor= campaign;
    emit campaignChanged();
}
void CampaignDock::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    if(event->isAccepted())
    {
        // emit changeVisibility(false);
    }
}
} // namespace campaign
