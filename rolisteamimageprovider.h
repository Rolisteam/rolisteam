/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#ifndef ROLISTEAMIMAGEPROVIDER_H
#define ROLISTEAMIMAGEPROVIDER_H

#include <QQuickImageProvider>

class RolisteamImageProvider : public QQuickImageProvider
{
public:
    RolisteamImageProvider();
    //virtual QImage      requestImage(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);
    virtual QPixmap 	requestPixmap(const QString & id, QSize * size, const QSize & requestedSize);
   // virtual QQuickTextureFactory * 	requestTexture(const QString & id, QSize * size, const QSize & requestedSize, bool requestedAutoTransform);

    virtual void insertPix(QString key,QPixmap img);
private:
    QHash<QString,QPixmap> m_data;
};

#endif // ROLISTEAMIMAGEPROVIDER_H
