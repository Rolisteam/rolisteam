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
#include "webpagecontroller.h"

#include "data/cleveruri.h"

WebpageController::WebpageController(QObject* parent)
    : AbstractMediaContainerController(new CleverURI(CleverURI::WEBVIEW), parent)
{
}

void WebpageController::saveData() const {}

void WebpageController::loadData() const {}

QString WebpageController::url() const
{
    return m_url;
}

bool WebpageController::hideUrl() const
{
    return m_hideUrl;
}

bool WebpageController::keepSharing() const
{
    return m_keepSharing;
}

QString WebpageController::html() const
{
    return m_html;
}

WebpageController::State WebpageController::state() const
{
    return m_state;
}

void WebpageController::setUrl(QString url)
{
    if(m_url == url)
        return;

    m_url= url;
    emit urlChanged(m_url);
}

void WebpageController::setHideUrl(bool hideUrl)
{
    if(m_hideUrl == hideUrl)
        return;

    m_hideUrl= hideUrl;
    emit hideUrlChanged(m_hideUrl);
}

void WebpageController::setKeepSharing(bool keepSharing)
{
    if(m_keepSharing == keepSharing)
        return;

    m_keepSharing= keepSharing;
    emit keepSharingChanged(m_keepSharing);
}

void WebpageController::setHtml(QString html)
{
    if(m_html == html)
        return;

    m_html= html;
    emit htmlChanged(m_html);
}

void WebpageController::setState(WebpageController::State state)
{
    if(m_state == state)
        return;

    m_state= state;
    emit stateChanged(m_state);
}

void WebpageController::share()
{
    // TODO when network is ready
}

void WebpageController::shareHtml()
{
    // TODO when network is ready
}
