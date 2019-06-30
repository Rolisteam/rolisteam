/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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

#include <QDebug>
#include <QMenu>
#include <QVBoxLayout>

#include "playersListWidget.h"
#include "userlistview.h"

#include "data/character.h"
#include "data/person.h"
#include "data/player.h"
#include "delegate.h"
#include "map/map.h"
#include "userlist/playersList.h"
#include "widgets/persondialog.h"

/**************************
 * PlayersListWidgetModel *
 **************************/

PlayersListWidgetModel::PlayersListWidgetModel(QObject* parent) : QSortFilterProxyModel(parent), m_map(nullptr)
{
    setSourceModel(PlayersList::instance());
}

Qt::ItemFlags PlayersListWidgetModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags ret= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if(m_map != nullptr && isCheckable(index))
        ret|= Qt::ItemIsUserCheckable;

    auto realCurrent= mapToSource(index);
    auto isLocal    = realCurrent.data(PlayersList::IsLocal).toBool();
    auto localIsGM  = realCurrent.data(PlayersList::LocalIsGM).toBool();

    if(isLocal || localIsGM)
        ret|= Qt::ItemIsEditable;
    return ret;
}

QVariant PlayersListWidgetModel::data(const QModelIndex& index, int role) const
{
    if(isCheckable(index) && role == Qt::CheckStateRole)
    {
        if(m_map == nullptr)
            return false;
        auto realIndex= mapToSource(index);
        Person* person= static_cast<Person*>(realIndex.internalPointer());
        return m_map->isVisiblePc(person->getUuid());
    }
    return QAbstractProxyModel::data(index, role);
}

