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
#include "webviewupdater.h"

#include "controller/view_controller/webpagecontroller.h"
#include "network/networkmessage.h"
#include "worker/messagehelper.h"

WebViewUpdater::WebViewUpdater(QObject* parent) : MediaUpdaterInterface(parent)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::WebPageCategory, this);
}

void WebViewUpdater::addMediaController(MediaControllerBase* ctrl)
{
    auto webCtrl= dynamic_cast<WebpageController*>(ctrl);
    if(nullptr == webCtrl)
        return;

    if(webCtrl->state() != WebpageController::RemoteView)
    {
        auto shareWebPage= [webCtrl](bool b) {
            if(b)
                MessageHelper::shareWebpage(webCtrl);
            else if(webCtrl->sharingMode() == WebpageController::None)
                MessageHelper::closeMedia(webCtrl->uuid(), webCtrl->contentType());
        };

        connect(webCtrl, &WebpageController::urlSharingChanged, this, shareWebPage);
        connect(webCtrl, &WebpageController::htmlSharingChanged, this, shareWebPage);

        auto updateWebPage= [webCtrl]() {
            if(webCtrl->sharingMode() == WebpageController::None || !webCtrl->keepSharing())
                return;
            MessageHelper::updateWebpage(webCtrl);
        };

        connect(webCtrl, &WebpageController::htmlChanged, this, updateWebPage);
        connect(webCtrl, &WebpageController::pathChanged, this, updateWebPage);
    }
    else
    {
        m_webPages.insert(webCtrl->uuid(), webCtrl);
    }
}

NetWorkReceiver::SendType WebViewUpdater::processMessage(NetworkMessageReader* msg)
{
    if(msg->category() == NetMsg::WebPageCategory && msg->action() == NetMsg::UpdateContent)
    {
        auto uuid= msg->string8();
        auto ctrl= m_webPages.value(uuid);
        if(!ctrl.isNull())
            MessageHelper::readUpdateWebpage(ctrl, msg);
    }
    return NetWorkReceiver::NONE;
}
