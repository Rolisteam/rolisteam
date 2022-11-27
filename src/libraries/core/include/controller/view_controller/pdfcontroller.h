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
#ifndef PDFCONTROLLER_H
#define PDFCONTROLLER_H

#include <QObject>

#include "mediacontrollerbase.h"
#include <core_global.h>
class QBuffer;
class CORE_EXPORT PdfController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(QByteArray data READ data NOTIFY dataChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
public:
    explicit PdfController(const QString& id= QString(), const QUrl& path= QUrl(), const QByteArray& data= QByteArray(),
                           QObject* parent= nullptr);
    ~PdfController() override;

    QByteArray data() const;
    QBuffer* buffer();
    qreal zoomFactor() const;

    void setData(const QByteArray& data);

public slots:
    void setZoomFactor(qreal zoom);
    void zoomIn();
    void zoomOut();
    void copyImage(const QPixmap& image);
    void copyText(const QString& text);
    void shareAsPdf();

signals:
    void dataChanged(QByteArray data);
    void sharePdf(QString id);
    void zoomFactorChanged(qreal r);

private:
    QByteArray m_data;
    qreal m_zoom= 1.0;
};

#endif // PDFCONTROLLER_H
