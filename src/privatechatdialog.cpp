/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
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


#include "privatechatdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QListView>
#include <QVBoxLayout>

#include "chat.h"
#include "persons.h"
#include "playersList.h"


/**************************
 * PrivateChatDialogModel *
 **************************/

PrivateChatDialogModel::PrivateChatDialogModel(const QSet<Player *> & set, QObject * parent)
    : PlayersListProxyModel(parent), m_set(set)
{
    m_set.insert(PlayersList::instance().localPlayer());
}

Qt::ItemFlags PrivateChatDialogModel::flags(const QModelIndex &index) const
{
    PlayersList & g_playersList = PlayersList::instance();
    Player * player = g_playersList.getPlayer(index);

    if (player == NULL)
        return Qt::NoItemFlags;

    if (player == g_playersList.localPlayer())
        return Qt::ItemIsEnabled;

    if (player->hasFeature("MultiChat"))
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

    return Qt::NoItemFlags;
}

QVariant PrivateChatDialogModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole)
    {
        return QVariant(m_set.contains(PlayersList::instance().getPlayer(index)));
    }

    return QAbstractProxyModel::data(index, role);
}

bool PrivateChatDialogModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && (index.flags() & Qt::ItemIsUserCheckable))
    {
        Player * player = PlayersList::instance().getPlayer(index);
        if (!m_set.remove(player))
            m_set.insert(player);
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

QSet<Player *> & PrivateChatDialogModel::playersSet()
{
    return m_set;
}

void PrivateChatDialogModel::setPlayersSet(const QSet<Player *> & set)
{
    m_set = set;
    m_set.insert(PlayersList::instance().localPlayer());
    emit dataChanged(createIndex(0, 0, PlayersList::NoParent),
            createIndex(PlayersList::instance().numPlayers() - 1, 0, PlayersList::NoParent));
}

/*********************
 * PrivateChatDialog *
 *********************/

PrivateChatDialog::PrivateChatDialog(QWidget * parent)
    : QDialog(parent), m_model(QSet<Player *>())
{
    m_name_w = new QLineEdit;

    QListView * listView = new QListView;
    listView->setModel(&m_model);

    QFormLayout * formLayout = new QFormLayout;
    formLayout->addRow(tr("&Nom : "), m_name_w);
    formLayout->addRow(tr("&Joueurs : "), listView);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    setSizeGripEnabled(true);
}

QSize PrivateChatDialog::sizeHint() const
{
    return minimumSizeHint() * 1.4;
}

int PrivateChatDialog::edit(PrivateChat * chat)
{
    if (chat == NULL)
        return QDialog::Rejected;

    m_name_w->setText(chat->name());
    m_model.setPlayersSet(chat->players());
    int ret = exec();

    if (ret == QDialog::Accepted)
        chat->set(m_name_w->text(), m_model.playersSet());

    return ret;
}
