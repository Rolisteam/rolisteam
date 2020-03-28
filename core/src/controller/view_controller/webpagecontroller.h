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
#ifndef WEBPAGECONTROLLER_H
#define WEBPAGECONTROLLER_H

#include <QObject>

#include "abstractmediacontroller.h"

class WebpageController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool hideUrl READ hideUrl WRITE setHideUrl NOTIFY hideUrlChanged)
    Q_PROPERTY(bool keepSharing READ keepSharing WRITE setKeepSharing NOTIFY keepSharingChanged)
    Q_PROPERTY(bool htmlSharing READ htmlSharing WRITE setHtmlSharing NOTIFY htmlSharingChanged)
    Q_PROPERTY(bool urlSharing READ urlSharing WRITE setUrlSharing NOTIFY urlSharingChanged)
    Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
    Q_PROPERTY(WebpageController::State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(
        WebpageController::SharingMode sharingMode READ sharingMode WRITE setSharingMode NOTIFY sharingModeChanged)
public:
    enum State
    {
        localIsGM,
        LocalIsPlayer,
        RemoteView
    };
    Q_ENUM(State)
    enum SharingMode
    {
        None,
        Url,
        Html
    };

    explicit WebpageController(QObject* parent= nullptr);

    void saveData() const override;
    void loadData() const override;

    QString url() const;
    bool hideUrl() const;
    bool keepSharing() const;
    bool urlSharing() const;
    bool htmlSharing() const;
    QString html() const;
    WebpageController::State state() const;
    WebpageController::SharingMode sharingMode() const;

signals:
    void urlChanged(QString uri);
    void hideUrlChanged(bool hideUrl);
    void keepSharingChanged(bool keepSharing);
    void htmlChanged(QString html);
    void stateChanged(State state);
    void sharingModeChanged(SharingMode mode);
    void htmlSharingChanged(bool htmlSharing);
    void urlSharingChanged(bool urlSharing);

public slots:
    void setUrl(QString url);
    void setHideUrl(bool hideUrl);
    void setKeepSharing(bool keepSharing);
    void setHtml(QString html);
    void setState(State state);
    void setHtmlSharing(bool b);
    void setUrlSharing(bool b);
    void setSharingMode(SharingMode mode);

private:
    QString m_url;
    bool m_hideUrl;
    bool m_keepSharing;
    QString m_html;
    State m_state;
    SharingMode m_mode;
};

#endif // WEBPAGECONTROLLER_H
