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
#ifndef NOTEMEDIACONTROLLER_H
#define NOTEMEDIACONTROLLER_H

#include <QObject>

#include "mediamanagerbase.h"

class NoteController;
class NoteMediaController : public MediaManagerBase
{
    Q_OBJECT
public:
    explicit NoteMediaController(ContentModel* contentModel, QObject* parent= nullptr);
    ~NoteMediaController() override;

    // bool openMedia(const QString& uuid, const std::map<QString, QVariant>& args) override;
    // void closeMedia(const QString& id) override;
    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
    int managerCount() const override;

    std::vector<NoteController*> controllers() const;
signals:
    void noteControllerCreated(NoteController* noteCtrl);

private:
    std::vector<std::unique_ptr<NoteController>> m_notes;
};

#endif // NOTEMEDIACONTROLLER_H
