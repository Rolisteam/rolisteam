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

#include "controller/view_controller/sharednotecontroller.h"
#include "participantspane.h"
#include "ui_participantspane.h"

#include "data/player.h"
#include "enu.h"
#include "model/playerproxymodel.h"
#include "utilities.h"

////////////////////////////////////////////////
/// \brief ParticipantsPane::ParticipantsPane
/// \param parent
////////////////////////////////////////////////

ParticipantsPane::ParticipantsPane(SharedNoteController* ctrl, QWidget* parent)
    : QWidget(parent), ui(new Ui::ParticipantsPane), m_sharedCtrl(ctrl)
{
    ui->setupUi(this);

    ui->m_downToolBtn->setDefaultAction(ui->m_demoteAction);
    ui->m_upToolBtn->setDefaultAction(ui->m_promoteAction);

    connect(ui->m_promoteAction, &QAction::triggered, this, &ParticipantsPane::promoteCurrentItem);
    connect(ui->m_demoteAction, &QAction::triggered, this, &ParticipantsPane::demoteCurrentItem);

    ui->m_treeview->setModel(m_sharedCtrl->participantModel());

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

    m_sharedCtrl->promoteCurrentItem(current);
}

void ParticipantsPane::demoteCurrentItem()
{
    QModelIndex current= ui->m_treeview->currentIndex();

    if(!current.isValid() || !current.parent().isValid())
        return;

    m_sharedCtrl->demoteCurrentItem(current);
}

/*void ParticipantsPane::fill(NetworkMessageWriter* msg)
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
    / * m_model->setPlayerPermission(playerId, static_cast<ParticipantModel::Permission>(perm));
     if(playerId == m_model->getOwner())
     {
         emit localPlayerPermissionChanged(m_model->getPermissionFor(playerId));
     }* /
}*/
