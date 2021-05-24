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
#include "contentcontroller.h"

#include <QFileInfo>
#include <QFileSystemModel>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "updater/media/mediaupdaterinterface.h"
#include "updater/media/mindmapupdater.h"
#include "updater/media/sharednotecontrollerupdater.h"
#include "updater/media/vmapupdater.h"
#include "updater/media/webviewupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"

#include "gamecontroller.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"
#include "model/remoteplayermodel.h"
#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"
#include "preferencescontroller.h"
#include "session/sessionitemmodel.h"
#include "undoCmd/newmediacontroller.h"
#include "undoCmd/openmediacontroller.h"
#include "undoCmd/removemediacontrollercommand.h"
#include "userlist/playermodel.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"

void sendOffMediaController(MediaControllerBase* ctrl)
{
    if(!ctrl->localIsOwner() || ctrl->remote())
        return;

    switch(ctrl->contentType())
    {
    case Core::ContentType::PICTURE:
        MessageHelper::sendOffImage(dynamic_cast<ImageController*>(ctrl));
        break;
    case Core::ContentType::VECTORIALMAP:
        MessageHelper::sendOffVMap(dynamic_cast<VectorialMapController*>(ctrl));
        break;
    default:
        break;
    }
}

ContentController::ContentController(PlayerModel* playerModel, CharacterModel* characterModel, QClipboard* clipboard,
                                     QObject* parent)
    : AbstractControllerInterface(parent)
    , m_sessionModel(new QFileSystemModel()) // session::SessionItemModel)
    , m_contentModel(new ContentModel)
    , m_clipboard(clipboard)
    , m_sessionName(tr("default"))
{
    CharacterSheetController::setCharacterModel(characterModel);
    SharedNoteController::setPlayerModel(playerModel);
    MindMapController::setRemotePlayerModel(new RemotePlayerModel(playerModel));

    connect(m_sessionModel.get(), &QFileSystemModel::rootPathChanged, this, &ContentController::mediaRootChanged);

    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory, this);

    auto fModel= new FilteredContentModel(Core::ContentType::VECTORIALMAP);
    fModel->setSourceModel(m_contentModel.get());

    auto fModel2= new FilteredContentModel(Core::ContentType::SHAREDNOTE);
    fModel2->setSourceModel(m_contentModel.get());

    auto fModel3= new FilteredContentModel(Core::ContentType::MINDMAP);
    fModel3->setSourceModel(m_contentModel.get());

    std::unique_ptr<VMapUpdater> vmapUpdater(new VMapUpdater(fModel));
    std::unique_ptr<SharedNoteControllerUpdater> sharedNoteUpdater(new SharedNoteControllerUpdater(fModel2));
    std::unique_ptr<WebViewUpdater> webviewUpdater(new WebViewUpdater());
    std::unique_ptr<MindMapUpdater> mindMapUpdater(new MindMapUpdater(fModel3));
    MindMapController::setMindMapUpdater(mindMapUpdater.get());

    m_mediaUpdaters.insert({Core::ContentType::VECTORIALMAP, std::move(vmapUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::SHAREDNOTE, std::move(sharedNoteUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::WEBVIEW, std::move(webviewUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::MINDMAP, std::move(mindMapUpdater)});

    connect(m_contentModel.get(), &ContentModel::mediaControllerAdded, this, [this](MediaControllerBase* ctrl) {
        if(nullptr == ctrl)
            return;
        connect(ctrl, &MediaControllerBase::performCommand, this, &ContentController::performCommand);
        emit mediaControllerCreated(ctrl);
        sendOffMediaController(ctrl);
        auto it= m_mediaUpdaters.find(ctrl->contentType());
        if(it != m_mediaUpdaters.end())
            it->second->addMediaController(ctrl);
    });

    connect(m_clipboard, &QClipboard::dataChanged, this, [this]() { emit canPasteChanged(canPaste()); });
}

ContentController::~ContentController()= default;

void ContentController::setGameController(GameController* game)
{
    m_preferences= game->preferencesManager();
    m_preferences->registerListener("BackGroundPositioning", this);
    m_preferences->registerListener("PathOfBackgroundImage", this);
    m_preferences->registerListener("BackGroundColor", this);
    m_preferences->registerListener("shortNameInTabMode", this);
    m_preferences->registerListener("MaxLengthTabName", this);
}

void ContentController::preferencesHasChanged(const QString& key)
{
    if(key == QStringLiteral("BackGroundPositioning"))
        emit workspacePositioningChanged();
    else if(key == QStringLiteral("PathOfBackgroundImage"))
        emit workspaceFilenameChanged();
    else if(key == QStringLiteral("BackGroundColor"))
        emit workspaceColorChanged();
    else if(key == QStringLiteral("shortNameInTabMode"))
        emit shortTitleTabChanged();
    else if(key == QStringLiteral("MaxLengthTabName"))
        emit maxLengthTabNameChanged();
}

void ContentController::newMedia(Core::ContentType type, const std::map<QString, QVariant>& params)
{
    auto arg= params;
    arg.insert({"ownerId", m_localId});
    arg.insert({"localId", m_localId});
    arg.insert({"gamemasterId", m_gameMasterId});
    emit performCommand(new NewMediaController(type, m_contentModel.get(), arg, localIsGM()));
}

void ContentController::openMedia(const std::map<QString, QVariant>& args)
{
    auto it= args.find("type");

    if(it == args.end())
        return;

    auto type= it->second.value<Core::ContentType>();

    emit performCommand(new OpenMediaController(m_contentModel.get(), type, args, localIsGM()));
}

QFileSystemModel* ContentController::sessionModel() const
{
    return m_sessionModel.get();
}

ContentModel* ContentController::contentModel() const
{
    return m_contentModel.get();
}

int ContentController::contentCount() const
{
    return m_contentModel->rowCount();
}

void ContentController::setSessionName(const QString& name)
{
    if(name == m_sessionName)
        return;
    m_sessionName= name;
    emit sessionNameChanged();
}

void ContentController::setSessionPath(const QString& path)
{
    if(path == m_sessionPath)
        return;
    m_sessionPath= path;
    emit sessionPathChanged();
}

void ContentController::addChapter(const QModelIndex& index)
{
    Q_UNUSED(index)
}

void ContentController::removeSelectedItems(const QModelIndexList& selection)
{
    Q_UNUSED(selection)
}
void ContentController::openResources(const QModelIndex& index)
{
    Q_UNUSED(index)
}
void ContentController::saveSession()
{
    Q_ASSERT(!m_localId.isEmpty());
    ModelHelper::saveSession(m_sessionPath, m_sessionName, this);
}

void ContentController::saveSessionBackUp()
{
    Q_ASSERT(!m_localId.isEmpty());
    auto path= m_sessionPath;
    QFileInfo info(path);

    auto name= QStringLiteral("%1_back.sce").arg(info.baseName());
    path= QStringLiteral("%1/%2").arg(info.absolutePath()).arg(name);
    ModelHelper::saveSession(path, m_sessionName, this);
}

void ContentController::setMediaRoot(const QString& path)
{
    m_sessionModel->setRootPath(path);
    emit mediaRootChanged();
}

void ContentController::loadSession()
{
    setSessionName(ModelHelper::loadSession(m_sessionPath, this));
}

QString ContentController::gameMasterId() const
{
    return m_gameMasterId;
}

QString ContentController::currentMediaId() const
{
    return m_contentModel->activeMediaId();
}

MediaControllerBase* ContentController::media(const QString& id) const
{
    return m_contentModel->media(id);
}

QString ContentController::localId() const
{
    return m_localId;
}

bool ContentController::localIsGM() const
{
    return m_localId == m_gameMasterId;
}

void ContentController::setGameMasterId(const QString& id)
{
    if(id == m_gameMasterId)
        return;
    m_gameMasterId= id;
    emit gameMasterIdChanged(m_gameMasterId);
}

void ContentController::setLocalId(const QString& id)
{
    if(m_localId == id)
        return;
    m_localId= id;
    Media::MediaFactory::setLocalId(id);
    emit localIdChanged(m_localId);
}

void ContentController::clear()
{
    // m_sessionModel->();
    m_contentModel->clearData();
}

void ContentController::closeCurrentMedia()
{
    m_contentModel->removeMedia(m_contentModel->activeMediaId());
}

void ContentController::saveMedia(const QString& uuid, const QString& path)
{
    Q_UNUSED(uuid)
    Q_UNUSED(path)
    /*auto it= std::find_if(m_loadedResources.begin(), m_loadedResources.end(),
                          [uuid](const ResourcesNode* node) { return node->; });*/
}

int ContentController::maxLengthTabName() const
{
    return m_preferences->value(QStringLiteral("MaxLengthTabName"), 20).toInt();
}

bool ContentController::shortTitleTab() const
{
    return m_preferences->value(QStringLiteral("shortNameInTabMode"), false).toBool();
}
QString ContentController::workspaceFilename() const
{
    return m_preferences->value(QStringLiteral("PathOfBackgroundImage"), 20).toString();
}
QColor ContentController::workspaceColor() const
{
    return m_preferences->value(QStringLiteral("BackGroundColor"), QColor(191, 191, 191)).value<QColor>();
}
int ContentController::workspacePositioning() const
{
    return m_preferences->value(QStringLiteral("BackGroundPositioning"), 0).toInt();
}

QString ContentController::sessionName() const
{
    return m_sessionName;
}

QString ContentController::sessionPath() const
{
    return m_sessionPath;
}

NetWorkReceiver::SendType ContentController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType result= NetWorkReceiver::NONE;
    std::set<NetMsg::Action> actions({NetMsg::AddMedia, NetMsg::UpdateMediaProperty, NetMsg::CloseMedia,
                                      NetMsg::AddSubImage, NetMsg::RemoveSubImage});

    if(actions.find(msg->action()) == actions.end())
        return result;

    QSet<NetMsg::Action> subActions{NetMsg::UpdateMediaProperty, NetMsg::AddSubImage, NetMsg::RemoveSubImage};

    if(msg->action() == NetMsg::CloseMedia)
    {
        Q_UNUSED(static_cast<Core::ContentType>(msg->uint8()));
        auto id= MessageHelper::readMediaId(msg);
        // m_sessionModel->removeMedia(id);
        m_contentModel->removeMedia(id);
    }
    else if(msg->action() == NetMsg::AddMedia)
    {
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto media= Media::MediaFactory::createRemoteMedia(type, msg, localIsGM());
        m_contentModel->appendMedia(media);
    }
    else if(subActions.contains(msg->action()))
    {
        // qDebug() << "ContentController subaction";
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto it= m_mediaUpdaters.find(type);
        if(it != m_mediaUpdaters.end())
        {
            auto updater= it->second.get();
            updater->processMessage(msg);
        }
        else
        {
            QString mediaId= msg->string8();
        }
    }

    return result;
}

void ContentController::addImageAs(const QPixmap& map, Core::ContentType type)
{
    Q_UNUSED(map)
    Q_UNUSED(type)
    /* if(type == Core::ContentType::PICTURE)
     {
         m_imageControllers->addImage(map);
     }
     else if(type == Core::ContentType::VECTORIALMAP)
     {
         m_vmapControllers->addImageToMap(map);
     }*/
}

void ContentController::copyData() {}

void ContentController::pasteData()
{
    auto mediaCtrl= m_contentModel->media(m_contentModel->activeMediaId());
    if(!m_clipboard)
        return;
    const QMimeData* mimeData= m_clipboard->mimeData();
    if(!mimeData)
        return;

    bool accepted= false;
    if(mediaCtrl)
        accepted= mediaCtrl->pasteData(*mimeData);

    if(!accepted)
        readMimeData(*mimeData);
}

void ContentController::readMimeData(const QMimeData& data)
{
    if(data.hasImage())
    {
        auto pix= qvariant_cast<QPixmap>(data.imageData());
        if(pix.isNull())
            return;
        auto imageBytes= IOHelper::pixmapToData(pix);
        QString name(IOHelper::htmlToTitle(data, tr("Copied Image")));

        openMedia({{"type", QVariant::fromValue(Core::ContentType::PICTURE)},
                   {"ownerId", m_localId},
                   {"data", imageBytes},
                   {"name", name}});
    }
    else if(data.hasUrls())
    {
        // qDebug() << "hasURL";
    }
    else if(data.hasHtml())
    {
        // qDebug() << "hasHTML";
        // setText(data.html());
        // setTextFormat(Qt::RichText);
    }
    else if(data.hasText())
    {
        // qDebug() << "hasTEXT";
        // setText(data.text());
        // setTextFormat(Qt::PlainText);
    }
    else
    {
        // qDebug() << "nothing";
        // setText(tr("Cannot display data"));
    }
}

bool ContentController::canPaste() const
{
    bool res= false;

    if(!m_clipboard)
        return res;

    auto data= m_clipboard->mimeData();
    res= data->hasImage();
    return res;
}

bool ContentController::canCopy() const {}

QString ContentController::mediaRoot() const
{
    return m_sessionModel->rootPath();
}
/*void ContentController::setActiveMediaController(AbstractMediaContainerController* mediaCtrl)
{
    std::find_if(m_mediaControllers.begin(), m_mediaControllers.end(),
                 [this](const std::pair<Core::ContentType, MediaControllerInterface*>& pair) {

                 });
    if()
}*/
