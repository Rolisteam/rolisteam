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


#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>


#include "playersListWidget.h"
#include "userlistview.h"

#include "map/map.h"
#include "delegate.h"
#include "data/person.h"
#include "data/character.h"
#include "data/player.h"
#include "persondialog.h"
#include "userlist/playersList.h"



/**************************
 * PlayersListWidgetModel *
 **************************/

PlayersListWidgetModel::PlayersListWidgetModel(QObject * parent)
    : PlayersListProxyModel(parent), m_map(NULL)
{
}

Qt::ItemFlags PlayersListWidgetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags ret = Qt::ItemIsEnabled;
    
    if (index.parent().isValid())
        ret |= Qt::ItemIsSelectable;

    if (isCheckable(index))
        ret |= Qt::ItemIsUserCheckable;
    
    PlayersList* g_playersList = PlayersList::instance();
    Person * person = g_playersList->getPerson(index);
    if (g_playersList->isLocal(person))
        ret |= Qt::ItemIsEditable;

    return ret;
}

QVariant PlayersListWidgetModel::data(const QModelIndex &index, int role) const
{
    if (isCheckable(index) && role == Qt::CheckStateRole)
    {
        return QVariant(m_map->isVisiblePc(PlayersList::instance()->getPerson(index)->getUuid()));
    }
    return QAbstractProxyModel::data(index, role);
}

