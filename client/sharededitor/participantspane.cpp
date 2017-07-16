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

#include "participantspane.h"
#include "ui_participantspane.h"

#include "utilities.h"
#include "enu.h"
#include "data/player.h"



ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    /*connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));*/

    ui->m_downToolBtn->setDefaultAction(ui->m_demoteAction);
    ui->m_upToolBtn->setDefaultAction(ui->m_promoteAction);


    m_playerList = PlayersList::instance();

    m_model = new ParcipantsModel(m_playerList);
    ui->m_treeview->setModel(m_model);
    connect(m_playerList,SIGNAL(playerAdded(Player*)),this,SLOT(addNewPlayer(Player*)));
    connect(m_playerList,SIGNAL(playerDeleted(Player*)),this,SLOT(addNewPlayer(Player*)));

    ui->connectInfoLabel->hide();
    ui->m_treeview->resizeColumnToContents(0);
    ui->m_treeview->resizeColumnToContents(1);
    ui->m_treeview->expandAll();


  /*  rwItem = ui->treeWidget->topLevelItem(0);
    roItem = ui->treeWidget->topLevelItem(1);
    waitItem = ui->treeWidget->topLevelItem(2);
    owner = rwItem->child(0);*/

    ui->connectInfoLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->participantsLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
 /*   ui->promotePushButton->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->demotePushButton->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));*/
}

ParticipantsPane::~ParticipantsPane()
{
    delete ui;
}

void ParticipantsPane::setOwnership(bool isOwner)
{

}


void ParticipantsPane::newParticipant(QTcpSocket *socket)
{

}

bool ParticipantsPane::addParticipant(QString name, QTcpSocket *socket)
{

}
void ParticipantsPane::addNewPlayer(Player*)
{

}





void ParticipantsPane::newParticipant(QString name, QString address, QString permissions)
{

}

void ParticipantsPane::removeAllParticipants()
{

}


void ParticipantsPane::setParticipantPermissions(QString name, QString address, QString permissions)
{

}

void ParticipantsPane::setOwnerName(QString name)
{
    //m_owner->setText(0, name);
}

/*bool ParticipantsPane::canWrite()
{
   // return participantMap.contains(socket) && participantMap.value(socket)->permissions == Enu::ReadWrite;
}

bool ParticipantsPane::canRead()
{
   // return participantMap.value(socket)->permissions == Enu::ReadOnly || participantMap.value(socket)->permissions == Enu::ReadWrite;
}*/

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
}

void ParticipantsPane::onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *)
{    
  /*  if (item->parent() == rwItem) {
        if (owner == item) { // if it's the owner, can't demote
            ui->demotePushButton->setDisabled(true);
        } else {
            ui->demotePushButton->setDisabled(false);
        }
        ui->promotePushButton->setDisabled(true);
        ui->demotePushButton->setText("Demote");
    }
    else if (item->parent() == roItem) {
        ui->demotePushButton->setDisabled(false);
        ui->promotePushButton->setDisabled(false);
        ui->demotePushButton->setText("Demote");
    }
    else if (item->parent() == waitItem) {
        ui->demotePushButton->setDisabled(false);
        ui->demotePushButton->setText("Kick");
        ui->promotePushButton->setDisabled(false);
    }*/
}

void ParticipantsPane::on_promotePushButton_clicked()
{
 /*   QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    // find the currently selected item in the participants list

    if (selectedItems.size() == 0) {
        return;
    }
    for (int i = 0; i < selectedItems.size(); i++) {
        selectedItems.at(i)->setSelected(false);
    }

    QString permissions;
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                // This should not happen, but we won't crash.
                // Instead, disable the promote button.
                ui->promotePushButton->setEnabled(false);
                permissions = "write";
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                rwItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadWrite;
                permissions = "write";
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                waitItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadOnly;

                emit memberCanNowRead(participantList.at(i)->socket);
                permissions = "read";
            }
            emit memberPermissionsChanged(participantList.at(i)->socket, permissions);
            return;
        }
    }*/
}

