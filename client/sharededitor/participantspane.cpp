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
#include "participantspane.h"
#include "ui_participantspane.h"

#include "utilities.h"
#include "enu.h"

#include <QTime>
#include <QHostAddress>
#include <QTreeWidgetItem>
#include <QSettings>


ParticipantsPane::ParticipantsPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParticipantsPane)
{
    ui->setupUi(this);

    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    ui->connectInfoLabel->hide();
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->expandAll();

    rwItem = ui->treeWidget->topLevelItem(0);
    roItem = ui->treeWidget->topLevelItem(1);
    waitItem = ui->treeWidget->topLevelItem(2);
    owner = rwItem->child(0);

    ui->connectInfoLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->participantsLabel->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->promotePushButton->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->demotePushButton->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
}

ParticipantsPane::~ParticipantsPane()
{
    delete ui;
}

void ParticipantsPane::setOwnership(bool isOwner)
{
    ui->promotePushButton->setVisible(isOwner);
    ui->demotePushButton->setVisible(isOwner);
}

void ParticipantsPane::setConnectInfo(QString address, QString port)
{
    ui->connectInfoLabel->show();
    ui->connectInfoLabel->setText("IP: " + address + "\nPort: " + port);
}

void ParticipantsPane::newParticipant(QTcpSocket *socket)
{
//    qDebug() << socket << " connected";
    // Add a participant, but don't make an item until we have a name.
    // Once we have a name, we updateName() (below) and insert it to the treeWidget
    Participant *participant = new Participant;
    participantList.append(participant);
    participantMap.insert(socket, participant);

    participant->socket = socket;
    participant->permissions = Enu::Waiting;
    participant->socket = socket;
    participant->address = socket->peerAddress();

    // Initializes the incoming block size to 0
    participant->blockSize = 0;
}

bool ParticipantsPane::addParticipant(QString name, QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);

    for (int i = 0; i < participantList.size(); i++) {
        if (socket->peerAddress() == participantList.at(i)->address && participantList.at(i)->name == name) {
            // duplicate connection, reject
//            qDebug() << "duplicate connection, rejecting...";
            participantList.removeOne(participant);
            participantMap.remove(socket);
            delete participant;
            return false;
        }
    }
    participant->name = name;
    // This person is now ready to be added to the permissions tree view
    participant->item = new QTreeWidgetItem(waitItem);
    participant->item->setText(0, name);

    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);

    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, QString("%1@%2").arg(name).arg(participant->address.toString()));

    return true;
}

QString ParticipantsPane::getNameForSocket(QTcpSocket *socket)
{
    Participant *participant = participantMap.value(socket);
    return participant->name;
}

QString ParticipantsPane::getNameAddressForSocket(QTcpSocket *socket)
{
    if (participantMap.contains(socket)) {
        Participant *participant = participantMap.value(socket);
        return QString("%1@%2").arg(participant->name).arg(participant->address.toString());
    }
    else {
        return "NULL";
    }
}

void ParticipantsPane::newParticipant(QString name, QString address, QString permissions)
{
    // The server has given us a name to add to the treeWidget
    Participant *participant = new Participant;
    participantList.append(participant);

    participant->name = name;
    participant->address = QHostAddress(address);

    // everyone has their own colors - colors aren't consistent across participants
    participant->color = QColor::fromHsv(qrand() % 256, 190, 190);

    if (permissions == "waiting") {
        participant->item = new QTreeWidgetItem(waitItem);
        participant->permissions = Enu::Waiting;
    }
    else if (permissions == "read") {
        participant->item = new QTreeWidgetItem(roItem);
        participant->permissions = Enu::ReadOnly;
    }
    else if (permissions == "write") {
        participant->item = new QTreeWidgetItem(rwItem);
        participant->permissions = Enu::ReadWrite;
    }
    participant->item->setText(0, name);
    participant->item->setBackgroundColor(1, participant->color);
    participant->item->setToolTip(0, name + "@" + address);
}

void ParticipantsPane::removeAllParticipants()
{
    for (int i = 0; i < participantList.size(); i++) {
        participantList.at(i)->item->parent()->removeChild(participantList.at(i)->item);
        delete participantList.at(i);
    }
    participantList.clear();
    participantMap.clear();
}

void ParticipantsPane::removeParticipant(QTcpSocket *socket)
{
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->socket == socket) {
            Participant *participant = participantList.at(i);
            participant->item->parent()->removeChild(participant->item);
            participantMap.remove(socket);
            participantList.removeAt(i);
            delete participant;
            return;
        }
    }
}

void ParticipantsPane::removeParticipant(QString name, QString address)
{
    // This is a function to be used by the participants in removing participants via control messages.
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->name == name && participantList.at(i)->address.toString() == address) {
            Participant *participant = participantList.at(i);
            participant->item->parent()->removeChild(participant->item);
            participantList.removeAt(i);
            delete participant;
        }
    }
}

void ParticipantsPane::setParticipantPermissions(QString name, QString address, QString permissions)
{
    QString fullName = name + "@" + address;
    for (int i = 0; i < participantList.size(); i++) {
        if (participantList.at(i)->item->toolTip(0) == fullName) {
            Participant *participant = participantList.at(i);
            participant->item->parent()->removeChild(participant->item);

            if (permissions == "waiting") {
                waitItem->addChild(participant->item);
                participant->permissions = Enu::Waiting;
            }
            else if (permissions == "read") {
                roItem->addChild(participant->item);
                participant->permissions = Enu::ReadOnly;
            }
            else if (permissions == "write") {
                rwItem->addChild(participant->item);
                participant->permissions = Enu::ReadWrite;
            }
        }
    }
}

void ParticipantsPane::setOwnerName(QString name)
{
//    qDebug() << "setting owner name: " << name;
    owner->setText(0, name);
}

bool ParticipantsPane::canWrite(QTcpSocket *socket)
{
    return participantMap.contains(socket) && participantMap.value(socket)->permissions == Enu::ReadWrite;
}

bool ParticipantsPane::canRead(QTcpSocket *socket)
{
    return participantMap.value(socket)->permissions == Enu::ReadOnly || participantMap.value(socket)->permissions == Enu::ReadWrite;
}

void ParticipantsPane::setFont(QFont font)
{
    // I'm not sure we want to set all these things to this font.
    ui->treeWidget->setFont(font);
    ui->connectInfoLabel->setFont(font);
    ui->promotePushButton->setFont(font);
    ui->demotePushButton->setFont(font);
}

void ParticipantsPane::onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *)
{    
    if (item->parent() == rwItem) {
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
    }
}

void ParticipantsPane::on_promotePushButton_clicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
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
    }
}

void ParticipantsPane::on_demotePushButton_clicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();
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
    }
}
