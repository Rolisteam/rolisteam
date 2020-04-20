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
#include "model/participantsmodel.h"
#include "model/playerproxymodel.h"
#include "utilities.h"

////////////////////////////////////////////////
/// \brief ParticipantsPane::ParticipantsPane
/// \param parent
////////////////////////////////////////////////

ParticipantsPane::ParticipantsPane(PlayerModel* playerModel, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParticipantsPane), m_model(new ParticipantModel(playerModel))
{
    ui->setupUi(this);

    ui->m_downToolBtn->setDefaultAction(ui->m_demoteAction);
    ui->m_upToolBtn->setDefaultAction(ui->m_promoteAction);

    connect(ui->m_promoteAction, SIGNAL(triggered(bool)), this, SLOT(promoteCurrentItem()));
    connect(ui->m_demoteAction, SIGNAL(triggered(bool)), this, SLOT(demoteCurrentItem()));

    ui->m_treeview->setModel(m_model.get());

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

    if(!current.isValid() || !current.parent().isValid())
        return;

    m_model->promotePlayer(current);
    //    emit memberCanNowRead(player->uuid());

    /*if(current.parent().isValid())
    {
        Player* player= static_cast<Player*>(current.internalPointer());
        if(i >= 0)
        {
            emit memberPermissionsChanged(player->uuid(), i);
            if(i == ParticipantModel::readOnly)
            {
            }
        }
    }*/
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

    if(!current.isValid() || !current.parent().isValid())
        return;

    m_model->demotePlayer(current);

    /*    if(i >= 0)
        {
            emit memberPermissionsChanged(player->uuid(), i);
            if(i == ParticipantModel::hidden)
            {
                emit closeMediaToPlayer(player->uuid());
            }
        }*/
}

bool ParticipantsPane::canWrite(Player* player)
{
    if(player == nullptr)
        return false;
    return true; //(m_model->getPermissionFor(player->uuid()) == ParticipantModel::readWrite);
}

bool ParticipantsPane::canRead(Player* player)
{
    if(player == nullptr)
        return false;
    // bool readWrite= (m_model->getPermissionFor(player->uuid()) == ParticipantModel::readWrite);
    // bool read= (m_model->getPermissionFor(player->uuid()) == ParticipantModel::readOnly);
    return true; // read | readWrite;
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
    /* m_model->setPlayerPermission(playerId, static_cast<ParticipantModel::Permission>(perm));
     if(playerId == m_model->getOwner())
     {
         emit localPlayerPermissionChanged(m_model->getPermissionFor(playerId));
     }*/
}

void ParticipantsPane::setFont(QFont font)
{
    // I'm not sure we want to set all these things to this font.
    ui->connectInfoLabel->setFont(font);
}

void ParticipantsPane::setOwnerId(const QString& id)
{
    /* if(nullptr == m_playerList)
         return;
     // auto owner= m_playerList->getPlayer(id);*/

    m_model->setOwner(id);
    /* if(owner == m_playerList->getLocalPlayer())
     {
         emit localPlayerIsOwner(true);
         emit localPlayerPermissionChanged(ParticipantsModel::readWrite);
     }*/
}
