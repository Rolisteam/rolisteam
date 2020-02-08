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
#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QColor>
#include <QObject>
#include <QSize>

#include "abstractmediacontroller.h"

#include "vmap/vmapframe.h"

class CleverURI;
class MapController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)
    Q_PROPERTY(VMapFrame::PermissionMode permission READ permission WRITE setPermission NOTIFY permissionChanged)
public:
    explicit MapController(CleverURI* uri, const std::map<QString, QVariant>& args, QObject* parent= nullptr);

    void saveData() const override;
    void loadData() const override;

    QSize size() const;
    bool hidden() const;

    QColor bgColor() const;
    VMapFrame::PermissionMode permission() const;

public slots:
    void setSize(const QSize& size);
    void setHidden(bool hidden);
    void setBgColor(QColor bgColor);

    void setPermission(VMapFrame::PermissionMode permission);

signals:
    void sizeChanged();
    void hiddenChanged(bool hidden);
    void bgColorChanged(QColor bgColor);
    void permissionChanged(VMapFrame::PermissionMode permission);

private:
    QSize m_size;
    bool m_hidden;
    QColor m_bgColor;
    VMapFrame::PermissionMode m_permission;
};

#endif // MAPCONTROLLER_H
