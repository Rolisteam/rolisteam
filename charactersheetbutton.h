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
#ifndef CHARACTERSHEETBUTTON_H
#define CHARACTERSHEETBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsItem>
#include "charactersheetitem.h"
#include "csitem.h"

class CharacterSheetButton : public CSItem
{
        Q_OBJECT
public:
    CharacterSheetButton(QPointF topleft,QGraphicsItem* parent = 0);
    CharacterSheetButton(QGraphicsItem* parent = 0);

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId) const;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var);

    virtual void save(QJsonObject& json,bool exp=false);
    virtual void load(QJsonObject &json,QList<QGraphicsScene*> scene);

    virtual void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec);

    QString getId() const;
    void setId(const QString &id);

    QString getTitle() const;
    void setTitle(const QString &title);

    QColor getBgColor() const;
    void setBgColor(const QColor &bgColor);

    QColor getTextColor() const;
    void setTextColor(const QColor &textColor);

    QRectF getRect() const;
    void setRect(const QRectF &rect);
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual void setNewEnd(QPointF nend);
    QRectF boundingRect() const;

    CharacterSheetItem* getChildAt(QString) const;

signals:
    void updateNeeded(CSItem* c);

private:
    void init();

private:
    QColor m_bgColor;
    QColor m_textColor;
    QRectF m_rect;
    static int m_count;

};

#endif // CHARACTERSHEETBUTTON_H
