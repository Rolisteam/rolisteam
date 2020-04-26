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

#include <QAbstractProxyModel>
#include <QHostAddress>
#include <QTreeWidgetItem>
#include <QWidget>

//#include "network/networkmessagereader.h"
//#include "network/networkmessagewriter.h"
#include "participantmodel.h"

namespace Ui
{
class ParticipantsPane;
}

class SharedNoteController;
class ParticipantsPane : public QWidget
{
    Q_OBJECT
public:
    ParticipantsPane(SharedNoteController* ctrl, QWidget* parent= nullptr);
    virtual ~ParticipantsPane();

    bool canWrite(Player* idPlayer);
    bool canRead(Player* idPlayer);

    // void fill(NetworkMessageWriter* msg);
    // void readFromMsg(NetworkMessageReader* msg);
    // void readPermissionChanged(NetworkMessageReader* msg);

signals:
    void memberCanNowRead(QString name);
    void memberPermissionsChanged(QString id, int i);
    void closeMediaToPlayer(QString id);
    void localPlayerPermissionChanged(ParticipantModel::Permission);
    void localPlayerIsOwner(bool);

private slots:
    void promoteCurrentItem();
    void demoteCurrentItem();

private:
    Ui::ParticipantsPane* ui;
    QPointer<SharedNoteController> m_sharedCtrl;
};

#endif // PARTICIPANTSPANE_H
