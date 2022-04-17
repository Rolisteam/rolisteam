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
#ifndef CHARACTERSHEETMEDIACONTROLLER_H
#define CHARACTERSHEETMEDIACONTROLLER_H

#include "mediamanagerbase.h"

#include <QPointer>
#include <core_global.h>
#include <memory>
#include <vector>
class CharacterModel;
class ContentModel;
class CharacterSheetController;
class CORE_EXPORT CharacterSheetMediaController : public MediaManagerBase
{
    Q_OBJECT
    Q_PROPERTY(QString gameMasterId READ gameMasterId WRITE setGameMasterId NOTIFY gameMasterIdChanged)
public:
    CharacterSheetMediaController(ContentModel* model, CharacterModel* characterModel);

    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    QString gameMasterId() const;
    std::vector<CharacterSheetController*> controllers() const;

public slots:
    void setGameMasterId(const QString& gameMasterId);

signals:
    void gameMasterIdChanged(QString gameMasterId);

private:
    void addCharacterSheet(const QHash<QString, QVariant>& params);

private:
    // std::vector<std::unique_ptr<CharacterSheetController>> m_sheets;
    QPointer<CharacterModel> m_characterModel;
    QString m_gameMasterId;
};

#endif // CHARACTERSHEETMEDIACONTROLLER_H
