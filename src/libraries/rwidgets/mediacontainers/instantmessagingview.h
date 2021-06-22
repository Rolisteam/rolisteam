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
#ifndef INSTANTMESSAGINGVIEW_H
#define INSTANTMESSAGINGVIEW_H

#include <QDockWidget>
#include <QPointer>

namespace Ui
{
class InstantMessagingView;
}
class InstantMessagingController;
class QQuickWidget;
class InstantMessagerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(InstantMessagingController* ctrl READ ctrl NOTIFY ctrlChanged)
public:
    InstantMessagerManager(QObject* object= nullptr);

    InstantMessagingController* ctrl() const;

    void setCtrl(InstantMessagingController* ctrl);

signals:
    void ctrlChanged();

private:
    QPointer<InstantMessagingController> m_ctrl;
};

class InstantMessagingView : public QWidget
{
    Q_OBJECT

public:
    explicit InstantMessagingView(InstantMessagingController* ctrl, QWidget* parent= nullptr);
    ~InstantMessagingView();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::InstantMessagingView* m_ui;
    std::unique_ptr<QQuickWidget> m_qmlViewer;
    QPointer<InstantMessagingController> m_ctrl;
};

#endif // INSTANTMESSAGINGVIEW_H
