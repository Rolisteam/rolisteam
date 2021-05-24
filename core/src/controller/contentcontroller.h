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

#include <QClipboard>
#include <QModelIndexList>
#include <QObject>
#include <QPointer>
#include <map>
#include <memory>

#include "controller/controllerinterface.h"

#include "media/mediatype.h"
#include "network/receiveevent.h"
#include "preferences/preferenceslistener.h"

namespace session
{
class SessionItemModel;
}
class MediaContainer;
class QFileSystemModel;
class QAbstractItemModel;
class ResourcesNode;
class PreferencesManager;
class MediaManagerBase;
class NetworkMessageReader;
class ImageMediaController;
class NetworkController;
class MapMediaController;
class CharacterModel;
class PdfMediaController;
class PlayerModel;
class NoteMediaController;
class ContentModel;
class MediaControllerBase;
class MediaUpdaterInterface;
// class AbstractMediaContainerController;
class ContentController : public AbstractControllerInterface, public PreferencesListener, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(QFileSystemModel* sessionModel READ sessionModel CONSTANT)
    Q_PROPERTY(ContentModel* contentModel READ contentModel CONSTANT)
    Q_PROPERTY(QString workspaceFilename READ workspaceFilename NOTIFY workspaceFilenameChanged)
    Q_PROPERTY(QColor workspaceColor READ workspaceColor NOTIFY workspaceColorChanged)
    Q_PROPERTY(int workspacePositioning READ workspacePositioning NOTIFY workspacePositioningChanged)
    Q_PROPERTY(bool shortTitleTab READ shortTitleTab NOTIFY shortTitleTabChanged)
    Q_PROPERTY(int maxLengthTabName READ maxLengthTabName NOTIFY maxLengthTabNameChanged)
    Q_PROPERTY(QString sessionName READ sessionName NOTIFY sessionNameChanged)
    Q_PROPERTY(QString sessionPath READ sessionPath WRITE setSessionPath NOTIFY sessionPathChanged)
    Q_PROPERTY(QString gameMasterId READ gameMasterId WRITE setGameMasterId NOTIFY gameMasterIdChanged)
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
    Q_PROPERTY(QString mediaRoot READ mediaRoot WRITE setMediaRoot NOTIFY mediaRootChanged)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)

public:
    explicit ContentController(PlayerModel* playerModel, CharacterModel* characterModel, QClipboard* clipboard,
                               QObject* parent= nullptr);
    ~ContentController() override;

    QFileSystemModel* sessionModel() const;
    ContentModel* contentModel() const;
    std::vector<MediaManagerBase*> mediaUpdaters() const;
    int contentCount() const;

    int maxLengthTabName() const;
    bool shortTitleTab() const;
    QString workspaceFilename() const;
    QColor workspaceColor() const;
    int workspacePositioning() const;
    QString sessionName() const;
    QString sessionPath() const;
    QString gameMasterId() const;
    QString currentMediaId() const;
    MediaControllerBase* media(const QString& id) const;
    QString localId() const;
    bool localIsGM() const;
    bool canPaste() const;
    bool canCopy() const;
    QString mediaRoot() const;

    void setGameController(GameController*) override;
    void preferencesHasChanged(const QString& key) override;
    void newMedia(Core::ContentType type, const std::map<QString, QVariant>& params= std::map<QString, QVariant>());
    void openMedia(const std::map<QString, QVariant>& params= std::map<QString, QVariant>());
    void clear();
    void closeCurrentMedia();
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

signals:
    void workspaceFilenameChanged();
    void workspaceColorChanged();
    void workspacePositioningChanged();
    void shortTitleTabChanged();
    void maxLengthTabNameChanged();
    void mediaRootChanged();

    void sessionChanged(bool);
    void sessionNameChanged();
    void sessionPathChanged();
    void gameMasterIdChanged(const QString& gameMasterId);
    void localIdChanged(const QString& game);
    void mediaControllerCreated(MediaControllerBase* base);
    void canPasteChanged(bool);

public slots:
    // Media API
    void saveMedia(const QString& uuid, const QString& path);

    // Session API
    void openResources(const QModelIndex& index);
    void addChapter(const QModelIndex& index);
    void removeSelectedItems(const QModelIndexList& selection);
    void setSessionName(const QString& name);
    void setSessionPath(const QString& path);
    void addImageAs(const QPixmap& map, Core::ContentType type);
    void saveSession();
    void loadSession();
    void setGameMasterId(const QString& id);
    void setLocalId(const QString& id);
    void saveSessionBackUp();
    void setMediaRoot(const QString& path);

    void copyData();
    void pasteData();

private:
    void readMimeData(const QMimeData& data);

private:
    std::unique_ptr<QFileSystemModel> m_sessionModel;
    std::map<Core::ContentType, std::unique_ptr<MediaUpdaterInterface>> m_mediaUpdaters;
    std::unique_ptr<ContentModel> m_contentModel;

    QPointer<QClipboard> m_clipboard;

    PreferencesManager* m_preferences= nullptr;
    QString m_sessionName;
    QString m_sessionPath;
    QString m_gameMasterId;
    QString m_localId;
};

#endif // CONTENTCONTROLLER_H
