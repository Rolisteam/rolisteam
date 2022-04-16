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
#ifndef IMAGECONTROLLER_H
#define IMAGECONTROLLER_H

#include <QObject>
#include <QPixmap>
#include <memory>

#include "mediacontrollerbase.h"

class QMovie;
class ImageController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(bool fitWindow READ fitWindow WRITE setFitWindow NOTIFY fitWindowChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QPixmap pixmap READ pixmap NOTIFY pixmapChanged)
    Q_PROPERTY(Qt::CursorShape cursor READ cursor NOTIFY cursorChanged)
    Q_PROPERTY(qreal ratioV READ ratioV NOTIFY ratioVChanged)
    Q_PROPERTY(qreal ratioH READ ratioH NOTIFY ratioHChanged) // bis
    Q_PROPERTY(bool isMovie READ isMovie NOTIFY isMovieChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged)
public:
    enum Status
    {
        Playing,
        Stopped,
        Paused
    };
    Q_ENUM(Status);

    explicit ImageController(const QString& id= QString(), const QString& name= QString(), const QUrl& url= QUrl(),
                             const QByteArray& data= QByteArray(), QObject* parent= nullptr);
    virtual ~ImageController();

    bool fitWindow() const;
    qreal zoomLevel() const;
    const QPixmap& pixmap() const;
    const QPixmap scaledPixmap() const;
    Qt::CursorShape cursor() const;
    bool isMovie() const;
    Status status() const;
    QByteArray data() const;

    qreal ratioV() const;
    qreal ratioH() const;

signals:
    void fitWindowChanged();
    void zoomLevelChanged();
    void pixmapChanged();
    void cursorChanged();
    void ratioVChanged();
    void ratioHChanged();
    void isMovieChanged();
    void dataChanged();
    void statusChanged(Status s);

public slots:
    void setZoomLevel(qreal lvl);
    void setFitWindow(bool b);
    void zoomIn(qreal step= 0.2);
    void zoomOut(qreal step= 0.2);
    void setData(QByteArray& array);

    void play();
    void stop();

private:
    void checkMovie();
    void setPixmap(const QPixmap& pix);
    void setStatus(Status status);

private:
    bool m_fitWindow= true;
    QPixmap m_image;
    qreal m_zoomLevel= 1.0;
    Status m_status= Stopped;
    QByteArray m_data;
    std::unique_ptr<QMovie> m_movie;
};

#endif // IMAGECONTROLLER_H
