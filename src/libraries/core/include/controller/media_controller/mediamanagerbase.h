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
#ifndef MEDIACONTROLLERINTERFACE_H
#define MEDIACONTROLLERINTERFACE_H

#include <QObject>
#include <QPointer>

#include "media/mediatype.h"
#include "network/networkreceiver.h"
#include <core_global.h>
class QUndoStack;
class ContentModel;
class SingleContentTypeModel;
class MediaControllerBase;
class CORE_EXPORT MediaManagerBase : public QObject, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool localIsGM READ localIsGM WRITE setLocalIsGM NOTIFY localIsGMChanged)
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
    Q_PROPERTY(SingleContentTypeModel* model READ model CONSTANT)
public:
    MediaManagerBase(Core::ContentType contentType, ContentModel* contentModel, QObject* parent= nullptr);
    virtual ~MediaManagerBase();
    Core::ContentType type() const;
    virtual void registerNetworkReceiver()= 0;
    virtual int managerCount() const;

    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader*);

    void setUndoStack(QUndoStack* stack);
    bool localIsGM() const;
    QString localId() const;
    SingleContentTypeModel* model() const;

public slots:
    void setLocalIsGM(bool localIsGM);
    void setLocalId(const QString& id);

signals:
    void localIsGMChanged(bool localIsGM);
    void localIdChanged(QString id);
    void mediaClosed(QString id);
    void mediaAdded(QString id, QString path, Core::ContentType type, QString name);

protected:
    std::unique_ptr<SingleContentTypeModel> m_model;
    bool m_localIsGM= false;
    QString m_localId;
    QPointer<QUndoStack> m_undoStack;
};

#endif // MEDIACONTROLLERINTERFACE_H