void ParticipantsPane::on_demotePushButton_clicked()
{
/*    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
    // find the currently selected item in the participants list

    if (selectedItems.size() == 0) {
        return;
    }
    for (int i = 0; i < selectedItems.size(); i++) {
        selectedItems.at(i)->setSelected(false);
    }

    QString permissions;
    for (int i = 0; i < participantList.size(); i++) {
        if (selectedItems.at(0) == participantList.at(i)->item) {
            if (participantList.at(i)->permissions == Enu::ReadWrite) {
                rwItem->removeChild(participantList.at(i)->item);
                roItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::ReadOnly;
                permissions = "read";
            }
            else if (participantList.at(i)->permissions == Enu::ReadOnly) {
                roItem->removeChild(participantList.at(i)->item);
                waitItem->insertChild(0, participantList.at(i)->item);
                participantList.at(i)->permissions = Enu::Waiting;
                permissions = "waiting";
            }
            else if (participantList.at(i)->permissions == Enu::Waiting) {
                // This should only happen when we're kicking someone out of the document.
                permissions = "kick";
            }
            emit memberPermissionsChanged(participantList.at(i)->socket, permissions);
            return;
        }
    }*/
}
/////////////////////////////////////////////////
///
/// ParticipantsModel Code.
///
/////////////////////////////////////////////////

ParcipantsModel::ParcipantsModel(PlayersList* m_playerList)
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

int ParcipantsModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        qDebug() << "size data:" <<m_data.size();
        return m_data.size();
    }
    else if(!parent.parent().isValid())
    {
        qDebug() << "row count parentrow" << parent.row();
        QList<Player*>* list = m_data.at(parent.row());
        return list->size();
    }
    else
    {
        return 0;
    }
}

int ParcipantsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ParcipantsModel::data(const QModelIndex &index, int role) const
{
   // debugModel();
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
            qDebug() << "parent row:"<<parent.row() << index.row();
            QList<Player*>* list = m_data.at(parent.row());
            if(!list->isEmpty())
            {
                Player* player = list->at(index.row());
                return player->getName();
            }
        }
    }
    return QVariant();
}
void ParcipantsModel::debugModel() const
{
    for(auto list : m_data)
    {
        qDebug() << "list:" << m_data.indexOf(list) << "size:" << list->size();
        for(auto player : *list)
        {
            qDebug() << player->getName();
        }
    }
}

QModelIndex ParcipantsModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    Player* childItem = static_cast<Player*>(child.internalPointer());

    QList<Player*>* current = nullptr;
    for(auto list : m_data)
    {
        if(list->contains(childItem))
        {
            //qDebug() << "match" << list << list->size() << childItem;
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

QModelIndex ParcipantsModel::index(int row, int column, const QModelIndex & parent) const
{
    if(row<0)
        return QModelIndex();

    if (!parent.isValid())
    {
        qDebug() << "index" << row;
        QList<Player*>* list = m_data.at(row);
        return createIndex(row, column, list);
    }
    else
    {
        QList<Player*>* list = static_cast<QList<Player*>*>(parent.internalPointer());
        Player* player = list->at(row);
        qDebug() << player->getName();
        return createIndex(row, column, player);
    }
}

Qt::ItemFlags ParcipantsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled  | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}

void ParcipantsModel::addHiddenPlayer(Player *)
{

}

void ParcipantsModel::removePlayer(Player *)
{

}

void ParcipantsModel::promotePlayer(Player *)
{

}

void ParcipantsModel::demotePlayer(Player *)
{

}
Player* ParcipantsModel::getOwner() const
{
    return m_owner;
}

void ParcipantsModel::setOwner(Player* owner)
{
    m_owner = owner;
    QList<Player*>* list = getListByChild(m_owner);
    QList<Player*>* destList =m_data.at(readWrite);
    QModelIndex parent;
    QModelIndex dest = createIndex(0,0,destList);
    int indexDest = destList->size();

    if(nullptr!=list)
    {
        int index = m_data.indexOf(list);
        int indexPlayer = list->indexOf(owner);
        parent = createIndex(index,0,list);


        beginMoveRows(parent,indexPlayer,indexPlayer,dest,indexDest);
        list->removeOne(owner);
        destList->append(owner);
        endMoveRows();
    }
    else
    {
        beginInsertRows(dest,indexDest,indexDest);
        destList->append(owner);
        endInsertRows();
    }

}
QList<Player*>* ParcipantsModel::getListByChild(Player* owner)
{
    for(auto list : m_data)
    {
        if(list->contains(owner))
        {
            return list;
        }
    }
    return nullptr;
}
