/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "imagecontroller.h"

#include "data/cleveruri.h"

ImageController::ImageController(CleverURI* uri, QObject* parent) : AbstractMediaContainerController(uri, parent)
{
    m_image= QPixmap(uri->getUri());
}

bool ImageController::fitWindow() const
{
    return m_fitWindow;
}

qreal ImageController::zoomLevel() const
{
    return m_zoomLevel;
}

const QPixmap& ImageController::pixmap() const
{
    return m_image;
}

Qt::CursorShape ImageController::cursor() const
{
    return m_fitWindow ? Qt::ArrowCursor : Qt::OpenHandCursor;
}

qreal ImageController::ratioV() const
{
    return static_cast<qreal>(m_image.size().width()) / m_image.size().height();
}

qreal ImageController::ratioH() const
{
    return static_cast<qreal>(m_image.size().height()) / m_image.size().width();
}

void ImageController::saveData() const {}

void ImageController::loadData() const {}

void ImageController::setZoomLevel(qreal lvl)
{
    lvl= qBound(0.2, lvl, 4.0);
    if(qFuzzyCompare(lvl, m_zoomLevel))
        return;

    m_zoomLevel= lvl;
    emit zoomLevelChanged();
}

void ImageController::setFitWindow(bool b)
{
    if(m_fitWindow == b)
        return;
    m_fitWindow= b;
    emit fitWindowChanged();
    emit cursorChanged();
}

void ImageController::zoomIn(qreal step)
{
    setZoomLevel(m_zoomLevel + step);
}

void ImageController::zoomOut(qreal step)
{
    setZoomLevel(m_zoomLevel - step);
}

const QPixmap ImageController::scaledPixmap() const
{
    return m_image.scaled(static_cast<int>(m_image.width() * m_zoomLevel),
                          static_cast<int>(m_image.height() * m_zoomLevel));
}
