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
#ifndef VMAP_IMAGEITEMCONTROLLER_H
#define VMAP_IMAGEITEMCONTROLLER_H

#include <QMovie>
#include <QPixmap>
#include <QRectF>
#include <QVariant>

#include "visualitemcontroller.h"
class VectorialMapController;
namespace vmap
{
class ImageController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap NOTIFY pixmapChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(qreal ratio READ ratio NOTIFY ratio)
public:
    enum Corner
    {
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft
    };
    ImageController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    QPixmap pixmap() const;
    QRectF rect() const override;
    QString path() const;
    qreal ratio() const;
    QByteArray data() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;

signals:
    void pixmapChanged();
    void rectChanged();
    void pathChanged(QString path);
    void isMovieChanged(bool movie);
    void ratio(qreal ratio);
    void dataChanged(QByteArray data);
    void rectEditFinished();

public slots:
    void setCorner(const QPointF& move, int corner) override;
    void setPath(QString path);
    void setData(QByteArray data);
    void setRect(QRectF rect);

private:
    void setImage(QPixmap pix);
    void checkMovie();

private:
    QPixmap m_pix;
    QRectF m_rect;
    QString m_path;
    QByteArray m_data;
    bool m_editingRect= false;
};
} // namespace vmap

#endif // VMAP_IMAGEITEMCONTROLLER_H
