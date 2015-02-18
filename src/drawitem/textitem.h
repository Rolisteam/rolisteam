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
#ifndef TEXTITEM_H
#define TEXTITEM_H
#include "visualitem.h"
#include <QObject>
#include <QFontMetrics>



class QLineEdit;
/**
  * @brief displays and manages text on map, part of QGraphicsScene/view.
  * @todo add features for amend font size, text orientation,
  */
class TextItem : public QObject ,public VisualItem
{
    Q_OBJECT
public:
    /**
      * @brief Constructor with parameters
      * @param start, starting point, it represents the bottom right rectangle corner where the text willbe displayed
      */
    TextItem(QPointF& start,QLineEdit* editor,QColor& penColor,QGraphicsItem * parent = 0);
    /**
      * @brief paint the item into the scene.
      */
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    /**
      * @brief accessor to the bounding rect, helpful for focus and collision detection
      */
    virtual QRectF boundingRect() const ;
    /**
      * @brief amends the position of the end point, not really useful for this kind of graphical item.
      */
    virtual void setNewEnd(QPointF& nend);


public slots:
    /**
      * @brief called when edition is done, remove the editor and call update to draw the text as usual.
      */
    void editingFinished();

private:
    QPointF m_start;
    QString m_text;
    QLineEdit* m_textEdit;
    QFontMetrics* m_metricFont;
};

#endif // TEXTITEM_H
