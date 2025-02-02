/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef CHARACTERSHEETUPDATER_H
#define CHARACTERSHEETUPDATER_H

#include <QObject>
#include <QPointer>
#include <core_global.h>

#include "mediaupdaterinterface.h"
#include "worker/characterfinder.h"
#include <memory.h>

namespace campaign
{
class CampaignManager;
}
struct CSSharingInfo;

class CharacterSheetController;
class FilteredContentModel;
class CharacterSheet;
class Character;
class CSItem;
class NetworkMessageReader;

class CORE_EXPORT CharacterSheetUpdater : public MediaUpdaterInterface
{
    Q_OBJECT

public:
    enum class SharingMode : quint8
    {
        ALL,
        ONE,
        Many
    };
    Q_ENUM(SharingMode)

    CharacterSheetUpdater(FilteredContentModel* model, campaign::CampaignManager* campaign, QObject* parent= nullptr);
    void addMediaController(MediaControllerBase* ctrl) override;

    void shareCharacterSheetTo(CharacterSheetController* ctrl, CharacterSheet* sheet,
                               CharacterSheetUpdater::SharingMode mode, Character* character,
                               const QStringList& recipients);

    void addRemoteCharacterSheet(CharacterSheetController* ctrl);
    void setUpFieldUpdate(CharacterSheet* sheet) const;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
public slots:
    void updateTableFieldCell(CharacterSheet*, const QString& path, int r, int c);
private slots:
    void updateField(CharacterSheet* sheet, CSItem* itemSheet, const QString& path);
    void updateTableRowCount(bool add, CharacterSheet* sheet, CSItem* itemSheet, const QString& path, int index);

signals:
    void characterSheetAdded(NetworkMessageReader* msg);
    void characterSheetRemoved(const QString& uuid, const QString& ctrlId, const QString& characterId);

private:
    QList<QPointer<CharacterSheetController>> m_ctrls;
    QPointer<FilteredContentModel> m_model;
    QList<CSSharingInfo> m_sharingData;
    std::unique_ptr<CharacterFinder> m_finder;
};

struct CSSharingInfo
{
    QString ctrlId;
    QString sheetId;
    QString characterId;
    CharacterSheetUpdater::SharingMode mode;
    QPointer<CharacterSheet> sheet;
    QStringList recipients;
};
#endif // CHARACTERSHEETUPDATER_H
