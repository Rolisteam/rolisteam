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
#ifndef MINDMAPVIEW_H
#define MINDMAPVIEW_H

#include <QPointer>
#include <QQuickWidget>
#include <memory>

#include "data/mediacontainer.h"

class MindMapController;

class MindmapManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MindMapController* ctrl READ ctrl NOTIFY ctrlChanged)
public:
    MindmapManager(QObject* object= nullptr);

    MindMapController* ctrl() const;

    void setCtrl(MindMapController* ctrl);

signals:
    void ctrlChanged();

private:
    QPointer<MindMapController> m_ctrl;
};

class MindMapView : public MediaContainer
{
    Q_OBJECT
public:
    MindMapView(MindMapController* ctrl, QWidget* parent= nullptr);

private:
    std::unique_ptr<QQuickWidget> m_qmlViewer;
    QPointer<MindMapController> m_ctrl;
};

#endif // MINDMAPVIEW_H
