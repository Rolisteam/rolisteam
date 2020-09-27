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

class MindNode;
class Link : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QPointF startPoint READ startPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint NOTIFY endPointChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    enum Direction
    {
        StartToEnd,
        EndToStart,
        Both
    };
    Q_ENUM(Direction)
    explicit Link(QObject* parent= nullptr);

    Direction direction() const;
    void setDirection(const Direction& direction);

    MindNode* start() const;
    void setStart(MindNode* start);

    MindNode* end() const;
    void setEnd(MindNode* end);

    QPointF endPoint() const;
    QPointF startPoint() const;

    float getLength() const;

    float getStiffness() const;
    void setStiffness(float stiffness);
    void setVisible(bool vi);
    bool isVisible() const;
    QString text() const;

    void cleanUpLink();
signals:
    void linkChanged();
    void visibleChanged();
    void directionChanged();
    void startPointChanged();
    void endPointChanged();
    void textChanged();

public slots:
    void computePosition();
    void setText(const QString& text);

private:
    Direction m_dir= StartToEnd;
    bool m_visible= true;
    QPointer<MindNode> m_start;
    QPointer<MindNode> m_end;
    QString m_text;
    float m_stiffness= 400.0f;
};

#endif // LINK_H
