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
#ifndef MINDNODE_H
#define MINDNODE_H
#include <QObject>
#include <QPointF>
#include <QQmlEngine>
#include <QVector2D>

#include "mindmap/data/positioneditem.h"
#include "mindmap/mindmap_global.h"

namespace mindmap
{
class MINDMAP_EXPORT MindNode : public PositionedItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MindNode)
    Q_PROPERTY(QString imageUri READ imageUri WRITE setImageUri NOTIFY imageUriChanged)
    Q_PROPERTY(int styleIndex READ styleIndex WRITE setStyleIndex NOTIFY styleIndexChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(QString tagsText READ tagsText WRITE setTagsText NOTIFY tagsChanged)
public:
    MindNode(QObject* parent= nullptr);
    virtual ~MindNode();

    QString imageUri() const;
    int styleIndex() const;
    QString description() const;
    const QStringList& tags() const;
    const QString tagsText() const;
    QString toString(bool withLabel);

public slots:
    void setTagsText(const QString& newTagsText);
    void setImageUri(const QString& uri);
    void setStyleIndex(int idx);
    void setDescription(const QString& newDescription);
    void setTags(const QStringList& newTags);

signals:
    void imageUriChanged();
    void styleIndexChanged();
    void descriptionChanged();
    void tagsChanged();

protected:
    void updatePosition();
    void computeContentSize();
    void setLinkVisibility();
    void distantFromParent(const QRectF& boundingRect);

private:
    QString m_imageUri;
    int m_styleIndex= 0; // default
    QString m_description;
    QStringList m_tags;
};
} // namespace mindmap
#endif // MINDNODE_H
