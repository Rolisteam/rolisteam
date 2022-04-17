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
#include "controller/view_controller/imagecontroller.h"

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QMovie>

#include "iohelper.h"

constexpr char const* RESOURCE_SCHEME{"qrc"};
ImageController::ImageController(const QString& id, const QString& name, const QUrl& url, const QByteArray& data,
                                 QObject* parent)
    : MediaControllerBase(id, Core::ContentType::PICTURE, parent), m_data(data)
{
    setName(name);
    setUrl(url);
    if(m_data.isEmpty() && !url.isEmpty() && (url.isLocalFile() || url.scheme() == RESOURCE_SCHEME))
    {
        QString path
            = url.isLocalFile() ? url.toLocalFile() : QString(":%1").arg(url.toDisplayString(QUrl::RemoveScheme));

        m_data= utils::IOHelper::loadFile(path);
    }
    checkMovie();
}

ImageController::~ImageController()= default;

bool ImageController::fitWindow() const
{
    return m_fitWindow;
}

QByteArray ImageController::data() const
{
    return m_data;
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

bool ImageController::isMovie() const
{
    if(!m_movie)
        return false;
    return m_movie->isValid();
}

ImageController::Status ImageController::status() const
{
    return m_status;
}

qreal ImageController::ratioV() const
{
    if(m_image.isNull())
        return 0;
    return static_cast<qreal>(m_image.size().width()) / m_image.size().height();
}

qreal ImageController::ratioH() const
{
    if(m_image.isNull())
        return 0;
    return static_cast<qreal>(m_image.size().height()) / m_image.size().width();
}

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

void ImageController::setStatus(Status status)
{
    if(m_status == status)
        return;
    m_status= status;
    emit statusChanged(m_status);
}

void ImageController::zoomIn(qreal step)
{
    setZoomLevel(m_zoomLevel + step);
}

void ImageController::zoomOut(qreal step)
{
    setZoomLevel(m_zoomLevel - step);
}

void ImageController::play()
{
    if(!m_movie)
        return;
    if(m_status == Playing)
        m_movie->setPaused(true);
    else if(m_status == Paused)
        m_movie->start();
}

void ImageController::stop()
{
    if(!m_movie)
        return;
    m_movie->stop();
}

void ImageController::setPixmap(const QPixmap& pix)
{
    m_image= pix;
    emit pixmapChanged();
}

void ImageController::checkMovie()
{
    auto buf= new QBuffer(&m_data);
    buf->open(QIODevice::ReadOnly);

    m_movie.reset(new QMovie(buf, QByteArray(), this));
    if((m_movie->isValid()) && (m_movie->frameCount() > 1))
    {
        connect(m_movie.get(), &QMovie::updated, this, [this]() { setPixmap(m_movie->currentPixmap()); });
        connect(m_movie.get(), &QMovie::stateChanged, this, [this](const QMovie::MovieState& state) {
            switch(state)
            {
            case QMovie::NotRunning:
                setStatus(ImageController::Stopped);
                break;
            case QMovie::Paused:
                setStatus(ImageController::Paused);
                break;
            case QMovie::Running:
                setStatus(ImageController::Playing);
                break;
            }
        });
        m_movie->start();
    }
    else
    {
        m_movie.reset();
        QPixmap img;
        img.loadFromData(m_data);
        setPixmap(img);
    }
}

void ImageController::setData(QByteArray& array)
{
    if(m_data == array)
        return;
    m_data= array;
    emit dataChanged();
    checkMovie();
}

const QPixmap ImageController::scaledPixmap() const
{
    return m_image.scaled(static_cast<int>(m_image.width() * m_zoomLevel),
                          static_cast<int>(m_image.height() * m_zoomLevel));
}
