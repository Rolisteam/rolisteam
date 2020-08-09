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

/*#include "controller/media_controller/charactersheetmediacontroller.h"
#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "controller/media_controller/notemediacontroller.h"
#include "controller/media_controller/sharednotemediacontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/media_controller/webpagemediacontroller.h"*/

#ifdef WITH_PDF
//#include "controller/media_controller/pdfmediacontroller.h"
#endif

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "updater/mediaupdaterinterface.h"
#include "updater/sharednotecontrollerupdater.h"
#include "updater/vmapupdater.h"

#include "gamecontroller.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"
#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"
#include "preferencescontroller.h"
#include "session/sessionitemmodel.h"
#include "undoCmd/newmediacontroller.h"
#include "undoCmd/openmediacontroller.h"
#include "undoCmd/removemediacontrollercommand.h"
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

ContentController::ContentController(PlayerModel* playerModel, CharacterModel* characterModel, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_sessionModel(new SessionItemModel)
    , m_contentModel(new ContentModel)
    , m_sessionName(tr("default"))
{
    CharacterSheetController::setCharacterModel(characterModel);
    SharedNoteController::setPlayerModel(playerModel);

    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory, this);

    std::unique_ptr<VMapUpdater> vmapUpdater(new VMapUpdater);
    std::unique_ptr<SharedNoteControllerUpdater> sharedNoteUpdater(new SharedNoteControllerUpdater);
    m_mediaUpdaters.insert({Core::ContentType::VECTORIALMAP, std::move(vmapUpdater)});
    m_mediaUpdaters.insert({Core::ContentType::SHAREDNOTE, std::move(sharedNoteUpdater)});

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
    emit performCommand(new NewMediaController(type, m_contentModel.get(), params, localIsGM()));
}

void ContentController::openMedia(const std::map<QString, QVariant>& args)
{
    auto it= args.find("type");

    if(it == args.end())
        return;

    auto type= it->second.value<Core::ContentType>();

    emit performCommand(new OpenMediaController(m_contentModel.get(), type, args, localIsGM()));
}

SessionItemModel* ContentController::sessionModel() const
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

void ContentController::addContent(ResourcesNode* node)
{
    m_sessionModel->addResource(node, QModelIndex());
}

void ContentController::removeContent(ResourcesNode* node)
{
    m_sessionModel->removeNode(node);
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

void ContentController::addChapter(const QModelIndex& index) {}

void ContentController::removeSelectedItems(const QModelIndexList& selection) {}
void ContentController::openResources(const QModelIndex& index) {}
void ContentController::saveSession()
{
    // saveAllMediaContainer();
    ModelHelper::saveSession(m_sessionPath, m_sessionName, this);
}

void ContentController::loadSession()
{
    setSessionName(ModelHelper::loadSession(m_sessionPath, this));
}

QString ContentController::gameMasterId() const
{
    return m_gameMasterId;
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
    emit localIdChanged(m_localId);
}

void ContentController::clear()
{
    m_sessionModel->clearData();
    m_contentModel->clearData();
}

void ContentController::saveMedia(const QString& uuid, const QString& path)
{
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
    std::set<NetMsg::Action> actions({NetMsg::AddMedia, NetMsg::UpdateMediaProperty, NetMsg::CloseMedia});
    if(actions.find(msg->action()) == actions.end())
        return result;

    if(msg->action() == NetMsg::CloseMedia)
    {
        Q_UNUSED(static_cast<Core::ContentType>(msg->uint8()));
        auto id= MessageHelper::readMediaId(msg);
        m_sessionModel->removeMedia(id);
        m_contentModel->removeMedia(id);
    }
    else
    {
        auto type= static_cast<Core::ContentType>(msg->uint8());
        auto media= Media::MediaFactory::createRemoteMedia(type, msg, localIsGM());
        m_contentModel->appendMedia(media);
    }

    return result;
}

void ContentController::addImageAs(const QPixmap& map, Core::ContentType type)
{
    /* if(type == Core::ContentType::PICTURE)
     {
         m_imageControllers->addImage(map);
     }
     else if(type == Core::ContentType::VECTORIALMAP)
     {
         m_vmapControllers->addImageToMap(map);
     }*/
}

/*void ContentController::setActiveMediaController(AbstractMediaContainerController* mediaCtrl)
{
    std::find_if(m_mediaControllers.begin(), m_mediaControllers.end(),
                 [this](const std::pair<Core::ContentType, MediaControllerInterface*>& pair) {

                 });
    if()
}*/
