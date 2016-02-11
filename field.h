/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#ifndef FIELD_H
#define FIELD_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsItem>
#include "item.h"
#include "csitem.h"

class Field : public CSItem
{
    Q_OBJECT
public:
    enum BorderLine {UP=1,LEFT=2,DOWN=4,RIGHT=8,ALL=15,NONE=16};
    enum TextAlign {ALignLEFT,ALignRIGHT,ALignCENTER};


    explicit Field(QGraphicsItem* parent = 0);
    explicit Field(QPointF topleft,QGraphicsItem* parent = 0);

    void drawField();
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );




    QString key() const;
    void setKey(const QString &key);

    QSize size() const;
    void setSize(const QSize &size);

    Field::BorderLine border() const;
    void setBorder(const Field::BorderLine &border);

    QFont font() const;
    void setFont(const QFont &font);

    QRectF boundingRect() const;


    virtual QVariant getValue(Item::ColumnId) const;
    virtual void setValue(Item::ColumnId id, QVariant var);

    virtual void save(QJsonObject& json,bool exp=false);
    virtual void load(QJsonObject &json,QGraphicsScene* scene);

    virtual void generateQML(QTextStream& out,Item::QMLSection sec);

    QStringList getAvailableValue() const;
    void setAvailableValue(const QStringList &availableValue);

signals:
    void updateNeeded(CSItem* c);

protected:
    void init();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent* ev);

private:
    QString m_key;
    BorderLine m_border;

    QFont  m_font;
    TextAlign m_textAlign;
    QStringList m_availableValue;

};

#endif // FIELD_H
