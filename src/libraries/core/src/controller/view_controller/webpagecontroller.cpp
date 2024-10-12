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
#include "controller/view_controller/webpagecontroller.h"

WebpageController::WebpageController(const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::WEBVIEW, parent)
{
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

bool WebpageController::htmlSharing() const
{
    return Html == m_mode;
}

bool WebpageController::urlSharing() const
{
    return Url == m_mode;
}

void WebpageController::setHtmlSharing(bool sharing)
{
    if(htmlSharing() == sharing)
        return;

    if(sharing)
        setSharingMode(Html);
    else
        setSharingMode(None);

    emit htmlSharingChanged(sharing);
}

void WebpageController::setUrlSharing(bool sharing)
{
    if(urlSharing() == sharing)
        return;

    if(sharing)
        setSharingMode(Url);
    else
        setSharingMode(None);
    emit urlSharingChanged(sharing);
}

void WebpageController::setSharingMode(WebpageController::SharingMode mode)
{
    if(mode == m_mode)
        return;

    auto update= false;
    if(mode == None || m_mode == None)
        update= true;
    m_mode= mode;

    if(update)
        emit sharingModeChanged(mode);
}

void WebpageController::setPageUrl(QUrl url)
{
    if(url == m_pageUrl)
        return;
    m_pageUrl= url;
    emit pageUrlChanged();
}

WebpageController::SharingMode WebpageController::sharingMode() const
{
    return m_mode;
}

QUrl WebpageController::pageUrl() const
{
    return m_pageUrl;
}

QUrl WebpageController::url() const
{
    if(localGM())
        return MediaControllerBase::url();

    return m_pageUrl;
}
