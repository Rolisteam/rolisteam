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
#include "controller/contentcontroller.h"

#include <QFileInfo>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "updater/media/charactersheetupdater.h"
#include "updater/media/genericupdater.h"
#include "updater/media/mediaupdaterinterface.h"
#include "updater/media/mindmapupdater.h"
#include "updater/media/sharednotecontrollerupdater.h"
#include "updater/media/vmapupdater.h"
#include "updater/media/webviewupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"

#include "data/campaignmanager.h"

#include "controller/gamecontroller.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"
#include "model/playermodel.h"
#include "model/remoteplayermodel.h"
#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"
#include "undoCmd/newmediacontroller.h"
#include "undoCmd/openmediacontroller.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

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

ContentController::ContentController(campaign::CampaignManager* campaign, PlayerModel* playerModel,
                                     CharacterModel* characterModel, QClipboard* clipboard, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_sessionModel(new QFileSystemModel())
    , m_contentModel(new ContentModel)
    , m_historyModel(new history::HistoryModel)
    , m_clipboard(clipboard)
    , m_campaign(campaign)
{
    CharacterSheetController::setCharacterModel(characterModel);
    SharedNoteController::setPlayerModel(playerModel);
    CharacterFinder::setPlayerModel(playerModel);
    MindMapController::setRemotePlayerModel(new RemotePlayerModel(playerModel));

    connect(m_sessionModel.get(), &QFileSystemModel::rootPathChanged, this, &ContentController::mediaRootChanged);

    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory, this);

    auto fModel= new FilteredContentModel(Core::ContentType::VECTORIALMAP);
    fModel->setSourceModel(m_contentModel.get());

    auto fModel2= new FilteredContentModel(Core::ContentType::SHAREDNOTE);
    fModel2->setSourceModel(m_contentModel.get());

    auto fModel3= new FilteredContentModel(Core::ContentType::MINDMAP);
    fModel3->setSourceModel(m_contentModel.get());

    auto fModel4= new FilteredContentModel(Core::ContentType::CHARACTERSHEET);
    fModel4->setSourceModel(m_contentModel.get());

    std::unique_ptr<VMapUpdater> vmapUpdater(new VMapUpdater(campaign, fModel));
    std::unique_ptr<SharedNoteControllerUpdater> sharedNoteUpdater(new SharedNoteControllerUpdater(fModel2, campaign));
    std::unique_ptr<WebViewUpdater> webviewUpdater(new WebViewUpdater(campaign));
    std::unique_ptr<MindMapUpdater> mindMapUpdater(new MindMapUpdater(fModel3, campaign));
    std::unique_ptr<GenericUpdater> imageUpdater(new GenericUpdater(campaign));
    std::unique_ptr<GenericUpdater> notesUpdater(new GenericUpdater(campaign));
    std::unique_ptr<CharacterSheetUpdater> characterSheetUpdater(new CharacterSheetUpdater(fModel4, campaign));

    connect(characterSheetUpdater.get(), &CharacterSheetUpdater::characterSheetAdded, this,
            [this](NetworkMessageReader* msg)
            {
                auto media= Media::MediaFactory::createRemoteMedia(Core::ContentType::CHARACTERSHEET, msg, localColor(),
                                                                   localIsGM());

                auto existingMedia= dynamic_cast<CharacterSheetController*>(m_contentModel->media(media->uuid()));
                auto sheetCtrl= dynamic_cast<CharacterSheetController*>(media);
                if(!existingMedia)
                {
                    auto sheetUpdater= dynamic_cast<CharacterSheetUpdater*>(
                        m_mediaUpdaters[Core::ContentType::CHARACTERSHEET].get());
                    sheetUpdater->addRemoteCharacterSheet(sheetCtrl);
                    m_contentModel->appendMedia(media);
                }
                else
                {
                    qDebug() << "Merge characterSheet @@@@@@@@@@";
                    existingMedia->merge(sheetCtrl);
                }
            });

    connect(characterSheetUpdater.get(), &CharacterSheetUpdater::characterSheetRemoved, this,
            [this](const QString& uuid, const QString& ctrlId, const QString& characterId)
            {
                Q_UNUSED(characterId)
                qDebug() << "contentController stop SHaring" << uuid;
                auto media= m_contentModel->media(ctrlId);
                auto sheetCtrl= dynamic_cast<CharacterSheetController*>(media);

                if(!sheetCtrl)
                    return;

                auto model= sheetCtrl->model();

                if(!model)
                    return;

                if(model->getCharacterSheetCount() == 1 && nullptr != model->getCharacterSheetById(uuid))
                {
                    qDebug() << "updater: sheet count == 1";
                    m_contentModel->removeMedia(ctrlId);
                }
                else if(model->getCharacterSheetCount() > 1)
                {
                    qDebug() << "updater: sheet count > 1";
                    model->removeCharacterSheet(model->getCharacterSheetById(uuid));
                }
            });

    std::unique_ptr<GenericUpdater> pdfUpdater(new GenericUpdater(campaign));
    MindMapController::setMindMapUpdater(mindMapUpdater.get());

    m_mediaUpdaters.insert({Core::ContentType::VECTORIALMAP, std::move(vmapUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::SHAREDNOTE, std::move(sharedNoteUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::WEBVIEW, std::move(webviewUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::MINDMAP, std::move(mindMapUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::PICTURE, std::move(imageUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::NOTES, std::move(notesUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::CHARACTERSHEET, std::move(characterSheetUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::PDF, std::move(pdfUpdater)});

    connect(m_contentModel.get(), &ContentModel::mediaControllerAdded, this,
            [this](MediaControllerBase* ctrl)
            {
                if(nullptr == ctrl)
                    return;
                ctrl->setPreferences(m_preferences);
                connect(ctrl, &MediaControllerBase::performCommand, this, &ContentController::performCommand);
                connect(ctrl, &MediaControllerBase::popCommand, this, &ContentController::popCommand);
                emit mediaControllerCreated(ctrl);
                sendOffMediaController(ctrl);
                auto it= m_mediaUpdaters.find(ctrl->contentType());
                if(it != m_mediaUpdaters.end())
                    it->second->addMediaController(ctrl);
                qDebug() << "ADD CONTROLLER @@@:" << ctrl->url();
                if(!localIsGM())
                    m_historyModel->addLink(ctrl->url(), ctrl->name(), ctrl->contentType());
            });

    connect(campaign, &campaign::CampaignManager::autoSavedNeeded, this, &ContentController::saveOpenContent);
    connect(m_clipboard, &QClipboard::dataChanged, this, [this]() { emit canPasteChanged(canPaste()); });

    connect(m_historyModel.get(), &history::HistoryModel::modelReset, this, &ContentController::historyChanged);
    connect(m_historyModel.get(), &history::HistoryModel::rowsInserted, this, &ContentController::historyChanged);
    connect(m_historyModel.get(), &history::HistoryModel::rowsRemoved, this, &ContentController::historyChanged);
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

void ContentController::newMedia(campaign::CampaignEditor* editor, const std::map<QString, QVariant>& params)
{
    auto arg= params;
    arg.insert({Core::keys::KEY_OWNERID, m_localId});
    arg.insert({Core::keys::KEY_LOCALID, m_localId});
    arg.insert({Core::keys::KEY_GMID, m_gameMasterId});
    emit performCommand(new NewMediaController(m_contentModel.get(), arg, localColor(), localIsGM(), editor));
}

void ContentController::openMedia(const std::map<QString, QVariant>& args)
{
    auto it= args.find(Core::keys::KEY_TYPE);

    if(it == args.end())
        return;

    auto type= it->second.value<Core::ContentType>();

    emit performCommand(new OpenMediaController(m_contentModel.get(), type, args, localColor(), localIsGM()));
}

void ContentController::renameMedia(const QString& id, const QString& path)
{
    auto media= m_contentModel->media(id);
    if(!media)
        return;
    media->setUrl(QUrl::fromLocalFile(path));
}

QFileSystemModel* ContentController::sessionModel() const
{
    return m_sessionModel.get();
}

ContentModel* ContentController::contentModel() const
{
    return m_contentModel.get();
}

history::HistoryModel* ContentController::historyModel() const
{
    return m_historyModel.get();
}

int ContentController::contentCount() const
{
    return m_contentModel->rowCount();
}

void ContentController::setMediaRoot(const QString& path)
{
    qDebug() << "PathMedia root:" << path << m_sessionModel->rootPath();
    m_sessionModel->setRootPath(path);
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
    m_contentModel->clearData();
}

void ContentController::clearHistory()
{
    m_historyModel->clear();
}

void ContentController::closeCurrentMedia()
{
    m_contentModel->removeMedia(m_contentModel->activeMediaId());
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
        m_contentModel->removeMedia(id);
    }
    else if(msg->action() == NetMsg::AddMedia)
    {
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto media= Media::MediaFactory::createRemoteMedia(type, msg, localColor(), localIsGM());
        m_contentModel->appendMedia(media);
    }
    else if(subActions.contains(msg->action()))
    {
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

void ContentController::saveOpenContent()
{
    campaign::FileSerializer::writeContentModel(m_campaign->placeDirectory(campaign::Campaign::Place::CONTENT_ROOT),
                                                m_contentModel.get());
}

bool ContentController::canPaste() const
{
    bool res= false;

    if(!m_clipboard)
        return res;

    auto data= m_clipboard->mimeData();
    if(!data)
        return res;

    res= data->hasImage();
    return res;
}

bool ContentController::canCopy() const
{
    return false;
}

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

const QColor& ContentController::localColor() const
{
    return m_localColor;
}

void ContentController::setLocalColor(const QColor& newLocalColor)
{
    if(m_localColor == newLocalColor)
        return;
    m_localColor= newLocalColor;
    emit localColorChanged();
}

QStringList ContentController::readData() const
{
    return campaign::FileSerializer::readContentModel(
        m_campaign->placeDirectory(campaign::Campaign::Place::CONTENT_ROOT));
}
