/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef HIGHLIGHTERITEM_H
#define HIGHLIGHTERITEM_H

#include <QGraphicsObject>
#include <QPointer>

#include "rwidgets_global.h"

class QPropertyAnimation;

class PreferencesManager;
/**
 * @brief displays an ellipse on maps.
 */
class RWIDGET_EXPORT HighlighterItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ getRadius WRITE setRadius NOTIFY radiusChanged)
public:
    /**
     * @brief constructor with parameters
     * @param center first point clicked by the user
     * @param either the shape is filled or not
     * @param color used for drawing it.
     */
    HighlighterItem(PreferencesManager* preference, const QPointF& center, int penSize, const QColor& penColor,
                    QGraphicsItem* parent= nullptr, bool autoDestruction= true);
    /**
     * @brief paint the ellipse at the correct position
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);

    virtual QPainterPath shape() const;

    /**
     * @brief gives the bounding rect of the ellipse
     */
    virtual QRectF boundingRect() const;

    void setRadius(qreal radius);
    qreal getRadius() const;

signals:
    void radiusChanged();

protected:
    void initAnimation(bool autoDestruction);

private:
    QPointer<PreferencesManager> m_preferences;
    qreal m_radius;
    QPointF m_center;
    QColor m_color;
    quint16 m_penSize;
    QPropertyAnimation* m_animation;
};

#endif // HIGHLIGHTERITEM_H
