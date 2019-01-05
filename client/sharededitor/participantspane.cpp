/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QTime>
#include <QHostAddress>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "participantspane.h"
#include "ui_participantspane.h"

#include "utilities.h"
#include "enu.h"
#include "data/player.h"

/////////////////////////////////////////////////
///
/// ParticipantsModel Code.
///
/////////////////////////////////////////////////

ParticipantsModel::ParticipantsModel(PlayersList* m_playerList)
{

    m_data.append(&m_readWrite);
    m_data.append(&m_readOnly);
    m_data.append(&m_hidden);

    m_permissionGroup << tr("Read Write") << tr("Read Only") << tr("Hidden");

    for(int  i = 0; i < m_playerList->getPlayerCount() ; ++i)
    {
        Player* player = m_playerList->getPlayer(i);
        m_hidden.append(player);
    }
}

int ParticipantsModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        return m_data.size();
    }
    else if(!parent.parent().isValid())
    {
        QList<Player*>* list = m_data.at(parent.row());
        return list->size();
    }
    else
    {
        return 0;
    }
}

int ParticipantsModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ParticipantsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {
        QModelIndex parent = index.parent();
        if(!parent.isValid())
        {
            return m_permissionGroup.at(index.row());
        }
        else
        {
            QList<Player*>* list = m_data.at(parent.row());
            if(!list->isEmpty())
            {
                Player* player = list->at(index.row());
                return player->name();
            }
        }
    }
    return QVariant();
}

QModelIndex ParticipantsModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    Player* childItem = static_cast<Player*>(child.internalPointer());

    QList<Player*>* current = nullptr;
    for(auto list : m_data)
    {
        if(list->contains(childItem))
        {
            current = list;
        }
    }

    if (nullptr == current)
    {
        return QModelIndex();
    }
    else
    {
        return createIndex(m_data.indexOf(current), 0, current);
    }

}

QModelIndex ParticipantsModel::index(int row, int column, const QModelIndex & parent) const
{
    if(row<0)
        return QModelIndex();

    if (!parent.isValid())
    {
        QList<Player*>* list = m_data.at(row);
        return createIndex(row, column, list);
    }
    else
    {
        QList<Player*>* list = static_cast<QList<Player*>*>(parent.internalPointer());
        Player* player = list->at(row);
        return createIndex(row, column, player);
    }
}

Qt::ItemFlags ParticipantsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled  | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}

void ParticipantsModel::addHiddenPlayer(Player * player)
{
    auto parent = createIndex(2,0,&m_hidden);
    beginInsertRows(parent,m_hidden.size(),m_hidden.size());
    m_hidden.append(player);
    endInsertRows();
}

void ParticipantsModel::removePlayer(Player * player)
{
    auto list = getListByChild(player);
    auto parent = createIndex(m_data.indexOf(list),0,&list);

    beginRemoveRows(parent,list->indexOf(player),list->indexOf(player));
    list->removeAll(player);
    endRemoveRows();
}

int ParticipantsModel::promotePlayer(Player* player)
{
    ParticipantsModel::Permission perm = getPermissionFor(player);
    switch(perm)
    {
        case readOnly:
        case hidden:
            setPlayerInto(player,static_cast<Permission>(perm-1));
            return static_cast<Permission>(perm-1);
        break;
    default:
        break;
    }
    return -1;
}

int ParticipantsModel::demotePlayer(Player* player)
{
    ParticipantsModel::Permission perm = getPermissionFor(player);
    switch(perm)
    {
        case readOnly:
        case readWrite:
            setPlayerInto(player,static_cast<Permission>(perm+1));
            return static_cast<Permission>(perm+1);
        break;
    default:
        break;
    }
    return -1;
}
Player* ParticipantsModel::getOwner() const
{
    return m_owner;
}

void ParticipantsModel::setOwner(Player* owner)
{
    m_owner = owner;
    setPlayerInto(m_owner,readWrite);
}
ParticipantsModel::Permission ParticipantsModel::getPermissionFor(Player* player)
{
    int permission = 3;
    for(auto& list : m_data)
    {
        if(list->contains(player))
        {
            permission = m_data.indexOf(list);
        }
    }
    return static_cast<Permission>(permission);
}

void ParticipantsModel::setPlayerInto(Player* player, Permission level)
{
    QList<Player*>* list = getListByChild(player);
    QList<Player*>* destList =m_data.at(level);
    QModelIndex parent;
    QModelIndex dest = createIndex(destList->size(),0,destList);
    int indexDest = destList->size();

    if(destList == list)
    {
        return;
    }

    if(nullptr!=list)
    {
        int index = m_data.indexOf(list);
        int indexPlayer = list->indexOf(player);
        parent = createIndex(index,0,list);


        beginMoveRows(parent,indexPlayer,indexPlayer,dest,indexDest);
        list->removeOne(player);
        destList->append(player);
        endMoveRows();
    }
    else
    {
        beginInsertRows(dest,indexDest,indexDest);
        destList->append(player);
        endInsertRows();
    }
}

