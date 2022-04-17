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
#ifndef LINK_H
#define LINK_H

#include <QObject>
#include <QPointF>
#include <QPointer>

#include "mindmaptypes.h"

#include "mindmap/mindmap_global.h"

namespace mindmap
{

class MindNode;
class MINDMAP_EXPORT Link : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(mindmap::ArrowDirection direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QPointF startPoint READ startPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint NOTIFY endPointChanged)
    Q_PROPERTY(QPointF p1 READ p1 NOTIFY startPointChanged)
    Q_PROPERTY(QPointF p2 READ p2 NOTIFY endPointChanged)
    Q_PROPERTY(QString p1Id READ p1Id NOTIFY startPointChanged)
    Q_PROPERTY(QString p2Id READ p2Id NOTIFY endPointChanged)
public:
    /*enum Role
    {
        P1_POSITION,
        P2_POSITION,
        TEXT,
        ARROW
    };
    Q_ENUM(Role)*/
    explicit Link(QObject* parent= nullptr);

    mindmap::ArrowDirection direction() const;
    void setDirection(const mindmap::ArrowDirection& direction);

    MindNode* start() const;
    void setStart(MindNode* start);

    MindNode* endNode() const;
    void setEnd(MindNode* end);

    QString id() const;

    QPointF endPoint() const;
    QPointF startPoint() const;
    QString p1Id() const;
    QString p2Id() const;

    QPointF p1() const;
    QPointF p2() const;

    float getLength() const;

    float getStiffness() const;
    void setStiffness(float stiffness);
    void setVisible(bool vi);
    bool isVisible() const;
    QString text() const;

    void cleanUpLink();

    static void setMinimumLenght(float v);

    QString toString(bool withLabel);
signals:
    void visibleChanged();
    void directionChanged();
    void startPointChanged();
    void endPointChanged();
    void startPositionChanged();
    void endPositionChanged();
    void textChanged();
    void idChanged();

public slots:
    void computePosition();
    void setText(const QString& text);
    void setId(const QString& text);

private:
    QPointF computePoint(bool) const;

private:
    mindmap::ArrowDirection m_dir= mindmap::ArrowDirection::StartToEnd;
    bool m_visible= true;
    QPointer<MindNode> m_start;
    QPointer<MindNode> m_end;
    QString m_text;
    QString m_uuid;
    float m_stiffness= 400.0f;
    static float m_minimunLenght;
};
} // namespace mindmap
#endif // LINK_H