bool PlayersListWidgetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "super PlayersListWidgetModel::setData";
    PlayersList* g_playersList = PlayersList::instance();
    Person * person = g_playersList->getPerson(index);

    if (person != NULL && g_playersList->isLocal(person))
        switch (role)
        {
            case Qt::EditRole:
            case Qt::DisplayRole:
                g_playersList->setLocalPersonName(person, value.toString());
                return true;
            case Qt::DecorationRole:
                if (value.type() == QVariant::Color)
                {
                    g_playersList->setLocalPersonColor(person, value.toString());
                    return true;
                }
        }

    if (role == Qt::CheckStateRole && isCheckable(index))
    {
        // isCheckable ensures person and m_map is not NULL and person is a character.
        m_map->toggleCharacterView(static_cast<Character *>(person));
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

void PlayersListWidgetModel::changeMap(Map * map)
{
    if (map == m_map)
        return;

    m_map = map;

    // We need to tell which rows should be updated
    PlayersList* playersList = PlayersList::instance();
    QModelIndex begin;
    QModelIndex end;
    int i;
    int max = 0;
    if(NULL!=playersList->getLocalPlayer())
    {
        max = (playersList->getLocalPlayer()->isGM() ? playersList->numPlayers() : 1);
    }

    for (i = 0 ; i < max ; i++)
    {
        Player * player = playersList->getPlayer(i);
        int nbCharacters = player->getCharactersCount();

        if (nbCharacters > 0)
        {
            begin = createIndex(0, 0, i);
            end   = createIndex(nbCharacters, 0, i);
            break;
        }
    }
    for (; i < max ; i++)
    {
        Player * player = playersList->getPlayer(i);
        int nbCharacters = player->getCharactersCount();

        if (nbCharacters > 0)
            end   = createIndex(nbCharacters, 0, i);
    }

    if (begin.isValid() && end.isValid())
        emit dataChanged(begin, end);
}

bool PlayersListWidgetModel::isCheckable(const QModelIndex &index) const
{
    if (!index.isValid() || m_map == NULL)
        return false;

    PlayersList * playersList = PlayersList::instance();

    Person * person = playersList->getPerson(index);
    if (person == NULL)
        return false;

    Player* localPlayer = playersList->getLocalPlayer();

    return ((person->parent() == localPlayer) ||
            (localPlayer->isGM() && index.parent().isValid()));
}


/*******************
 * PlayersListView *
 *******************/

/*PlayersListView::PlayersListView(QWidget * parent)
    : QTreeView(parent)
{
    static Delegate delegate;
    setItemDelegate(&delegate);

    m_avatarAct = new QAction(tr("Set avatar…"),this);
}

PlayersListView::~PlayersListView()
{
}

void PlayersListView::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPoint mPos = event->pos();
    QModelIndex index = indexAt(mPos);
    mPos -= visualRect(index).topLeft();

    Delegate * delegate = static_cast<Delegate *>(itemDelegate());
    int role = delegate->roleAt(viewOptions(), index, mPos);
    if (role == Qt::DecorationRole)
    {
        QVariant value = index.data(role);
        if (value.type() == QVariant::Color)
        {
            QColor color = qvariant_cast<QColor>(value);
            static QColorDialog colorDialog;
            colorDialog.setCurrentColor(color);
            if (colorDialog.exec() == QDialog::Accepted)
            {
                model()->setData(index, QVariant(colorDialog.currentColor()), role);
            }
            return;
        }
    }

    QTreeView::mouseDoubleClickEvent(event);
}
void PlayersListView::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    menu.addAction(m_avatarAct);


    menu.exec(event->globalPos());
}*/

/********************
 * PlayerListWidget *
 ********************/

PlayersListWidget::PlayersListWidget(QWidget * parent)
    : QDockWidget(parent)
{
    setAllowedAreas(Qt::AllDockWidgetAreas);
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setWindowTitle(tr("Player List"));
    setObjectName("PlayersListWidget");

    setUI();
}

PlayersListWidget::~PlayersListWidget()
{
}

PlayersListWidgetModel * PlayersListWidget::model() const
{
    return m_model;
}

void PlayersListWidget::editIndex(const QModelIndex & index)
{
    if (!index.isValid())
        return;

    PlayersList* g_playersList = PlayersList::instance();
    Person * person = g_playersList->getPerson(index);
    if (!g_playersList->isLocal(person))
        return;

    if (m_personDialog->edit(tr("Edit"), person->getName(), person->getColor()) == QDialog::Accepted)
    {
        g_playersList->changeLocalPerson(person, m_personDialog->getName(), m_personDialog->getColor());
    }
}

void PlayersListWidget::createLocalCharacter()
{
    PlayersList* g_playersList = PlayersList::instance();
    Player * localPlayer = g_playersList->getLocalPlayer();

    if (m_personDialog->edit(tr("New Character"), tr("New Character"), localPlayer->getColor()) == QDialog::Accepted)
    {
        g_playersList->addLocalCharacter(new Character(m_personDialog->getName(), m_personDialog->getColor()));
    }
}


void PlayersListWidget::selectAnotherPerson(const QModelIndex & current)
{
    PlayersList* g_playersList = PlayersList::instance();
    m_delButton->setEnabled(current.isValid() && current.parent().isValid() &&
            g_playersList->isLocal(g_playersList->getPerson(current)));
}


void PlayersListWidget::deleteSelected()
{
    PlayersList* g_playersList = PlayersList::instance();
    QModelIndex current = m_selectionModel->currentIndex();
    if (current.isValid() && current.parent().isValid() &&
            g_playersList->isLocal(g_playersList->getPerson(current)))
    {
        g_playersList->delLocalCharacter(current.row());
        m_delButton->setEnabled(false);
    }
}

void PlayersListWidget::setUI()
{
    // Central Widget
    QWidget * centralWidget = new QWidget(this);

    // PlayersListView
    QTreeView * playersListView  = new UserListView();//= new PlayersListView(centralWidget);
    m_model = new PlayersListWidgetModel;
    playersListView->setModel(m_model);
    m_selectionModel = playersListView->selectionModel();
    playersListView->setHeaderHidden(true);
  //  playersListView->setIconSize(QSize(64,64));

    // Add PJ button
    Player* tmp = PlayersList::instance()->getLocalPlayer();
    //PlayersList::instance()->localPlayer()->isGM()
    QString what;
    if(NULL!=tmp)
    {
        what = (tmp->isGM() ? tr("NPC") : tr("PC"));
    }
    else
    {
        what = tr("PC");
    }

    QPushButton * addPlayerButton = new QPushButton(tr("Add a %1").arg(what), centralWidget);

    // Del PJ buttun
    m_delButton = new QPushButton(tr("Remove %1").arg(what), centralWidget);
    m_delButton->setEnabled(false);

    // Button layout
    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addPlayerButton);
    buttonLayout->addWidget(m_delButton);

    // Layout
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 3, 3);
    layout->addWidget(playersListView);
    layout->addLayout(buttonLayout);
    centralWidget->setLayout(layout);
    setWidget(centralWidget);

    // Actions
    connect(m_selectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(selectAnotherPerson(const QModelIndex &)));
    connect(m_model, SIGNAL(rowsRemoved( const QModelIndex &, int, int)),
            playersListView, SLOT(clearSelection()));
    connect(addPlayerButton, SIGNAL(clicked()), this, SLOT(createLocalCharacter()));
    connect(m_delButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));

    // Dialog
    m_personDialog = new PersonDialog(this);
}
