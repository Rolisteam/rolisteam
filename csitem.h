/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "item.h"
#include <QGraphicsItem>

class CSItem : public QGraphicsObject,public Item
{
        Q_OBJECT
public:
    CSItem(QGraphicsItem* parent=0);
    virtual void setNewEnd(QPointF nend);


    QColor bgColor() const;
    void setBgColor(const QColor &bgColor);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

protected:
    QString m_id;

    //internal data
    QRectF m_rect;
    static int m_count;
    QColor m_bgColor;
    QColor m_textColor;
};

#endif // CSITEM_H
