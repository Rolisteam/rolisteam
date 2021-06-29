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
#ifndef CAMPAIGNMANAGER_H
#define CAMPAIGNMANAGER_H

#include <QObject>
#include <QString>
#include <memory>

#include "data/campaign.h"
#include "media/mediatype.h"

class DiceParser;
namespace campaign
{
class Media;
class CampaignEditor;
class CampaignUpdater;

class CampaignManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(campaign::Campaign* campaign READ campaign NOTIFY campaignChanged)
public:
    explicit CampaignManager(DiceParser* diceparser, QObject* parent= nullptr);
    virtual ~CampaignManager();

    bool createCampaign(const QUrl& dir);
    QString importFile(const QUrl& dir);
    QString createFileFromData(const QString& name, const QByteArray& data);

    void saveCampaign();
    void copyCampaign(const QUrl& dir);
    void openCampaign(const QUrl& dir);

    campaign::Campaign* campaign() const;
    campaign::CampaignEditor* editor() const;

    QString campaignDir() const;
    QString placeDirectory(campaign::Campaign::Place place) const;
public slots:
    void shareModels();
    void setLocalIsGM(bool b);

signals:
    void campaignChanged(const QString& str);
    void campaignLoaded();
    void fileImported(campaign::Media* path);
    void errorOccured(const QString& error);

private:
    std::unique_ptr<CampaignEditor> m_editor;
    std::unique_ptr<CampaignUpdater> m_campaignUpdater;
};
} // namespace campaign

#endif // CAMPAIGNMANAGER_H