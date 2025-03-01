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
#ifndef MINDITEM_H
#define MINDITEM_H

#include <QObject>
#include <QPointF>
#include <QQmlEngine>
#include <QVector2D>

#include "mindmap/mindmap_global.h"

namespace mindmap
{
class MINDMAP_EXPORT MindItem : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Enum only")
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(MindItem::Type type READ type CONSTANT)
public:
    enum Type
    {
        NodeType,
        LinkType,
        PackageType,
        InvalidType
    };
    Q_ENUM(Type)
    explicit MindItem(MindItem::Type type, QObject* parent= nullptr);

    QString text() const;
    QString id() const;
    bool isVisible() const;
    bool selected() const;
    MindItem::Type type() const;

public slots:
    void setText(QString text);
    void setVisible(bool op);
    void setSelected(bool isSelected);
    void setId(const QString& id);

signals:
    void idChanged();
    void visibleChanged(bool b);
    void textChanged(QString text);
    void selectedChanged();

protected:
    MindItem::Type m_type;
    QString m_id;
    QString m_text;
    bool m_visible= true;
    bool m_selected= false;
};
} // namespace mindmap
#endif // MINDITEM_H
