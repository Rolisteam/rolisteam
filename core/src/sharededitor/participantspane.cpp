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
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTime>
#include <QTreeWidgetItem>

#include "participantspane.h"
#include "ui_participantspane.h"

#include "data/player.h"
#include "enu.h"
#include "utilities.h"

/////////////////////////////////////////////////
///
/// ParticipantsModel Code.
///
/////////////////////////////////////////////////

ParticipantsModel::ParticipantsModel(QObject* parent) : QAbstractItemModel(parent)
{
    m_permissionGroup << tr("Read Write") << tr("Read Only") << tr("Hidden");
}

/*QModelIndex ParticipantsModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(!parent.isValid() && sourceIndex.row() > 0)
        return QModelIndex();

    if(!parent.isValid() && sourceIndex.row() == 0)
        return createIndex(0, 0, sourceIndex.internalPointer());

    if(parent.isValid() && parent.row() == 0)
        return createIndex(1 + sourceIndex.row(), 0, sourceIndex.internalPointer());

    return QModelIndex();
}

QModelIndex ParticipantsModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if(proxyIndex.row() == 0)
        return sourceModel()->index(0, 0, QModelIndex());
    else
        return sourceModel()->index(proxyIndex.row() - 1, 0, sourceModel()->index(0, 0, QModelIndex()));
}*/

int ParticipantsModel::rowCount(const QModelIndex& parent) const
{
    return 0;
    /*if(!parent.isValid())
    {
        return m_permissionGroup.size();
    }
    else
    {
        auto size= m_playerList->rowCount();
        int result= 0;
        switch(parent.row())
        {
        case readWrite:
            result= static_cast<int>(m_readWrite.size());
            break;
        case readOnly:
            result= static_cast<int>(m_readOnly.size());
            break;
        case hidden:
            result= static_cast<int>(size - (m_readOnly.size() + m_readWrite.size()));
            break;
        default:
            break;
        }
        return result;
    }*/
}

int ParticipantsModel::columnCount(const QModelIndex& index) const
{
    if(index.isValid())
        return 0;
    return 0;
}

QVariant ParticipantsModel::data(const QModelIndex& index, int role) const
{
    return {};
    /* if(!index.isValid())
         return QVariant();

     if(!index.parent().isValid())
     {
         auto parent= index.parent();
         const std::vector<QString>* vector= nullptr;
         if(parent.row() == 0)
         {
             vector= &m_readWrite;
         }
         else if(parent.row() == 1)
         {
             vector= &m_readOnly;
         }

         auto pos= static_cast<std::size_t>(index.row());
         QString id;
         if(nullptr != vector)
         {
             if(pos < vector->size())
                 id= vector->at(pos);
         }
         auto player= m_playerList->playerById(id);
         if(nullptr != player)
             return player->name();
         else
             return {};
     }
     else if(role == Qt::DisplayRole || role == Qt::EditRole || role == PlayerModel::IdentifierRole)
     {
         return m_permissionGroup.at(index.row());
     }*/

    /* if(Qt::DisplayRole == role)
     {
         QModelIndex parent= index.parent();
         if(!parent.isValid())
         {
             return m_permissionGroup.at(index.row());
         }
         else
         {
             QList<Player*>* list= m_data.at(parent.row());
             if(!list->isEmpty())
             {
                 Player* player= list->at(index.row());
                 return player->name();
             }
         }
     }
     return QVariant();*/
}

QModelIndex ParticipantsModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
    /*if(!child.isValid())
        return QModelIndex();

    auto id= child.data(PlayerModel::IdentifierRole).toString();

    if(m_permissionGroup.contains(id))
        return QModelIndex();
    else if(std::find(m_readWrite.begin(), m_readWrite.end(), id) != m_readWrite.end())
        return createIndex(0, 0);
    else if(std::find(m_readOnly.begin(), m_readOnly.end(), id) != m_readOnly.end())
        return createIndex(1, 0);
    else
        return createIndex(2, 0);
*/
    /*
    Player* childItem= static_cast<Player*>(child.internalPointer());

    QList<Player*>* current= nullptr;
    for(auto list : m_data)
    {
        if(list->contains(childItem))
        {
            current= list;
        }
    }

    if(nullptr == current)
    {
        return QModelIndex();
    }
    else
    {
        return createIndex(m_data.indexOf(current), 0, current);
    }*/
}

