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

namespace mindmap
{
class PackageNode : public PositionedItem
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int minimumMargin READ minimumMargin WRITE setMinimumMargin NOTIFY minimumMarginChanged)
public:
    explicit PackageNode(QObject* parent= nullptr);
    const QString& title() const;
    void setTitle(const QString& newTitle);
    const QList<PositionedItem*>& children() const;
    QStringList childrenId() const;

    int minimumMargin() const;
    void setMinimumMargin(int newMinimumMargin);

public slots:
    void addChild(PositionedItem* item);

signals:
    void titleChanged();
    void minimumMarginChanged();
    void childAdded(const QString& id);
private slots:
    void performLayout();

private:
    QString m_title;
    QList<PositionedItem*> m_internalChildren;
    qreal m_minimumMargin{25.};
};
} // namespace mindmap
#endif // PACKAGENODE_H
