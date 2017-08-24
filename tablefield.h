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
#ifndef TABLEFIELD_H
#define TABLEFIELD_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsItem>
#include "charactersheetitem.h"
#include "field.h"

#ifdef RCSE
#include "tablecanvasfield.h"
#else
class TableCanvasField : public QGraphicsObject
{
    TableCanvasField();
};
#endif



/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class TableField : public Field
{
    Q_OBJECT
public:

    enum TextAlign {TopRight, TopMiddle, TopLeft, CenterRight,CenterMiddle,CenterLeft,BottomRight,BottomMiddle,BottomLeft};

    explicit TableField(bool addCount = true,QGraphicsItem* parent = 0);
    explicit TableField(QPointF topleft,bool addCount = true,QGraphicsItem* parent = 0);


    void generateQML(QTextStream &out, CharacterSheetItem::QMLSection sec);
protected:
    void init();


protected:
    TableCanvasField* m_tableCanvasField;
};

#endif // TABLEFIELD_H
