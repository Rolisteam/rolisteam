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
#ifndef IMAGEMEDIACONTROLLER_H
#define IMAGEMEDIACONTROLLER_H

#include <QObject>
#include <memory>
#include <vector>

#include "mediamanagerbase.h"

class ImageController;
class QUndoStack;
class ImageMediaController : public MediaManagerBase
{
    Q_OBJECT
public:
    ImageMediaController();
    ~ImageMediaController() override;

    bool openMedia(const QString& id, const std::map<QString, QVariant>& args) override;
    void closeMedia(const QString& id) override;
    void registerNetworkReceiver() override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;
    void addImage(const QPixmap& image);

signals:
    void imageControllerCreated(ImageController* imageCtrl);

private:
    ImageController* addImageController(const QString& id, const QString& path, const QString& name,
                                        const QByteArray& pix);

private:
    std::vector<std::unique_ptr<ImageController>> m_images;
};

#endif // IMAGEMEDIACONTROLLER_H