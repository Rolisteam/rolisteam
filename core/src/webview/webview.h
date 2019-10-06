/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     http://www.rolisteam.org/                                           *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QAction>
#include <QMouseEvent>
#include <QWebEngineView>
#include <QWidget>

#include "data/mediacontainer.h"
#include "network/networkmessagewriter.h"

class WebView : public MediaContainer
{
    Q_OBJECT

public:
    enum ShareType
    {
        URL,
        HTML
    };
    enum State
    {
        localIsGM,
        LocalIsPlayer,
        RemoteView
    };
    explicit WebView(State localIsOwner, QWidget* parent= nullptr);
    virtual ~WebView();

    virtual bool readFileFromUri();
    virtual void saveMedia();
    virtual void putDataIntoCleverUri();

    void fill(NetworkMessageWriter& message);
    void readMessage(NetworkMessageReader& msg);

protected:
    void mousePressEvent(QMouseEvent* mouseEvent);
    void showEvent(QShowEvent* event);

    void createActions();
    void creationToolBar();

    void updateTitle();
    void sendOffClose();

private:
    bool m_keepSharing= false;
    QWebEngineView* m_view= nullptr;
    QAction* m_shareAsLink= nullptr;
    QAction* m_shareAsHtml= nullptr;
    // QAction* m_shareAsView = nullptr;
    QAction* m_hideAddress= nullptr;
    QAction* m_next= nullptr;
    QAction* m_previous= nullptr;
    QAction* m_reload= nullptr;
    QLineEdit* m_addressEdit= nullptr;
    QVBoxLayout* m_mainLayout= nullptr;
    State m_currentState;
};

#endif // WEBVIEW_H