void ParticipantsModel::saveModel(QJsonObject& root)
{
    QJsonArray hidden;
    QJsonArray readOnly;
    QJsonArray readWrite;

    for(auto& player : m_hidden)
    {
        hidden << player->getUuid();
    }

    for(auto& player : m_readOnly)
    {
        readOnly << player->getUuid();
    }

    for(auto& player : m_readWrite)
    {
        readWrite << player->getUuid();
    }

    root["hidden"]=hidden;
    root["readOnly"]=readOnly;
    root["readWrite"]=readWrite;
}
void ParticipantsModel::loadModel(QJsonObject& root)
{

    PlayersList* playerList = PlayersList::instance();

    QJsonArray hidden = root["hidden"].toArray();
    QJsonArray readOnly= root["readOnly"].toArray();
    QJsonArray readWrite= root["readWrite"].toArray();

    beginResetModel();

    m_hidden.clear();
    m_readOnly.clear();
    m_readWrite.clear();

    for(auto playerId : hidden)
    {
        Player* p = dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_hidden.append(p);
    }

    for(auto playerId : readOnly)
    {
        Player* p = dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readOnly.append(p);
    }

    for(auto playerId : readWrite)
    {
        Player* p = dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readWrite.append(p);
    }
    endResetModel();
}
QList<Player*>* ParticipantsModel::getListByChild(Player* owner)
{
    for(auto& list : m_data)
    {
        if(list->contains(owner))
        {
            return list;
        }
    }
    return nullptr;
}



////////////////////////////////////////////////
/// \brief ParticipantsPane::ParticipantsPane
/// \param parent
////////////////////////////////////////////////

ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    ui->m_downToolBtn->setDefaultAction(ui->m_demoteAction);
    ui->m_upToolBtn->setDefaultAction(ui->m_promoteAction);


    connect(ui->m_promoteAction,SIGNAL(triggered(bool)),this,SLOT(promoteCurrentItem()));
    connect(ui->m_demoteAction,SIGNAL(triggered(bool)),this,SLOT(demoteCurrentItem()));


    m_playerList = PlayersList::instance();
    connect(m_playerList,SIGNAL(playerAdded(Player*)),this,SLOT(addNewPlayer(Player*)));
    connect(m_playerList,SIGNAL(playerDeleted(Player*)),this,SLOT(removePlayer(Player*)));



    m_model = new ParticipantsModel(m_playerList);
    ui->m_treeview->setModel(m_model);

    ui->connectInfoLabel->hide();
    ui->m_treeview->resizeColumnToContents(0);
    ui->m_treeview->resizeColumnToContents(1);
    ui->m_treeview->expandAll();

    ui->connectInfoLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->participantsLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));

}

ParticipantsPane::~ParticipantsPane()
{
    delete ui;
}
void ParticipantsPane::promoteCurrentItem()
{
    QModelIndex current = ui->m_treeview->currentIndex();

    if(current.parent().isValid())
    {
        Player* player = static_cast<Player*>(current.internalPointer());
        int i = m_model->promotePlayer(player);
        if(i>=0)
        {
            emit memberPermissionsChanged(player->getUuid(),i);
            if(i==ParticipantsModel::readOnly)
            {
                emit memberCanNowRead(player->getUuid());
            }
        }
    }
}
bool ParticipantsPane::isOwner() const
{
    if(nullptr != m_model)
    {
        return m_playerList->isLocal(m_model->getOwner());
    }
    return false;
}
void ParticipantsPane::demoteCurrentItem()
{
    QModelIndex current = ui->m_treeview->currentIndex();

    if(current.parent().isValid())
    {
        Player* player = static_cast<Player*>(current.internalPointer());
        int i = m_model->demotePlayer(player);
        if(i>=0)
        {
            emit memberPermissionsChanged(player->getUuid(),i);
            if(i==ParticipantsModel::hidden)
            {
                emit closeMediaToPlayer(player->getUuid());
            }
        }
    }
}

bool ParticipantsPane::canWrite(Player* player)
{
    return (m_model->getPermissionFor(player) == ParticipantsModel::readWrite);
}

bool ParticipantsPane::canRead(Player* player)
{
    bool readWrite = (m_model->getPermissionFor(player) == ParticipantsModel::readWrite);
    bool read = (m_model->getPermissionFor(player) == ParticipantsModel::readOnly);
    return read | readWrite;
}
void ParticipantsPane::addNewPlayer(Player* player)
{
    if(nullptr != m_model)
    {
        m_model->addHiddenPlayer(player);
    }
}
void ParticipantsPane::removePlayer(Player* player)
{
    if(nullptr != m_model)
    {
        m_model->removePlayer(player);
    }
}

void ParticipantsPane::fill(NetworkMessageWriter* msg)
{
    QJsonDocument doc;
    QJsonObject root;
    m_model->saveModel(root);
    doc.setObject(root);
    msg->byteArray32(doc.toJson());

}
void ParticipantsPane::readFromMsg(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QByteArray data = msg->byteArray32();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        QJsonObject root= doc.object();
        m_model->loadModel(root);
    }
}
void ParticipantsPane::readPermissionChanged(NetworkMessageReader* msg)
{
    QString playerId = msg->string8();
    int perm = msg->int8();
    Player* player =dynamic_cast<Player*>(m_playerList->getPerson(playerId));
    if(nullptr != player)
    {
        m_model->setPlayerInto(player,static_cast<ParticipantsModel::Permission>(perm));
        if(player == m_playerList->getLocalPlayer())
        {
            emit localPlayerPermissionChanged(m_model->getPermissionFor(player));
        }
    }

}

void ParticipantsPane::setFont(QFont font)
{
    // I'm not sure we want to set all these things to this font.
    ui->connectInfoLabel->setFont(font);
}

Player *ParticipantsPane::getOwner() const
{
       return m_model->getOwner();
}

void ParticipantsPane::setOwner(Player *owner)
{
    m_model->setOwner(owner);
    if(owner == m_playerList->getLocalPlayer())
    {
        emit localPlayerIsOwner(true);
        emit localPlayerPermissionChanged(ParticipantsModel::readWrite);
    }

}
