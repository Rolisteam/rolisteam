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
#include "charactersheetitem.h"
#include "csitem.h"

#ifdef RCSE
#include "canvasfield.h"
#else
class CanvasField : public QGraphicsObject
{
    CanvasField();
};
#endif



/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class Field : public CSItem
{
    Q_OBJECT
public:

    enum TextAlign {TopRight, TopMiddle, TopLeft, CenterRight,CenterMiddle,CenterLeft,BottomRight,BottomMiddle,BottomLeft};

    explicit Field(QGraphicsItem* parent = 0);
    explicit Field(QPointF topleft,QGraphicsItem* parent = 0);
    virtual ~Field();


    //void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    QSize size() const;
    void setSize(const QSize &size);

    QFont font() const;
    void setFont(const QFont &font);

    //QRectF boundingRect() const;

    CharacterSheetItem* getChildAt(QString) const;

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId,int role) const;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var);

    virtual void save(QJsonObject& json,bool exp=false);
    virtual void load(QJsonObject &json,QList<QGraphicsScene*> scene);
    /**
     * @brief saveDataItem
     * @param json
     */
    virtual void saveDataItem(QJsonObject& json);
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void loadDataItem(QJsonObject& json);

    virtual QPointF mapFromScene(QPointF);

    virtual void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec);

    QStringList getAvailableValue() const;
    void setAvailableValue(const QStringList &availableValue);

    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const;

    void copyField(CharacterSheetItem* );

    bool getClippedText() const;
    void setClippedText(bool clippedText);

    //virtual QPainterPath shape() const;
    void setTextAlign(const TextAlign &textAlign);


    Field::TextAlign getTextAlignValue();

    virtual void setNewEnd(QPointF nend);

    CanvasField* getCanvasField() const;
    void setCanvasField(CanvasField* canvasField);

    void initGraphicsItem();
signals:
    void updateNeeded(CSItem* c);
    //void valueChanged(QString);

protected:
    void init();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent* ev);

    QPair<QString, QString> getTextAlign();
private:
    QString getQMLItemName();
private:
    QFont  m_font;
    TextAlign m_textAlign;
    QStringList m_availableValue;
    bool m_clippedText;
    CanvasField* m_canvasField;
};

#endif // FIELD_H
