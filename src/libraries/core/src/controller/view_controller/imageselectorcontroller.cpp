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
#include "controller/view_controller/imageselectorcontroller.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QNetworkReply>

#include "utils/iohelper.h"
#include "worker/iohelper.h"

ImageSelectorController::ImageSelectorController(bool askPath, Sources sources, Shape shape, const QString& directory,
                                                 QObject* parent)
    : QObject(parent), m_sources(sources), m_shape(shape), m_askPath(askPath), m_currentDir(directory)
{
#ifdef HAVE_QT_NETWORK
    m_manager.reset(new QNetworkAccessManager());
    connect(m_manager.get(), &QNetworkAccessManager::finished, this,
            [this](QNetworkReply* reply) { setImageData(reply->readAll()); });
#endif

    auto clipboard= QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &ImageSelectorController::contentToPasteChanged);
}

bool ImageSelectorController::canDrop() const
{
    return m_sources & DragAndDrop;
}

bool ImageSelectorController::canPaste() const
{
    return m_sources & Clipboard;
}

bool ImageSelectorController::canDownload() const
{
#ifdef HAVE_QT_NETWORK
    return m_sources & Web;
#else
    return false;
#endif
}

bool ImageSelectorController::askPath() const
{
    return m_askPath;
}

bool ImageSelectorController::validData() const
{
    auto const& validMovie= isMovie();
    auto const& validImg= !pixmap().isNull();

    return (validImg || validMovie);
}

QPixmap ImageSelectorController::pixmap() const
{
    return QPixmap::fromImage(QImage::fromData(m_data));
}

QByteArray ImageSelectorController::imageData() const
{
    return m_data;
}

ImageSelectorController::Shape ImageSelectorController::shape() const
{
    return m_shape;
}

ImageSelectorController::Sources ImageSelectorController::sources() const
{
    return m_sources;
}

QString ImageSelectorController::title() const
{
    return m_title;
}

QString ImageSelectorController::address() const
{
    return m_address.toString();
}

QString ImageSelectorController::currentDir() const
{
    return m_currentDir;
}

bool ImageSelectorController::hasContentToPaste() const
{
    auto clipboard= QGuiApplication::clipboard();
    if(!clipboard)
        return false;
    const QMimeData* mimeData= clipboard->mimeData();
    if(!mimeData)
        return false;

    return mimeData->hasImage();
}

bool ImageSelectorController::dataInShape() const
{
    auto dataGeo = computeDataGeometry();
    bool res = dataGeo.isValid();
    if(m_shape == Square)
        res &= (dataGeo.height() == dataGeo.width());
    return res;
}

bool ImageSelectorController::rectInShape() const
{
    bool res = computeDataGeometry().contains(m_rect);
    if(m_shape == Square)
        res &= (m_rect.height() == m_rect.width());
    return res;
}

bool ImageSelectorController::isMovie() const
{
    return m_movie.isValid() && m_movie.frameCount() > 1;
}

void ImageSelectorController::imageFromClipboard()
{
    auto clipboard= QGuiApplication::clipboard();

    if(!clipboard)
        return;
    const QMimeData* mimeData= clipboard->mimeData();
    if(!mimeData)
        return;

    if(mimeData->hasImage())
    {
        auto pix= qvariant_cast<QPixmap>(mimeData->imageData());
        if(pix.isNull())
            return;
        setImageData(IOHelper::pixmapToData(pix));
    }
    else if(mimeData->hasUrls())
    {
        auto urls= mimeData->urls();
        if(urls.isEmpty())
            return;
        auto url= urls.first();
        downloadImageFrom(url);
    }
}

void ImageSelectorController::downloadImageFrom(const QUrl& url)
{
    setAddressPrivate(url);
    if(m_address.isLocalFile())
    {
        openImageFromFile();
    }
    else
    {
#ifdef HAVE_QT_NETWORK
        setTitle(url.fileName());
        m_manager->get(QNetworkRequest(m_address));
#endif
    }
}

QMovie* ImageSelectorController::movie()
{
    return &m_movie;
}

QRect ImageSelectorController::rect() const
{
    return m_rect;
}

void ImageSelectorController::openImageFromFile()
{
    auto local= m_address.toLocalFile();
    QFileInfo info(local);
    setTitle(info.baseName());
    setImageData(utils::IOHelper::loadFile(local));
}

QByteArray ImageSelectorController::finalImageData() const
{
    auto data= imageData();
    if(m_shape != AnyShape)
    {
        QPixmap map= pixmap();
        data= IOHelper::pixmapToData(map.copy(m_rect));
    }
    return data;
}

void ImageSelectorController::setTitle(const QString& title)
{
    if(title == m_title)
        return;
    m_title= title;
    emit titleChanged();
}

void ImageSelectorController::setAddressPrivate(const QUrl& url)
{
    if(m_address == url)
        return;
    m_address= url;
    emit addressChanged();
}

QRect ImageSelectorController::computeDataGeometry() const
{
    QRect res;
    auto pix = pixmap();
    if(!pix.isNull())
        res = pix.rect();
    else if(isMovie())
        res = m_movie.frameRect();

    return res;
}

void ImageSelectorController::setAddress(const QString& address)
{
    if(this->address() == QUrl::fromUserInput(address).toString())
        return;
    m_address= QUrl::fromUserInput(address);
    emit addressChanged();
}

void ImageSelectorController::setRect(const QRect& rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    emit rectChanged();
}

void ImageSelectorController::setImageData(const QByteArray& array)
{
    if(array == m_data)
        return;
    m_data= array;

    m_buffer.setData(m_data);
    m_movie.setDevice(&m_buffer);
    emit imageDataChanged();
}
