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
#include <core_global.h>

#include "mediaupdaterinterface.h"

namespace campaign
{
class CampaignManager;
}
struct CSSharingInfo;

class CharacterSheetController;
class CharacterSheet;
class Character;
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

    CharacterSheetUpdater(campaign::CampaignManager* campaign, QObject* parent= nullptr);
    void addMediaController(MediaControllerBase* ctrl) override;

    void shareCharacterSheetTo(CharacterSheetController* ctrl, CharacterSheet* sheet,
                               CharacterSheetUpdater::SharingMode mode, Character* character,
                               const QStringList& recipients, bool gmToPlayer= true);

private:
    QList<QPointer<CharacterSheetController>> m_ctrls;
    QList<CSSharingInfo> m_sharingData;
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
