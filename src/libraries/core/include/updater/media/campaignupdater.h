/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CAMPAIGN_CAMPAIGNUPDATER_H
#define CAMPAIGN_CAMPAIGNUPDATER_H

#include <QObject>
#include <QPointer>

#include "network/receiveevent.h"
#include <core_global.h>

class DiceRoller;
namespace campaign
{
class Campaign;
class CORE_EXPORT CampaignUpdater : public QObject, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool localIsGM READ localIsGM WRITE setLocalIsGM NOTIFY localIsGMChanged)
public:
    explicit CampaignUpdater(DiceRoller* dice, Campaign* manager, QObject* parent= nullptr);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
    DiceRoller* diceParser() const;

    bool localIsGM();

    void setCampaign(Campaign* campaign);

public slots:
    void updateDiceModel();
    void updateStateModel();
    void setLocalIsGM(bool b);
    void saveCampaignTo(const QString& dir);
    void save();
    bool createCampaignTemplate(const QString& path);

signals:
    void localIsGMChanged();

private:
    void setUpdating(bool b);
    bool canForward();
    void saveDataInto(const QString& path);

private:
    QPointer<Campaign> m_campaign;
    QPointer<DiceRoller> m_dice;
    bool m_localIsGm= true;
    bool m_updatingModel= false;
};
} // namespace campaign
#endif // CAMPAIGN_CAMPAIGNUPDATER_H
