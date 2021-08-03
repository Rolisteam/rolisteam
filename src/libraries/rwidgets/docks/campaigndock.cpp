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
#include "model/mediamodel.h"
#include "preferences/preferencesmanager.h"

#include "ui_campaigndock.h"

namespace campaign
{
CampaignDock::CampaignDock(Campaign* ctrl, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui::CampaignDock)
    , m_campaign(ctrl)
    , m_model(new MediaModel(ctrl))
    , m_filteredModel(new MediaFilteredModel)
{
    m_ui->setupUi(this);
    setObjectName("CampaignDock");
    m_ui->m_view->setCampaign(ctrl);

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

    connect(this, &CampaignDock::campaignChanged, this, [this]() { m_model->setCampaign(m_campaign); });
    connect(m_ui->m_view, &CampaignView::openAs, this, &CampaignDock::openResource);
    connect(m_ui->m_view, &CampaignView::removeSelection, this, &CampaignDock::removeFile);

    m_filteredModel->setSourceModel(m_model.get());
    m_ui->m_view->setModel(m_filteredModel.get());

    connect(m_ui->m_patternEdit, &QLineEdit::textEdited, this,
            [this]() { m_filteredModel->setPattern(m_ui->m_patternEdit->text()); });

    connect(m_ui->m_typeCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this]() { m_filteredModel->setType(m_ui->m_typeCombobox->currentData().value<Core::MediaType>()); });

    m_ui->m_view->setDragEnabled(true);
    m_ui->m_view->setAcceptDrops(true);
    m_ui->m_view->setDropIndicatorShown(true);
    m_ui->m_view->setDefaultDropAction(Qt::MoveAction);
    m_ui->m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_ui->m_view->setAlternatingRowColors(true);
}
CampaignDock::~CampaignDock() {}

void CampaignDock::setCampaign(Campaign* campaign)
{
    if(m_campaign == campaign)
        return;
    m_campaign= campaign;
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

/*void SessionDock::addRessource(ResourcesNode* tp)
{
    if(nullptr == tp || nullptr == m_view)
        return;

    QModelIndex index= m_view->currentIndex();
    if(tp->getResourcesType() == ResourcesNode::Cleveruri)
    {
        auto cleveruri= dynamic_cast<CleverURI*>(tp);
        if(nullptr != cleveruri)
        {
            cleveruri->setListener(m_model);
        }
    }
    m_model->addResource(tp, index);
    emit sessionChanged(true);
}
void SessionDock::removeResource(CleverURI* uri)
{
    m_model->removeNode(uri);
}

void SessionDock::addChapter(QModelIndex& index)
{
    QString tmp= tr("Chapter %1").arg(m_model->rowCount(index) + 1);
    Chapter* chapter= new Chapter();
    chapter->setName(tmp);
    m_model->addResource(chapter, index);
}
    // QHeaderView* hHeader= m_ui->m_view->header(); // new QHeaderView(Qt::Vertical,this);
    hHeader->setSectionResizeMode(SessionItemModel::Name, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(SessionItemModel::LoadingMode, QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Displayed, QHeaderView::ResizeToContents);
    hHeader->setSectionResizeMode(SessionItemModel::Path, QHeaderView::ResizeToContents);
    m_ui->m_view->setHeader(hHeader);*/

// m_view->setColumnHidden(SessionItemModel::LoadingMode,true);
// m_ui->m_view->setColumnHidden(session::SessionItemModel::Path, true);

// connect(m_ui->m_view, &CampaignView::onDoubleClick, m_ctrl, &ContentController::openResources);
// connect(m_ui->m_view, &CampaignView::addChapter, m_ctrl, &ContentController::addChapter);
// connect(m_ui->m_view, &CampaignView::removeSelection, this,
//        [this]() { m_ctrl->removeSelectedItems(m_ui->m_view->getSelection()); });
// connect(m_model, &SessionItemModel::openResource, this, &SessionManager::openResource);

} // namespace campaign
