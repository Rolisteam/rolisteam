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
#include "charactersheetmediacontroller.h"

#include "controller/view_controller/charactersheetcontroller.h"
#include "data/cleveruri.h"
#include "model/charactermodel.h"

CharacterSheetMediaController::CharacterSheetMediaController(CharacterModel* model) : m_characterModel(model) {}

CleverURI::ContentType CharacterSheetMediaController::type() const
{
    return CleverURI::CHARACTERSHEET;
}

bool CharacterSheetMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<CharacterSheetController> imgCtrl(new CharacterSheetController(m_characterModel, uri));

    emit characterSheetCreated(imgCtrl.get());
    m_sheets.push_back(std::move(imgCtrl));
    return true;
}

void CharacterSheetMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_sheets.begin(), m_sheets.end(),
                            [id](const std::unique_ptr<CharacterSheetController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_sheets.end())
        return;

    (*it)->aboutToClose();
    m_sheets.erase(it, m_sheets.end());
}

void CharacterSheetMediaController::registerNetworkReceiver() {}

NetWorkReceiver::SendType CharacterSheetMediaController::processMessage(NetworkMessageReader* msg) {}
