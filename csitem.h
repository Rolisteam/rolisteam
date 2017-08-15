/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
#ifndef CSITEM_H
#define CSITEM_H

#include "charactersheetitem.h"
#include <QGraphicsObject>

/**
 * @brief The CSItem class is managing some item values for RCSE.
 */
class CSItem : public CharacterSheetItem
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ getX WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ getY WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ getHeight WRITE setHeight NOTIFY heightChanged)

public:
    enum BorderLine {UP=1,LEFT=2,DOWN=4,RIGHT=8,ALL=15,NONE=16};
    CSItem(QGraphicsItem* parent=0,bool addCount=true);
    virtual void setNewEnd(QPointF nend) =0;


    QColor bgColor() const;
    void setBgColor(const QColor &bgColor);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

    virtual qreal getX() const;
    virtual qreal getY() const;
    virtual qreal getWidth() const;
    virtual qreal getHeight() const;

    virtual void setX(qreal x);
    virtual void setY(qreal y);
    virtual void setWidth(qreal width);
    virtual void setHeight(qreal height);

    QRectF getRect() const;
    void setRect(const QRectF &rect);

    CSItem::BorderLine border() const;
    void setBorder(const CSItem::BorderLine &border);

    virtual QPointF mapFromScene(QPointF) =0;

    static void resetCount();
    static void setCount(int i);

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void askUpdate();

protected:
    //internal data
    QRectF m_rect;
    static int m_count;
    QColor m_bgColor;
    QColor m_textColor;
    BorderLine m_border;
    QPointF m_posPrivate;
};

#endif // CSITEM_H
