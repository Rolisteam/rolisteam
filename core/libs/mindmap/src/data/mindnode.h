#ifndef MINDNODE_H
#define MINDNODE_H
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
#include <QObject>
#include <QPointF>
#include <QVector2D>

class Link;
class MindNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString imageUri READ imageUri WRITE setImageUri NOTIFY imageUriChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight WRITE setContentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(bool open READ open WRITE setOpen NOTIFY openChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool hasLink READ hasLink NOTIFY hasLinkChanged)
    Q_PROPERTY(bool isDragged READ isDragged WRITE setDragged NOTIFY isDraggedChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(int styleIndex READ styleIndex WRITE setStyleIndex NOTIFY styleIndexChanged)
public:
    MindNode(QObject* parent= nullptr);
    virtual ~MindNode();

    MindNode* parentNode() const;
    void setParentNode(MindNode* parent);

    void setNextPosition(const QPointF& pos, Link* emiter);

    QPointF position() const;
    void setPosition(const QPointF& pos);

    QVector2D getVelocity() const;
    void setVelocity(const QVector2D& velocity);

    QVector2D getAcceleration() const;
    void setAcceleration(const QVector2D& acceleration);
    void applyForce(const QVector2D& force);

    QString text() const;
    int getMass() const;

    QString id() const;
    qreal contentWidth() const;
    void setContentWidth(qreal w);
    qreal contentHeight() const;
    void setContentHeight(qreal h);

    QPointF centerPoint() const;
    QRectF boundingRect() const;

    bool open() const;
    void setOpen(bool op);

    bool isVisible() const;
    void setVisible(bool op);

    void addLink(Link* link);
    void removeLink(Link* link);
    bool hasLink() const;

    QString imageUri() const;
    void setId(const QString& id);

    bool isDragged() const;
    bool selected() const;
    int styleIndex() const;

    const std::vector<QPointer<Link>>& getSubLinks() const;

    int subNodeCount() const;

public slots:
    void setText(QString text);
    void setMass(int mass);
    void setImageUri(const QString& uri);
    void setDragged(bool isdragged);
    void setSelected(bool isSelected);
    void translate(const QPointF& motion);
    void setStyleIndex(int idx);

signals:
    void positionChanged(const QPointF oldPos);
    void itemDragged(const QPointF motion);
    void textChanged(QString text);
    void contentWidthChanged();
    void contentHeightChanged();
    void openChanged();
    void visibleChanged();
    void hasLinkChanged();
    void imageUriChanged();
    void idChanged();
    void isDraggedChanged();
    void selectedChanged();
    void styleIndexChanged();

protected:
    void updatePosition();
    void computeContentSize();
    void setLinkVisibility();

private:
    MindNode* m_parent= nullptr;
    std::vector<QPointer<Link>> m_subNodelinks;
    QPointF m_position;
    QString m_id;
    QString m_text;
    bool m_open= true;
    bool m_visible= true;
    bool m_isDragged= false;
    bool m_selected= false;
    QString m_imageUri;
    int m_styleIndex= 0; // default

    qreal m_width= 100;
    qreal m_height= 20;

    int m_mass= 1;
    std::map<Link*, QPointF> m_nextPositions;
    QVector2D m_velocity;
    QVector2D m_acceleration;
};

#endif // MINDNODE_H
