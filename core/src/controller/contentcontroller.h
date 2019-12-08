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
#ifndef CONTENTCONTROLLER_H
#define CONTENTCONTROLLER_H

#include <QModelIndexList>
#include <QObject>
#include <map>
#include <memory>

#include "controllerinterface.h"
#include "data/cleveruri.h"
#include "preferences/preferenceslistener.h"

class MediaContainer;
class SessionItemModel;
class QAbstractItemModel;
class ResourcesNode;
class PreferencesManager;
class MediaControllerInterface;
class NetworkMessageReader;
class ImageMediaController;
class VectorialMapMediaController;
// class AbstractMediaContainerController;
class ContentController : public AbstractControllerInterface, public PreferencesListener
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(QString workspaceFilename READ workspaceFilename NOTIFY workspaceFilenameChanged)
    Q_PROPERTY(QColor workspaceColor READ workspaceColor NOTIFY workspaceColorChanged)
    Q_PROPERTY(int workspacePositioning READ workspacePositioning NOTIFY workspacePositioningChanged)
    Q_PROPERTY(bool shortTitleTab READ shortTitleTab NOTIFY shortTitleTabChanged)
    Q_PROPERTY(int maxLengthTabName READ maxLengthTabName NOTIFY maxLengthTabNameChanged)
    Q_PROPERTY(QString sessionName READ sessionName NOTIFY sessionNameChanged)
    Q_PROPERTY(QString sessionPath READ sessionPath WRITE setSessionPath NOTIFY sessionPathChanged)

public:
    explicit ContentController(QObject* parent= nullptr);
    ~ContentController() override;

    QAbstractItemModel* model() const;
    ImageMediaController* imagesCtrl() const;
    VectorialMapMediaController* vmapCtrl() const;

    int maxLengthTabName() const;
    bool shortTitleTab() const;
    QString workspaceFilename() const;
    QColor workspaceColor() const;
    int workspacePositioning() const;
    QString sessionName() const;
    QString sessionPath() const;

    void setGameController(GameController*) override;
    void preferencesHasChanged(const QString& key) override;

    void newMedia(CleverURI::ContentType type,
                  const std::map<QString, QVariant>& params= std::map<QString, QVariant>());
    void openMedia(CleverURI* uri, const std::map<QString, QVariant>& params= std::map<QString, QVariant>());

    void clear();
    void processMediaMessage(NetworkMessageReader* msg);
signals:
    void workspaceFilenameChanged();
    void workspaceColorChanged();
    void workspacePositioningChanged();
    void shortTitleTabChanged();
    void maxLengthTabNameChanged();

    void sessionChanged(bool);
    void sessionNameChanged();
    void sessionPathChanged();

public slots:
    // Media API
    void saveMedia(const QString& uuid, const QString& path);

    // Session API
    void openResources(const QModelIndex& index);
    void addChapter(const QModelIndex& index);
    void removeSelectedItems(const QModelIndexList& selection);
    void addContent(ResourcesNode* node);
    void removeContent(ResourcesNode* node);
    void setSessionName(const QString& name);
    void setSessionPath(const QString& path);
    // void setActiveMediaController(AbstractMediaContainerController* mediaCtrl);
    void saveSession();
    void loadSession();

private:
    std::unique_ptr<SessionItemModel> m_contentModel;
    std::map<CleverURI::ContentType, MediaControllerInterface*> m_mediaControllers;
    std::unique_ptr<ImageMediaController> m_imageControllers;
    std::unique_ptr<VectorialMapMediaController> m_vmapControllers;

    PreferencesManager* m_preferences;
    QString m_sessionName;
    QString m_sessionPath;
};

#endif // CONTENTCONTROLLER_H
