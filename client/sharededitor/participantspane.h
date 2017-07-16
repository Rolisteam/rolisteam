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

#include "userlist/playersList.h"

namespace Ui {
    class ParticipantsPane;
}

class ParcipantsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Permission {readWrite,readOnly,hidden};
    ParcipantsModel(PlayersList* m_playerList);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex & parent) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    Player* getOwner() const;
    void setOwner(Player *owner);

public slots:
    virtual void addHiddenPlayer(Player*);
    virtual void removePlayer(Player*);
    void promotePlayer(Player*);
    void demotePlayer(Player*);

private:
    QList<Player*> m_hidden;
    QList<Player*> m_readOnly;
    QList<Player*> m_readWrite;

    QList<QList<Player*>*> m_data;
    QStringList m_permissionGroup;

    Player* m_owner;
    QList<Player *> *getListByChild(Player *owner);
    void debugModel() const;
};

class ParticipantsPane : public QWidget
{
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    ~ParticipantsPane();

    void setOwnership(bool isOwner);
    void newParticipant(QTcpSocket *socket);
    bool addParticipant(QString name, QTcpSocket *socket);
    void newParticipant(QString name, QString address, QString permissions = "waiting");
    void removeAllParticipants();
    void removeParticipant(QString name, QString address);
    void setParticipantPermissions(QString name, QString address, QString permissions);
    void setOwnerName(QString name);
    bool canWrite(QTcpSocket *socket);
    bool canRead(QTcpSocket *socket);
    void setFont(QFont font);

    Player* getOwner() const;
    void setOwner(Player *owner);

signals:
    void memberCanNowRead(QTcpSocket *member);
    void memberPermissionsChanged(QTcpSocket *member, QString readability);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *);
    void on_promotePushButton_clicked();
    void on_demotePushButton_clicked();
    void addNewPlayer(Player*);

private:
    Ui::ParticipantsPane* ui;

    PlayersList* m_playerList;
    ParcipantsModel* m_model;

};

#endif // PARTICIPANTSPANE_H