QModelIndex ParticipantsModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
    /*
    if(row < 0)
        return QModelIndex();

    void* node= nullptr;
    QString id;
    if(!parent.isValid() && row == 1)
        node= const_cast<void*>(reinterpret_cast<const void*>(&m_readOnly));
    else if(!parent.isValid() && row == 0)
        node= const_cast<void*>(reinterpret_cast<const void*>(&m_readWrite));
    else if(!parent.isValid() && row == 2)
        node= const_cast<void*>(reinterpret_cast<const void*>(&m_playerList));
    else if(parent.isValid() && row == 0)
    {
    }

    return createIndex(row, column, node);*/
}

Qt::ItemFlags ParticipantsModel::flags(const QModelIndex& index) const
{
    if(index.parent().isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ParticipantsModel::addHiddenPlayer(Player* player)
{
    /*auto parent= createIndex(2, 0, &m_hidden);
    beginInsertRows(parent, m_hidden.size(), m_hidden.size());
    m_hidden.append(player);
    endInsertRows();*/
}

void ParticipantsModel::removePlayer(Player* player)
{
    /*  auto list= getListByChild(player);
       auto parent= createIndex(m_data.indexOf(list), 0, &list);

       beginRemoveRows(parent, list->indexOf(player), list->indexOf(player));
       list->removeAll(player);
       endRemoveRows();*/
}

int ParticipantsModel::promotePlayer(const QModelIndex& index)
{
    ParticipantsModel::Permission perm= getPermissionFor(index);
    switch(perm)
    {
    case readOnly:
    case hidden:
        setPlayerInto(index, static_cast<Permission>(perm - 1));
        return static_cast<Permission>(perm - 1);
        break;
    default:
        break;
    }
    return -1;
}

int ParticipantsModel::demotePlayer(const QModelIndex& index)
{
    if(!index.isValid())
        return 0;

    ParticipantsModel::Permission perm= getPermissionFor(index);
    switch(perm)
    {
    case readOnly:
    case readWrite:
        setPlayerInto(index, static_cast<Permission>(perm + 1));
        return static_cast<Permission>(perm + 1);
        break;
    default:
        break;
    }
    return -1;
}
QString ParticipantsModel::getOwner() const
{
    return m_ownerId;
}

void ParticipantsModel::setOwner(const QString& owner)
{
    if(owner == m_ownerId)
        return;
    m_ownerId= owner;
    beginInsertRows(createIndex(0, 0), 0, 0);
    m_readWrite.push_back(m_ownerId);
    endInsertRows();
}
ParticipantsModel::Permission ParticipantsModel::getPermissionFor(const QModelIndex& index)
{
    if(!index.isValid())
        return hidden;

    auto parent= index.parent();
    if(!parent.isValid())
        return hidden;

    return static_cast<Permission>(index.row());
}

void ParticipantsModel::setPlayerInto(const QModelIndex& index, Permission level)
{
    auto uuid= index.data(PlayerModel::IdentifierRole).toString();

    QModelIndex parent= index.parent();
    QModelIndex destParent= createIndex(static_cast<int>(level), 0);
    auto sourceRow= static_cast<Permission>(parent.row());
    auto destRow= static_cast<Permission>(destParent.row());
    if(destParent == parent)
    {
        return;
    }

    auto r= rowCount(destParent);

    beginMoveRows(parent, index.row(), index.row(), destParent, r);
    if(sourceRow == readWrite)
        std::remove(m_readWrite.begin(), m_readWrite.end(), uuid);
    else if(sourceRow == readOnly)
        std::remove(m_readOnly.begin(), m_readOnly.end(), uuid);

    if(destRow == readWrite)
        m_readWrite.push_back(uuid);
    else if(destRow == readOnly)
        m_readOnly.push_back(uuid);

    endMoveRows();

    /*

      if(nullptr != list)
      {
          int index= m_data.indexOf(list);
          int indexPlayer= list->indexOf(player);
          parent= createIndex(index, 0, list);

          beginMoveRows(parent, indexPlayer, indexPlayer, dest, indexDest);
          list->removeOne(player);
          destList->append(player);
          endMoveRows();
      }
      else
      {
          beginInsertRows(dest, indexDest, indexDest);
          destList->append(player);
          endInsertRows();
      }*/
}

void ParticipantsModel::saveModel(QJsonObject& root)
{
    /*   QJsonArray hidden;
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

       root["hidden"]= hidden;
       root["readOnly"]= readOnly;
       root["readWrite"]= readWrite;*/
}
void ParticipantsModel::loadModel(QJsonObject& root)
{
    /*PlayerModel* playerList= PlayerModel::instance();

    QJsonArray hidden= root["hidden"].toArray();
    QJsonArray readOnly= root["readOnly"].toArray();
    QJsonArray readWrite= root["readWrite"].toArray();

    beginResetModel();

    m_hidden.clear();
    m_readOnly.clear();
    m_readWrite.clear();

    for(auto playerId : hidden)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_hidden.append(p);
    }

    for(auto playerId : readOnly)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readOnly.append(p);
    }

    for(auto playerId : readWrite)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readWrite.append(p);
    }
    endResetModel();*/
}

////////////////////////////////////////////////
/// \brief ParticipantsPane::ParticipantsPane
/// \param parent
////////////////////////////////////////////////

ParticipantsPane::ParticipantsPane(QWidget* parent) : QWidget(parent), ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    ui->m_downToolBtn->setDefaultAction(ui->m_demoteAction);
    ui->m_upToolBtn->setDefaultAction(ui->m_promoteAction);

    connect(ui->m_promoteAction, SIGNAL(triggered(bool)), this, SLOT(promoteCurrentItem()));
    connect(ui->m_demoteAction, SIGNAL(triggered(bool)), this, SLOT(demoteCurrentItem()));

    /*m_playerList= PlayerModel::instance();
    connect(m_playerList, SIGNAL(playerAdded(Player*)), this, SLOT(addNewPlayer(Player*)));
    connect(m_playerList, SIGNAL(playerDeleted(Player*)), this, SLOT(removePlayer(Player*)));*/

    m_model= new ParticipantsModel(this);
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
    QModelIndex current= ui->m_treeview->currentIndex();

    if(current.parent().isValid())
    {
        Player* player= static_cast<Player*>(current.internalPointer());
        int i= m_model->promotePlayer(current);
        if(i >= 0)
        {
            emit memberPermissionsChanged(player->getUuid(), i);
            if(i == ParticipantsModel::readOnly)
            {
                emit memberCanNowRead(player->getUuid());
            }
        }
    }
}
bool ParticipantsPane::isOwner() const
{
    /* if(nullptr != m_model)
     {
         return m_playerList->isLocal(m_model->getOwner());
     }*/
    return true;
}
void ParticipantsPane::demoteCurrentItem()
{
    QModelIndex current= ui->m_treeview->currentIndex();

    if(!current.parent().isValid())
        return;

    Player* player= static_cast<Player*>(current.internalPointer());
    int i= m_model->demotePlayer(current);
    if(i >= 0)
    {
        emit memberPermissionsChanged(player->getUuid(), i);
        if(i == ParticipantsModel::hidden)
        {
            emit closeMediaToPlayer(player->getUuid());
        }
    }
}

bool ParticipantsPane::canWrite(Player* player)
{
    return true; //(m_model->getPermissionFor(player) == ParticipantsModel::readWrite);
}

bool ParticipantsPane::canRead(Player* player)
{
    /*bool readWrite= (m_model->getPermissionFor(player) == ParticipantsModel::readWrite);
    bool read= (m_model->getPermissionFor(player) == ParticipantsModel::readOnly);*/
    return true; // read | readWrite;
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
    if(nullptr != msg)
    {
        QByteArray data= msg->byteArray32();
        QJsonDocument doc= QJsonDocument::fromJson(data);

        QJsonObject root= doc.object();
        m_model->loadModel(root);
    }
}
void ParticipantsPane::readPermissionChanged(NetworkMessageReader* msg)
{
    QString playerId= msg->string8();
    int perm= msg->int8();
    /*Player* player= dynamic_cast<Player*>(m_playerList->getPerson(playerId));
    if(nullptr != player)
    {
        m_model->setPlayerInto(player, static_cast<ParticipantsModel::Permission>(perm));
        if(player == m_playerList->getLocalPlayer())
        {
            emit localPlayerPermissionChanged(m_model->getPermissionFor(player));
        }
    }*/
}

void ParticipantsPane::setFont(QFont font)
{
    // I'm not sure we want to set all these things to this font.
    ui->connectInfoLabel->setFont(font);
}

Player* ParticipantsPane::getOwner() const
{
    return nullptr; // m_model->getOwner();
}

void ParticipantsPane::setOwnerId(const QString& id)
{
    if(nullptr == m_playerList)
        return;
    // auto owner= m_playerList->getPlayer(id);

    m_model->setOwner(id);
    /* if(owner == m_playerList->getLocalPlayer())
     {
         emit localPlayerIsOwner(true);
         emit localPlayerPermissionChanged(ParticipantsModel::readWrite);
     }*/
}
