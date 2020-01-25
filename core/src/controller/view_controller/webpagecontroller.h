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
    Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)
    Q_PROPERTY(WebpageController::State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool share READ share WRITE setShare NOTIFY shareChanged)
public:
    enum State
    {
        localIsGM,
        LocalIsPlayer,
        RemoteView
    };
    explicit WebpageController(QObject* parent= nullptr);

    void saveData() const override;
    void loadData() const override;

    QString url() const;
    bool hideUrl() const;
    bool keepSharing() const;
    QString html() const;
    WebpageController::State state() const;
    bool share() const;

signals:
    void urlChanged(QString uri);
    void hideUrlChanged(bool hideUrl);
    void keepSharingChanged(bool keepSharing);
    void htmlChanged(QString html);
    void stateChanged(State state);
    void shareChanged(bool share);

public slots:
    void setUrl(QString url);
    void setHideUrl(bool hideUrl);
    void setKeepSharing(bool keepSharing);
    void setHtml(QString html);
    void setState(State state);
    void setShare(bool share);

private:
    QString m_url;
    bool m_hideUrl;
    bool m_keepSharing;
    QString m_html;
    State m_state;
    bool m_share;
};

#endif // WEBPAGECONTROLLER_H
