/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     https://rolisteam.org/                                           *
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
#include <QPointer>
#include <QWebEngineView>
#include <QWidget>

#include "rwidgets/mediacontainers/mediacontainer.h"
#include "network/networkmessagewriter.h"
namespace Ui
{
class WebView;
}

class WebpageController;
class WebView : public MediaContainer
{
    Q_OBJECT

public:
    enum ShareType
    {
        URL,
        HTML
    };

    explicit WebView(WebpageController* ctrl, QWidget* parent= nullptr);
    virtual ~WebView();

protected:
    void mousePressEvent(QMouseEvent* mouseEvent);
    void showEvent(QShowEvent* event);
    void updateTitle();

private:
    QPointer<WebpageController> m_webCtrl;
    Ui::WebView* m_ui;
};

#endif // WEBVIEW_H