bool PlayersListWidgetModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto realIndex= mapToSource(index);
    Person* person= static_cast<Person*>(realIndex.internalPointer());
    auto isLocal  = realIndex.data(PlayersList::IsLocal).toBool();
    auto localIsGM= realIndex.data(PlayersList::LocalIsGM).toBool();

    if(person != nullptr && (isLocal || localIsGM))
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            person->setName(value.toString());
            return true;
        case Qt::DecorationRole:
            if(value.type() == QVariant::Color)
            {
                person->setColor(value.value<QColor>());
                return true;
            }
        }

    if(role == Qt::CheckStateRole && isCheckable(index))
    {
        // isCheckable ensures person and m_map is not nullptr and person is a character.
        m_map->toggleCharacterView(static_cast<Character*>(person));
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

void PlayersListWidgetModel::setCurrentMap(Map* map)
{
    if(map == m_map)
        return;
    beginResetModel();
    m_map= map;
    endResetModel();
}

bool PlayersListWidgetModel::isCheckable(const QModelIndex& index) const
{
    if(!index.isValid() || m_map == nullptr || !index.parent().isValid())
        return false;

    auto realIndex= mapToSource(index);
    auto isLocal  = realIndex.data(PlayersList::IsLocal).toBool();
    auto localIsGM= realIndex.data(PlayersList::LocalIsGM).toBool();
    return isLocal || localIsGM;
}

/********************
 * PlayerListWidget *
 ********************/

PlayersListWidget::PlayersListWidget(QWidget* parent) : QDockWidget(parent)
{
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setWindowTitle(tr("Player List"));
    setObjectName("PlayersListWidget");

    setUI();
}

PlayersListWidget::~PlayersListWidget() {}

PlayersListWidgetModel* PlayersListWidget::model() const
{
    return m_model.get();
}

void PlayersListWidget::editIndex(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    PlayersList* playersList= PlayersList::instance();
    Person* person          = playersList->getPerson(index);
    if(!playersList->isLocal(person))
        return;

    if(m_personDialog->edit(tr("Edit"), person->name(), person->getColor(), m_personDialog->getAvatarUri())
       == QDialog::Accepted)
    {
        // person->setAvatar();
        playersList->changeLocalPerson(person, m_personDialog->getName(), m_personDialog->getColor(),
                                       QImage(m_personDialog->getAvatarUri()));
    }
}

void PlayersListWidget::createLocalCharacter()
{
    PlayersList* playersList= PlayersList::instance();
    Player* localPlayer     = playersList->getLocalPlayer();
    if(nullptr == localPlayer)
    {
        return;
    }

    if(m_personDialog->edit(tr("New Character"), tr("New Character"), localPlayer->getColor(), "") == QDialog::Accepted)
    {
        Character* tmp= new Character(m_personDialog->getName(), m_personDialog->getColor());
        tmp->setNpc(localPlayer->isGM());
        tmp->setAvatar(QImage(m_personDialog->getAvatarUri()));
        playersList->addLocalCharacter(tmp);
    }
}

void PlayersListWidget::selectAnotherPerson(const QModelIndex& current)
{
    auto playersList= PlayersList::instance();
    auto orgin      = m_model->mapToSource(current);
    auto person     = static_cast<Person*>(orgin.internalPointer());
    m_delButton->setEnabled(current.isValid() && current.parent().isValid() && playersList->isLocal(person));
}

void PlayersListWidget::deleteSelected()
{
    PlayersList* playersList= PlayersList::instance();
    QModelIndex current     = m_selectionModel->currentIndex();
    auto orgin              = m_model->mapToSource(current);
    auto person             = static_cast<Person*>(orgin.internalPointer());
    if(current.isValid() && current.parent().isValid() && playersList->isLocal(person))
    {
        playersList->delLocalCharacter(current.row());
        m_delButton->setEnabled(false);
    }
}
void PlayersListWidget::updateUi(bool isGM)
{
    if(isGM)
    {
        m_addPlayerButton->setText(tr("Add %1").arg("NPC"));
        m_delButton->setText(tr("Remove %1").arg("NPC"));
    }
    m_addPlayerButton->setEnabled(true);
}

void PlayersListWidget::setUI()
{
    // Central Widget
    QWidget* centralWidget= new QWidget(this);

    // PlayersListView
    m_playersListView= new UserListView(); //= new PlayersListView(centralWidget);
    connect(m_playersListView, &UserListView::runDiceForCharacter, this, &PlayersListWidget::runDiceForCharacter);
    m_model.reset(new PlayersListWidgetModel);
    m_playersListView->setPlayersListModel(m_model.get());
    m_selectionModel= m_playersListView->selectionModel();
    m_playersListView->setHeaderHidden(true);

    // Add PC button
    Player* tmp = PlayersList::instance()->getLocalPlayer();
    QString what= tr("PC");
    if(nullptr != tmp)
    {
        what= (tmp->isGM() ? tr("NPC") : tr("PC"));
    }

    m_addPlayerButton= new QPushButton(tr("Add a %1").arg(what), centralWidget);
    m_addPlayerButton->setEnabled(false);

    // Del PJ buttun
    m_delButton= new QPushButton(tr("Remove %1").arg(what), centralWidget);
    m_delButton->setEnabled(false);

    // Button layout
    QHBoxLayout* buttonLayout= new QHBoxLayout;
    buttonLayout->addWidget(m_addPlayerButton);
    buttonLayout->addWidget(m_delButton);

    // Layout
    QVBoxLayout* layout= new QVBoxLayout;
    layout->setContentsMargins(0, 0, 3, 3);
    layout->addWidget(m_playersListView);
    layout->addLayout(buttonLayout);
    centralWidget->setLayout(layout);
    setWidget(centralWidget);

    // Actions
    connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this,
            SLOT(selectAnotherPerson(QModelIndex)));
    connect(m_model.get(), SIGNAL(rowsRemoved(QModelIndex, int, int)), m_playersListView, SLOT(clearSelection()));
    connect(m_addPlayerButton, SIGNAL(clicked()), this, SLOT(createLocalCharacter()));
    connect(m_delButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));

    // Dialog
    m_personDialog= new PersonDialog(this);
}
