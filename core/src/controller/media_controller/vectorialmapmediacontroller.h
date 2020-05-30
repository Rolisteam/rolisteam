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
#ifndef VECTORIALMAPMEDIACONTROLLER_H
#define VECTORIALMAPMEDIACONTROLLER_H

#include "mediamanagerbase.h"
#include "vmap/vtoolbar.h"
#include <memory>
#include <vector>

class VectorialMapController;
class NetworkController;
class VMapUpdater;
class VectorialMapMediaController : public MediaManagerBase
{
    Q_OBJECT

public:
    VectorialMapMediaController();
    ~VectorialMapMediaController() override;

    VectorialMapController* currentVMap() const;

    bool openMedia(const QString& uuid, const std::map<QString, QVariant>& args) override;
    void closeMedia(const QString& id) override;
    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
    int managerCount() const override;

    Core::SelectableTool tool() const;

    void addImageToMap(const QPixmap& map);

    std::vector<VectorialMapController*> controllers() const;

signals:
    void npcNumberChanged(int);
    void toolColorChanged(const QColor&);
    void opacityChanged(qreal);
    void editionModeChanged(Core::EditionMode mode);
    void vmapControllerCreated(VectorialMapController* media);

    void gridSizeChanged(int size);
    void gridAboveChanged(bool);
    void gridColorChanged(QColor);
    void gridScaleChanged(double);
    void gridUnitChanged(Core::ScaleUnit unit);
    void gridVisibilityChanged(bool);

    void backgroundColorChanged(QColor);
    void layerChanged(Core::Layer);
    void collisionChanged(bool);
    void characterVision(bool);
    void visibilityModeChanged(Core::VisibilityMode mode);
    void permissionModeChanged(Core::PermissionMode mode);

public slots:
    void setTool(Core::SelectableTool tool);
    void setColor(const QColor& color);
    void setBackgroundColor(const QColor& color);
    void setEditionMode(Core::EditionMode mode);
    void setNpcNumber(int number);
    void setNpcName(const QString& name);
    void setOpacity(qreal opacity);
    void setPenSize(int penSize);
    void setCharacterVision(bool b);
    void setGridUnit(Core::ScaleUnit unit);
    void setGridSize(int size);
    void setGridScale(double scale);
    void setGridVisibility(bool visible);
    void setGridAbove(bool above);
    void setVisibilityMode(Core::VisibilityMode mode);
    void setPermissionMode(Core::PermissionMode mode);
    void setLayer(Core::Layer layer);
    void setGridPattern(Core::GridPattern pattern);
    void setCollision(bool b);
    void showTransparentItem();

private:
    void initializeOwnedVMap(VectorialMapController* ctrl);
    void updateProperties();
    VectorialMapController* addVectorialMapController(const QString& uuid, const QHash<QString, QVariant>& params);

private:
    std::vector<std::unique_ptr<VectorialMapController>> m_vmaps;
    std::unique_ptr<VMapUpdater> m_updater;
};

#endif // VECTORIALMAPMEDIACONTROLLER_H
