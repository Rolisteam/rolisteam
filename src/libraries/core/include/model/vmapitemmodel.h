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
#ifndef VMAPITEMMODEL_H
#define VMAPITEMMODEL_H

#include "controller/item_controllers/visualitemcontroller.h"
#include <QAbstractListModel>
#include <memory>
#include <vector>
#include <core_global.h>
namespace vmap
{

class  CORE_EXPORT VmapItemModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool modified READ modified WRITE setModifiedToAllItem NOTIFY modifiedChanged)
public:
    enum CustomRole
    {
        IdRole= Qt::UserRole + 1,
        SelectedRole,
        EditableRole,
        SelectableRole,
        VisibleRole,
        OpacityRole,
        RotationRole,
        LayerRole,
        PositionRole,
        LocalGmRole,
        ColorRole,
        LockedRole,
        ToolRole,
        TypeRole,
        InitializedRole,
        ControllerRole
    };
    Q_ENUM(CustomRole);
    explicit VmapItemModel(QObject* parent= nullptr);
    virtual ~VmapItemModel();

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    bool appendItemController(vmap::VisualItemController* item);
    bool removeItemController(const QString& uuid);

    void clearData();

    std::vector<vmap::VisualItemController*> items() const;
    vmap::VisualItemController* item(const QString& id) const;

    bool modified() const;

public slots:
    void setModifiedToAllItem(bool b);
signals:
    void itemControllerAdded(vmap::VisualItemController* ctrl);
    void modifiedChanged(bool b);

private:
    std::vector<std::unique_ptr<vmap::VisualItemController>> m_items;
};
} // namespace vmap

#endif // VMAPITEMMODEL_H
