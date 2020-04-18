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
#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <QColor>
#include <QObject>
#include <QSize>

#include "abstractmediacontroller.h"

#include "vmap/vmapframe.h"

class CleverURI;
class MapController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged)
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)
    Q_PROPERTY(QColor fogColor READ fogColor WRITE setFogColor NOTIFY fogColorChanged)
    Q_PROPERTY(Core::PermissionMode permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(QImage backgroundImage READ backgroundImage WRITE setBackgroundImage NOTIFY backgroundImageChanged)
    Q_PROPERTY(QImage originalBackgroundImage READ originalBackgroundImage WRITE setOriginalBackgroundImage NOTIFY
                   originalBackgroundImageChanged)
    Q_PROPERTY(QImage alphaLayer READ alphaLayer WRITE setAlphaLayer NOTIFY alphaLayerChanged)
    Q_PROPERTY(QImage alphaBackground READ alphaBackground WRITE setAlphaBackground NOTIFY alphaBackgroundChanged)
    Q_PROPERTY(QImage eraseAlpha READ eraseAlpha WRITE setEraseAlpha NOTIFY eraseAlphaChanged)
public:
    explicit MapController(CleverURI* uri, QObject* parent= nullptr);

    void saveData() const override;
    void loadData() const override;

    QImage backgroundImage() const;
    QImage originalBackgroundImage() const;
    QImage alphaLayer() const;
    QImage alphaBackground() const;
    QImage eraseAlpha() const;

    QColor fogColor() const;

    QSize size() const;
    bool hidden() const;

    QColor bgColor() const;
    Core::PermissionMode permission() const;

public slots:
    void setSize(const QSize& size);
    void setHidden(bool hidden);
    void setBgColor(QColor bgColor);

    void setPermission(Core::PermissionMode permission);

    void setBackgroundImage(const QImage& img);
    void setOriginalBackgroundImage(const QImage& img);
    void setAlphaLayer(const QImage& img);
    void setAlphaBackground(const QImage& img);
    void setEraseAlpha(const QImage& img);
    void setFogColor(const QColor& color);

signals:
    void sizeChanged();
    void hiddenChanged(bool hidden);
    void bgColorChanged(QColor bgColor);
    void permissionChanged(Core::PermissionMode permission);
    void backgroundImageChanged(QImage);
    void alphaLayerChanged(QImage);
    void alphaBackgroundChanged(QImage);
    void eraseAlphaChanged(QImage);
    void originalBackgroundImageChanged(QImage);
    void fogColorChanged(QColor);

private:
    void init();

private:
    QSize m_size;
    bool m_hidden;
    QColor m_bgColor;
    QColor m_fogColor;
    Core::PermissionMode m_permission;

    QImage m_backgroundImage;
    QImage m_originalBackgroundImage;
    QImage m_alphaBackground;
    QImage m_alphaLayer;
    QImage m_eraseAlpha;
};

#endif // MAPCONTROLLER_H
