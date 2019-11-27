/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef VECTORIALMAPMEDIACONTROLLER_H
#define VECTORIALMAPMEDIACONTROLLER_H

#include "mediacontrollerinterface.h"

class MediaContainer;
class VectorialMapMediaController : public MediaControllerInterface
{
    Q_OBJECT
public:
    VectorialMapMediaController();

    CleverURI::ContentType type() const;
    bool openMedia(CleverURI*);
    void clodeMedia(const QString& id);
    void registerNetworkReceiver();
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

private:
    std::vector<CleverURI*> m_openMedia;
};

#endif // VECTORIALMAPMEDIACONTROLLER_H