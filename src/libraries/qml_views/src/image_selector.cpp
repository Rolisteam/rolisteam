/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "qml_views/image_selector.h"

#include <QPainter>

namespace qml
{
void registerType()
{
    qmlRegisterType<ImageSelector>("Profile", 1, 0, "ImageSelector");
}
} // namespace qml

ImageSelector::ImageSelector()
{
    setFlags(QQuickItem::ItemHasContents);
    setAcceptedMouseButtons(Qt::AllButtons);
}

const QByteArray& ImageSelector::imageData() const
{
    return m_imageData;
}

void ImageSelector::setImageData(const QByteArray& newImageData)
{
    if(m_imageData == newImageData)
        return;
    m_imageData= newImageData;
    m_pixmap.loadFromData(m_imageData);
    emit imageDataChanged();
    update();
}

void ImageSelector::paint(QPainter* painter)
{
    if(m_pixmap.isNull())
    {
        painter->fillRect(0, 0, width(), height(), Qt::gray);
    }
    else if(m_pixmap.height() == m_pixmap.width())
    {
        painter->drawPixmap(QRectF(0., 0., width(), height()), m_pixmap,
                            QRectF(0., 0., m_pixmap.width(), m_pixmap.height()));
    }
    else
    {
        painter->fillRect(0, 0, width(), height(), Qt::red);
    }
}

bool ImageSelector::isSquare() const
{
    return m_pixmap.width() == m_pixmap.height();
}

void ImageSelector::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "click";
    if(event->button() & Qt::LeftButton)
    {
        emit mouseClicked();
    }
    QQuickPaintedItem::mousePressEvent(event);
}

