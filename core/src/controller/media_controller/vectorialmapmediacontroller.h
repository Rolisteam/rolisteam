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

#include "mediacontrollerinterface.h"
#include "vmap/vtoolbar.h"
#include <memory>
#include <vector>

class VectorialMapController;
class VectorialMapMediaController : public MediaControllerInterface
{
    Q_OBJECT

public:
    VectorialMapMediaController();
    ~VectorialMapMediaController() override;

    VectorialMapController* currentVMap() const;

    CleverURI::ContentType type() const override;
    bool openMedia(CleverURI* uri, const std::map<QString, QVariant>& args) override;
    void closeMedia(const QString& id) override;
    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    Core::SelectableTool tool() const;

signals:
    void npcNumberChanged(int);
    void colorChanged(const QColor&);
    void opacityChanged(qreal);
    void editionModeChanged(Core::EditionMode mode);
    void vmapControllerCreated(VectorialMapController* media);

public slots:
    void setTool(Core::SelectableTool tool);
    void setColor(const QColor& color);
    void setEditionMode(Core::EditionMode mode);
    void setNpcNumber(int number);
    void setNpcName(const QString& name);
    void setOpacity(qreal opacity);
    void setPenSize(int penSize);

private:
    void updateProperties();

private:
    std::vector<std::unique_ptr<VectorialMapController>> m_vmaps;
};

#endif // VECTORIALMAPMEDIACONTROLLER_H
