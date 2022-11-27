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
#ifndef IMAGSELECTORCONTROLLER_H
#define IMAGSELECTORCONTROLLER_H

#include <QBuffer>
#include <QDir>
#include <QMovie>
#include <QNetworkAccessManager>
#include <QObject>
#include <QPixmap>
#include <core_global.h>
class CORE_EXPORT ImageSelectorController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Shape shape READ shape CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(Sources sources READ sources CONSTANT)
    Q_PROPERTY(QByteArray imageData READ imageData NOTIFY imageDataChanged)
    Q_PROPERTY(QPixmap pixmap READ pixmap NOTIFY imageDataChanged)
    Q_PROPERTY(bool canDrop READ canDrop CONSTANT)
    Q_PROPERTY(bool canPaste READ canPaste CONSTANT)
    Q_PROPERTY(bool canDownload READ canDownload NOTIFY addressChanged)
    Q_PROPERTY(bool askPath READ askPath NOTIFY askPathChanged)
    Q_PROPERTY(bool validData READ validData NOTIFY imageDataChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString currentDir READ currentDir NOTIFY currentDirChanged)
    Q_PROPERTY(bool hasContentToPaste READ hasContentToPaste NOTIFY contentToPasteChanged)
    Q_PROPERTY(bool isMovie READ isMovie NOTIFY imageDataChanged)
    Q_PROPERTY(QMovie* movie READ movie NOTIFY imageDataChanged)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY rectChanged)
    Q_PROPERTY(bool rectInShape READ rectInShape NOTIFY rectChanged)
    Q_PROPERTY(bool dataInShape READ dataInShape NOTIFY imageDataChanged)
public:
    enum Shape
    {
        AnyShape,
        Square
    };
    Q_ENUM(Shape)
    enum Source
    {
        None= 0x000,
        DiskFile= 0x001,
        Clipboard= 0x002,
        DragAndDrop= 0x004,
        Web= 0x008,
        All= DiskFile | Clipboard | DragAndDrop | Web
    };
    Q_DECLARE_FLAGS(Sources, Source)
    Q_FLAG(Sources)
    explicit ImageSelectorController(bool askPath= false, Sources sources= All, Shape shape= AnyShape,
                                     const QString& directory= QDir::homePath(), QObject* parent= nullptr);

    void setImageData(const QByteArray& array);

    bool canDrop() const;
    bool canPaste() const;
    bool canDownload() const;
    bool askPath() const;
    bool validData() const;

    QPixmap pixmap() const;
    QByteArray imageData() const;
    Shape shape() const;
    ImageSelectorController::Sources sources() const;
    QString title() const;
    QString address() const;
    QString currentDir() const;
    bool hasContentToPaste() const;
    bool rectInShape() const;
    bool dataInShape() const;

    bool isMovie() const;
    QMovie* movie();

    QRect rect() const;
    QByteArray finalImageData() const;

public slots:
    void imageFromClipboard();
    void downloadImageFrom(const QUrl& url);
    void openImageFromFile();

    void setTitle(const QString& title);
    void setAddress(const QString& address);
    void setRect(const QRect& rect);

signals:
    void imageDataChanged();
    void askPathChanged();
    void currentDirChanged();
    void addressChanged();
    void titleChanged();
    void contentToPasteChanged();
    void rectChanged();

private:
    void setAddressPrivate(const QUrl& url);
    QRect computeDataGeometry() const;

private:
    QByteArray m_data;
    QBuffer m_buffer;
    QMovie m_movie;
    Sources m_sources;
    Shape m_shape= AnyShape;
    bool m_askPath= false;
    QString m_title;
    QUrl m_address;
    QString m_currentDir;
    bool m_contentToPaste= false;
    QRect m_rect;

    std::unique_ptr<QNetworkAccessManager> m_manager;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ImageSelectorController::Sources)

#endif // IMAGSELECTORCONTROLLER_H
