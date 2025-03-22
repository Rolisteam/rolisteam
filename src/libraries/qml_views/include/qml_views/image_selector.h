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
#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QPixmap>
#include <QQuickPaintedItem>

#include <qml_views/qmlviews_global.h>

namespace qml
{
QMLVIEWS_EXPORT void registerType();
}

class QMLVIEWS_EXPORT ImageSelector : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QByteArray imageData READ imageData WRITE setImageData NOTIFY imageDataChanged)
    Q_PROPERTY(bool isSquare READ isSquare NOTIFY imageDataChanged)
    Q_PROPERTY(bool hasImage READ hasImage NOTIFY imageDataChanged FINAL)
    QML_ELEMENT
public:
    ImageSelector();

    const QByteArray& imageData() const;
    void setImageData(const QByteArray& newImageData);
    void paint(QPainter* painter);
    bool isSquare() const;
    bool hasImage() const;
    void setHasImage(bool newHasImage);

signals:
    void imageDataChanged();
    void mouseClicked();

    void hasImageChanged();

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QByteArray m_imageData;
    QPixmap m_pixmap;
};

#endif
