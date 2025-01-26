/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef PACKAGENODE_H
#define PACKAGENODE_H

#include "mindmap/data/minditem.h"
#include "mindmap/data/positioneditem.h"
#include "mindmap/mindmap_global.h"
#include <QAbstractListModel>
#include <QObject>
#include <QString>

namespace mindmap
{

class ChildrenModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role
    {
        IdRole,
        NameRole
    };
    explicit ChildrenModel(QObject* parent= nullptr);

    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    const QList<PositionedItem*>& children() const;
    bool addChild(mindmap::PositionedItem* item);
    bool removeChild(const QString& id);

private:
    QList<mindmap::PositionedItem*> m_internalChildren;
};

class MINDMAP_EXPORT PackageNode : public PositionedItem
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int minimumMargin READ minimumMargin WRITE setMinimumMargin NOTIFY minimumMarginChanged)
    Q_PROPERTY(ChildrenModel* model READ model CONSTANT FINAL)
public:
    explicit PackageNode(QObject* parent= nullptr);
    const QString& title() const;
    void setTitle(const QString& newTitle);
    const QList<PositionedItem*>& children() const;
    QStringList childrenId() const;

    int minimumMargin() const;
    void setMinimumMargin(int newMinimumMargin);

    ChildrenModel* model() const;

public slots:
    void addChild(mindmap::PositionedItem* item, bool network= false);
    void removeChild(const QString& id, bool network= false);

signals:
    void titleChanged();
    void minimumMarginChanged();
    void childAdded(const QString& id);
    void childRemoved(const QString& id);
    void childrenChanged();

private slots:
    void performLayout();

private:
    QString m_title;
    // QList<PositionedItem*> m_internalChildren;
    qreal m_minimumMargin{25.};
    std::unique_ptr<ChildrenModel> m_children;
};
} // namespace mindmap
#endif // PACKAGENODE_H
