/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "mapcontroller.h"

#include <QImage>
#include <QPainter>
#include <QVariant>

#include "data/cleveruri.h"

void clearImage(QImage& image, const QColor& color)
{
    QPainter painter(&image);
    painter.fillRect(0, 0, image.width(), image.height(), color);
}

MapController::MapController(CleverURI* uri, QObject* parent) : AbstractMediaContainerController(uri, parent) {}

void MapController::init()
{
    m_originalBackgroundImage= QImage(m_size, QImage::Format_ARGB32);
    m_backgroundImage= QImage(m_size, QImage::Format_ARGB32_Premultiplied);
    m_alphaBackground= QImage(m_size, QImage::Format_ARGB32);
    m_alphaLayer= QImage(m_size, QImage::Format_ARGB32_Premultiplied);
    m_eraseAlpha= QImage(m_size, QImage::Format_ARGB32_Premultiplied);

    clearImage(m_alphaLayer, m_fogColor);
    clearImage(m_eraseAlpha, Qt::black);
}

void MapController::saveData() const {}

void MapController::loadData() const {}

QImage MapController::backgroundImage() const
{
    return m_backgroundImage;
}

QImage MapController::originalBackgroundImage() const
{
    return m_originalBackgroundImage;
}

QImage MapController::alphaLayer() const
{
    return m_alphaLayer;
}

QImage MapController::alphaBackground() const
{
    return m_alphaBackground;
}

QImage MapController::eraseAlpha() const
{
    return m_eraseAlpha;
}

QColor MapController::fogColor() const
{
    return m_fogColor;
}

QSize MapController::size() const
{
    return m_size;
}

void MapController::setHidden(bool hidden)
{
    if(m_hidden == hidden)
        return;

    m_hidden= hidden;
    emit hiddenChanged(m_hidden);
}

bool MapController::hidden() const
{
    return m_hidden;
}

void MapController::setSize(const QSize& size)
{
    if(m_size == size)
        return;
    m_size= size;
    emit sizeChanged();
    init();
}

QColor MapController::bgColor() const
{
    return m_bgColor;
}

void MapController::setBgColor(QColor bgColor)
{
    if(m_bgColor == bgColor)
        return;

    m_bgColor= bgColor;
    emit bgColorChanged(m_bgColor);
}

Core::PermissionMode MapController::permission() const
{
    return m_permission;
}

void MapController::setPermission(Core::PermissionMode permission)
{
    if(m_permission == permission)
        return;

    m_permission= permission;
    emit permissionChanged(m_permission);
}

void MapController::setBackgroundImage(const QImage& img)
{
    m_backgroundImage= img;
    emit backgroundImageChanged(m_backgroundImage);
}

void MapController::setOriginalBackgroundImage(const QImage& img)
{
    m_originalBackgroundImage= img;
    emit originalBackgroundImageChanged(m_originalBackgroundImage);
}

void MapController::setAlphaLayer(const QImage& img)
{
    m_alphaLayer= img;
    emit alphaLayerChanged(m_alphaLayer);
}

void MapController::setAlphaBackground(const QImage& img)
{
    m_alphaBackground= img;
    emit alphaBackgroundChanged(m_alphaBackground);
}

void MapController::setEraseAlpha(const QImage& img)
{
    m_eraseAlpha= img;
    emit eraseAlphaChanged(m_eraseAlpha);
}

void MapController::setFogColor(const QColor& color)
{
    if(color == m_fogColor)
        return;
    m_fogColor= color;
    emit fogColorChanged(m_fogColor);
}
