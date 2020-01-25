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
#include "webpagemediacontroller.h"

#include "controller/view_controller/webpagecontroller.h"

WebpageMediaController::WebpageMediaController() {}

WebpageMediaController::~WebpageMediaController() {}

CleverURI::ContentType WebpageMediaController::type() const
{
    return CleverURI::WEBVIEW;
}

bool WebpageMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<WebpageController> webCtrl(new WebpageController);

    emit webpagControllerCreated(webCtrl.get());
    m_webpages.push_back(std::move(webCtrl));
    return true;
}

void WebpageMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_webpages.begin(), m_webpages.end(),
                            [id](const std::unique_ptr<WebpageController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_webpages.end())
        return;

    (*it)->aboutToClose();
    m_webpages.erase(it, m_webpages.end());
}

void WebpageMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType WebpageMediaController::processMessage(NetworkMessageReader* msg)
{
    // return;
}

void WebpageMediaController::setUndoStack(QUndoStack* stack)
{
    // return;
}
