/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "charactersheet/charactersheetitem.h"
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QString>

#include <charactersheet/charactersheet_global.h>

class QGraphicsItem;
/**
 * @brief The CSItem class is managing some item values for RCSE.
 */
class CHARACTERSHEET_EXPORT CSItem : public CharacterSheetItem
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(BorderLine border READ border WRITE setBorder NOTIFY borderChanged)
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)

public:
    enum BorderLine
    {
        UP= 1,
        LEFT= 2,
        DOWN= 4,
        RIGHT= 8,
        ALL= 15,
        NONE= 16
    };
    CSItem(CharacterSheetItemType type, QGraphicsItem* parent= nullptr, bool addCount= true);

    QColor bgColor() const;
    QColor textColor() const;
    virtual qreal x() const;
    virtual qreal y() const;
    virtual qreal width() const;
    virtual qreal height() const;
    CSItem::BorderLine border() const;

    static void resetCount();
    static void setCount(int i);

    CharacterSheetItem* getChildFromId(const QString& id) const override;
public slots:
    void setBgColor(const QColor& bgColor);
    void setTextColor(const QColor& textColor);
    virtual void setX(qreal x);
    virtual void setY(qreal y);
    virtual void setWidth(qreal width);
    virtual void setHeight(qreal height);
    void setBorder(const CSItem::BorderLine& border);

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void askUpdate();
    void borderChanged();
    void bgColorChanged();

protected:
    QSize m_rect;
    QPointF m_pos;

    static int m_count;
    QColor m_bgColor;
    QColor m_textColor;
    BorderLine m_border;
    QPointF m_posPrivate;
};

#endif // CSITEM_H
