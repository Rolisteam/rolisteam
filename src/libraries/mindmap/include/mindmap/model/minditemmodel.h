/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#ifndef MINDITEMMODEL_H
#define MINDITEMMODEL_H

#include <QAbstractListModel>
#include <QPointer>

#include "mindmap/data/minditem.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/packagenode.h"
#include "mindmap/mindmap_global.h"

namespace mindmap
{
class ImageModel;
class MINDMAP_EXPORT MindItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Model provided by controller")
    Q_PROPERTY(QRectF contentRect READ contentRect NOTIFY geometryChanged)
    Q_PROPERTY(
        int defaultStyleIndex READ defaultStyleIndex WRITE setDefaultStyleIndex NOTIFY defaultStyleIndexChanged FINAL)
public:
    enum Roles
    {
        // Curves
        Label= Qt::UserRole + 1,
        Visible,
        Selected,
        Type,
        Uuid,
        Object,
        HasPicture,
        LinkStartPosition,
        LinkPositionFromSpacing

    };
    Q_ENUM(Roles)
    explicit MindItemModel(ImageModel* imgModel, QObject* parent= nullptr);
    ~MindItemModel() override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QRectF contentRect() const;
    std::vector<mindmap::MindItem*>& items(mindmap::MindItem::Type type);
    std::vector<mindmap::PositionedItem*> positionnedItems() const;
    mindmap::MindItem* item(const QString& id) const;
    mindmap::PositionedItem* positionItem(const QString& id) const;
    std::vector<mindmap::LinkController*> sublink(const QString& id) const;
    QString idFromIndex(int index) const;
    void appendItem(const QList<mindmap::MindItem*>& node, bool network= false);
    int defaultStyleIndex() const;
    void setDefaultStyleIndex(int newDefaultStyleIndex);

public slots:
    mindmap::MindItem* createItem(MindItem::Type type);
    bool removeItem(const mindmap::MindItem* node);
    void openItem(const QString& id, bool status);
    void setImageUriToNode(const QString& id);
    void update(const QString& id, int role);
    void clear();

signals:
    void geometryChanged();
    void itemAdded(const QList<mindmap::MindItem*> nodes);
    void itemRemoved(QStringList ids);
    void latestInsertedPackage(mindmap::PackageNode* package);

    void defaultStyleIndexChanged();

    void baseUrlChanged();

private:
    void removeAllSubItem(const mindmap::PositionedItem* item, QSet<QString>& items);

private:
    std::vector<mindmap::MindItem*> m_links;
    std::vector<mindmap::MindItem*> m_packages;
    std::vector<mindmap::MindItem*> m_nodes;
    QPointer<ImageModel> m_imgModel;
    int m_defaultStyleIndex= 0;
};
} // namespace mindmap
#endif // MINDITEMMODEL_H
