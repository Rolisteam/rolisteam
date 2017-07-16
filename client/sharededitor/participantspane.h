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
#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QHostAddress>
#include <QTcpSocket>

namespace Ui {
    class ParticipantsPane;
}

struct Participant {
    // Used by owner and particiapnts:
    QString name;
    QHostAddress address;
    QColor color;
    QTreeWidgetItem *item;

    // Only used by the owner:
    int permissions;
    QTcpSocket *socket;
    // Used to store the incoming packet's size
    quint32 blockSize;
};

class ParticipantsPane : public QWidget {
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setOwnership(bool isOwner);

    void setConnectInfo(QString address, QString port);

    // Participants are first new'd (prior to greetings)
    void newParticipant(QTcpSocket *socket);
    // ...and later added when they send their greetings
    bool addParticipant(QString name, QTcpSocket *socket);
    // getNameForSocket allows us to set their name in the chat pane correctly
    QString getNameForSocket(QTcpSocket *socket);
    // getNameAddressForSocket allows us to get name@address as the owner
    QString getNameAddressForSocket(QTcpSocket *socket);

    // Client participant pane add participant functions
    void newParticipant(QString name, QString address, QString permissions = "waiting");

    // For when we disconnect
    void removeAllParticipants();
    // This is a function to be used by the owner
    void removeParticipant(QTcpSocket *socket);
    // This is a function to be used by the participants in removing participants via control messages.
    void removeParticipant(QString name, QString address);

    void setParticipantPermissions(QString name, QString address, QString permissions);

    void setOwnerName(QString name);

    bool canWrite(QTcpSocket *socket);
    bool canRead(QTcpSocket *socket);

    void setFont(QFont font);

    QList<Participant*> participantList;
    QMap<QTcpSocket *, Participant *> participantMap;

private:
    Ui::ParticipantsPane *ui;

    QTreeWidgetItem *rwItem;
    QTreeWidgetItem *roItem;
    QTreeWidgetItem *waitItem;
    QTreeWidgetItem *owner;

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();

signals:
    void memberCanNowRead(QTcpSocket *member);
    void memberPermissionsChanged(QTcpSocket *member, QString readability);
};

#endif // PARTICIPANTSPANE_H
