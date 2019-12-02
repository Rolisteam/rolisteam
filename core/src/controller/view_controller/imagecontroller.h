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

#include "abstractmediacontroller.h"

class CleverURI;

class ImageController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(bool fitWindow READ fitWindow WRITE setFitWindow NOTIFY fitWindowChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QPixmap pixmap READ pixmap NOTIFY pixmapChanged)
    Q_PROPERTY(Qt::CursorShape cursor READ cursor NOTIFY cursorChanged)
    Q_PROPERTY(qreal ratioV READ ratioV NOTIFY ratioVChanged)
    Q_PROPERTY(qreal ratioH READ ratioH NOTIFY ratioHChanged) // bis
public:
    explicit ImageController(CleverURI* uri, const QPixmap& pixmap= QPixmap(), QObject* parent= nullptr);

    bool fitWindow() const;
    qreal zoomLevel() const;
    const QPixmap& pixmap() const;
    const QPixmap scaledPixmap() const;
    Qt::CursorShape cursor() const;

    qreal ratioV() const;
    qreal ratioH() const;

    void saveData() const override;
    void loadData() const override;

signals:
    void fitWindowChanged();
    void zoomLevelChanged();
    void pixmapChanged();
    void cursorChanged();
    void ratioVChanged();
    void ratioHChanged();

public slots:
    void setZoomLevel(qreal lvl);
    void setFitWindow(bool b);
    void zoomIn(qreal step= 0.2);
    void zoomOut(qreal step= 0.2);

private:
    bool m_fitWindow= true;
    QPixmap m_image;
    qreal m_zoomLevel= 1.0;
};

#endif // IMAGECONTROLLER_H
