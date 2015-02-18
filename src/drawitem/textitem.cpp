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
#include "textitem.h"
#include <QLineEdit>
#include <QPainter>
#include <QObject>
TextItem::TextItem()
{

}

TextItem::TextItem(QPointF& start,QLineEdit* editor,QColor& penColor,QGraphicsItem * parent)
        : VisualItem(penColor,parent)
{
    m_start = start;
    m_textEdit = editor;
    if(m_textEdit)
        m_metricFont = new QFontMetrics(m_textEdit->font());
    else
        m_metricFont = new QFontMetrics(QFont());
}
QRectF TextItem::boundingRect() const
{
    if(m_metricFont)
        return m_metricFont->boundingRect(m_text);
    else
        return QRect();
}
void TextItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if(!m_text.isEmpty())
    {
        painter->save();
        painter->setPen(m_color);
        painter->drawText(m_start,m_text);
        painter->restore();
    }

}
void TextItem::setNewEnd(QPointF& p)
{
    //QRectF tmp= m_rect;
   // m_rect.setBottomRight(p);
    //update(tmp);
}
VisualItem::ItemType TextItem::getType()
{
    return VisualItem::TEXT;
}
void TextItem::editingFinished()
{
    if(m_textEdit!=NULL)
    {
        m_text = m_textEdit->text();
        m_textEdit->setVisible(false);
        update ();
    }

}
void TextItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_text;
}

void TextItem::readData(QDataStream& in)
{
    in >> m_start;
    in >> m_text;
}
