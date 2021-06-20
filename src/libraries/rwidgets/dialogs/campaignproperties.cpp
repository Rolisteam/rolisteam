/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "campaignproperties.h"
#include "ui_campaignproperties.h"

#include "delegates/checkboxdelegate.h"
#include "delegates/colordelegate.h"
#include "delegates/imagepathdelegateitem.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include <QHeaderView>

CampaignProperties::CampaignProperties(campaign::Campaign* capm, QWidget* parent)
    : QDialog(parent), ui(new Ui::CampaignProperties), m_campaign(capm)
{
    ui->setupUi(this);
    Q_ASSERT(m_campaign);

    setWindowTitle(tr("Campaign Properties"));

    ui->m_tableViewAlias->setModel(m_campaign->diceAliases());
    ui->m_stateView->setModel(m_campaign->stateModel());

    ui->m_nameEdit->setText(m_campaign->name());
    ui->m_rootDir->setPath(m_campaign->rootDirectory());
    ui->m_rootDir->setEnabled(false);
    // ui->m_currentTheme->setModel();
    // ui->m_currentTheme->setCurrentText();

    QHeaderView* horizontalHeader= ui->m_tableViewAlias->horizontalHeader();
    horizontalHeader->setSectionResizeMode(DiceAliasModel::PATTERN, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::COMMAND, QHeaderView::Stretch);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::METHOD, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(DiceAliasModel::COMMENT, QHeaderView::ResizeToContents);

    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::METHOD, new rwidgets::CheckBoxDelegate());
    ui->m_tableViewAlias->setItemDelegateForColumn(DiceAliasModel::DISABLE, new rwidgets::CheckBoxDelegate());
    /*connect(m_ctrl, &PreferencesController::currentThemeIndexChanged, this, [this]() {
        m_preferences->registerValue("currentThemeIndex", QVariant::fromValue(m_ctrl->currentThemeIndex()), true);
        updateTheme();
    });*/

    connect(ui->m_addDiceAliasAct, &QToolButton::clicked, m_campaign, &campaign::Campaign::addAlias);
    connect(ui->m_delDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_campaign->deleteAlias(ui->m_tableViewAlias->currentIndex()); });
    connect(ui->m_upDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveAlias(ui->m_tableViewAlias->currentIndex(), campaign::Campaign::Move::UP); });
    connect(ui->m_downDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveAlias(ui->m_tableViewAlias->currentIndex(), campaign::Campaign::Move::DOWN); });
    connect(ui->m_topDiceAliasAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveAlias(ui->m_tableViewAlias->currentIndex(), campaign::Campaign::Move::TOP); });
    connect(ui->m_bottomDiceAliasAct, &QToolButton::clicked, this, [this]() {
        m_campaign->moveAlias(ui->m_tableViewAlias->currentIndex(), campaign::Campaign::Move::BOTTOM);
    });
    // connect(ui->m_testPushButton, &QToolButton::clicked, this, &campaign::CampaignManager::testAliasCommand);

    // States
    connect(ui->m_addCharacterStateAct, &QToolButton::clicked, m_campaign, &campaign::Campaign::addState);
    connect(ui->m_delCharceterStateAct, &QToolButton::clicked, m_campaign,
            [this]() { m_campaign->deleteState(ui->m_stateView->currentIndex()); });
    connect(ui->m_upCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveState(ui->m_stateView->currentIndex(), campaign::Campaign::Move::UP); });

    connect(ui->m_downCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveState(ui->m_stateView->currentIndex(), campaign::Campaign::Move::DOWN); });
    connect(ui->m_topCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveState(ui->m_stateView->currentIndex(), campaign::Campaign::Move::TOP); });
    connect(ui->m_bottomCharacterStateAct, &QToolButton::clicked, this,
            [this]() { m_campaign->moveState(ui->m_stateView->currentIndex(), campaign::Campaign::Move::BOTTOM); });

    ui->m_stateView->setItemDelegateForColumn(1, new rwidgets::ColorDelegate(this));
    ui->m_stateView->setItemDelegateForColumn(
        2, new rwidgets::ImagePathDelegateItem(m_campaign->directory(campaign::Campaign::Place::STATE_ROOT), this));
}

CampaignProperties::~CampaignProperties()
{
    delete ui;
}

void CampaignProperties::setCurrentTab(Tab tab)
{
    QHash<Tab, int> indexes{{Tab::Properties, 2}, {Tab::Dice, 0}, {Tab::States, 1}};

    auto idx= indexes.value(tab);
    ui->m_tabwidget->setCurrentIndex(idx);
}
