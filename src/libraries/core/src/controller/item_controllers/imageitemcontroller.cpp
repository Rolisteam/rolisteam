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
#include "controller/item_controllers/imageitemcontroller.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "worker/utilshelper.h"
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QMovie>

namespace vmap
{
QByteArray readImage(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return {};
    return file.readAll();
}

ImageItemController::ImageItemController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                                         QObject* parent)
    : VisualItemController(VisualItemController::IMAGE, params, ctrl, parent)
{

    m_tool= Core::SelectableTool::IMAGE;

    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    hu::setParamIfAny<QString>(cv::KEY_PATH, params,
                               [this](const QString& path)
                               {
                                   setPath(path);
                                   setData(readImage(path));
                               });
    hu::setParamIfAny<QByteArray>(cv::KEY_DATA, params, std::bind(&ImageItemController::setData, this, _1));

    checkMovie();

    hu::setParamIfAny<QRectF>(cv::KEY_RECT, params, std::bind(&ImageItemController::setRect, this, _1));

    if(rect().isNull())
        setRect(m_pix.rect());

    connect(this, &vmap::ImageItemController::pixmapChanged, this, [this] { setModified(); });
    connect(this, &vmap::ImageItemController::dataChanged, this, [this] { setModified(); });
    connect(this, &vmap::ImageItemController::rectChanged, this, [this] { setModified(); });
    connect(this, &vmap::ImageItemController::pathChanged, this, [this] { setModified(); });
}

QRectF ImageItemController::rect() const
{
    return m_rect;
}

QPixmap ImageItemController::pixmap() const
{
    return m_pix;
}

void ImageItemController::setRect(QRectF rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    emit rectChanged();
    m_editingRect= true;
}

void ImageItemController::setImage(QPixmap pix)
{
    if(pix.toImage() == m_pix.toImage())
        return;
    m_pix= pix;
    emit pixmapChanged();
}

void ImageItemController::setCorner(const QPointF& move, int corner, Core::TransformType tt)
{
    if(move.isNull())
        return;

    auto rect= m_rect;
    qreal x2= rect.right();
    qreal y2= rect.bottom();
    qreal x= rect.x();
    qreal y= rect.y();
    switch(corner)
    {
    case TopLeft:
        x+= move.x();
        y+= move.y();
        break;
    case TopRight:
        x2+= move.x();
        y+= move.y();
        break;
    case BottomRight:
        x2+= move.x();
        y2+= move.y();
        break;
    case BottomLeft:
        x+= move.x();
        y2+= move.y();
        break;
    }
    rect.setCoords(x, y, x2, y2);
    if(!rect.isValid())
        rect= rect.normalized();
    setRect(rect);
}

void ImageItemController::aboutToBeRemoved()
{
    emit removeItem();
}

QString ImageItemController::path() const
{
    return m_path;
}

qreal ImageItemController::ratio() const
{
    if(m_pix.isNull() || (m_pix.height() == 0))
        return 1.0;
    return m_pix.width() / m_pix.height();
}

QByteArray ImageItemController::data() const
{
    return m_data;
}

void ImageItemController::setData(QByteArray data)
{
    if(m_data == data)
        return;

    m_data= data;
    emit dataChanged(m_data);
}
void ImageItemController::setPath(QString path)
{
    if(m_path == path)
        return;

    m_path= path;
    emit pathChanged(m_path);
}

void ImageItemController::checkMovie()
{
    auto buf= new QBuffer(&m_data);
    buf->open(QIODevice::ReadOnly);

    auto movie= new QMovie(buf, QByteArray(), this);
    if((movie->isValid()) && (movie->frameCount() > 1))
    {
        connect(movie, &QMovie::updated, this, [this, movie]() { setImage(movie->currentPixmap()); });
        movie->start();
        // m_rect= m_movie->frameRect();
    }
    else
    {
        delete movie;
        QPixmap img;
        img.loadFromData(m_data);
        setImage(img);
    }
}

void ImageItemController::endGeometryChange()
{
    VisualItemController::endGeometryChange();

    if(m_editingRect)
    {
        emit rectEditFinished();
        m_editingRect= false;
    }
}
} // namespace vmap
