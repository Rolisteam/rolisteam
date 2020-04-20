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

#include "controller/media_controller/charactersheetmediacontroller.h"
#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/mediacontrollerinterface.h"
#include "controller/media_controller/pdfmediacontroller.h"
#include "controller/media_controller/sharednotemediacontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/media_controller/webpagemediacontroller.h"

#include "controller/view_controller/charactersheetcontroller.h"

#include "gamecontroller.h"
#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"
#include "preferencescontroller.h"
#include "session/sessionitemmodel.h"
#include "undoCmd/newmediacontroller.h"
#include "undoCmd/openmediacontroller.h"
#include "worker/modelhelper.h"

ContentController::ContentController(PlayerModel* playerModel, CharacterModel* characterModel, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_contentModel(new SessionItemModel)
    , m_imageControllers(new ImageMediaController)
    , m_vmapControllers(new VectorialMapMediaController)
    , m_sheetMediaController(new CharacterSheetMediaController(characterModel))
    , m_webPageMediaController(new WebpageMediaController)
    , m_sharedNoteMediaController(new SharedNoteMediaController(playerModel))
    , m_pdfMediaController(new PdfMediaController)
    , m_sessionName(tr("Unknown"))
{
    m_mediaControllers.insert({CleverURI::PICTURE, m_imageControllers.get()});
    m_mediaControllers.insert({CleverURI::VMAP, m_vmapControllers.get()});
    m_mediaControllers.insert({CleverURI::CHARACTERSHEET, m_sheetMediaController.get()});
    m_mediaControllers.insert({CleverURI::WEBVIEW, m_webPageMediaController.get()});
    m_mediaControllers.insert({CleverURI::SHAREDNOTE, m_sharedNoteMediaController.get()});
    m_mediaControllers.insert({CleverURI::PDF, m_pdfMediaController.get()});

    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory, this);

    std::for_each(m_mediaControllers.begin(), m_mediaControllers.end(),
                  [](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {
                      pair.second->registerNetworkReceiver();
                  });

    connect(this, &ContentController::gameMasterIdChanged, m_sheetMediaController.get(),
            &CharacterSheetMediaController::setGameMasterId);
    connect(this, &ContentController::localIdChanged, this, [this](const QString& id) {
        std::for_each(m_mediaControllers.begin(), m_mediaControllers.end(),
                      [id](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {
                          pair.second->setLocalId(id);
                      });
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
    /* m_preferences->registerLambda("Fog_color", [this](QVariant var) {
         if(!var.isValid())
             m_mapMediaController->setFogColor(QColor(0, 0, 0));
         else
             m_mapMediaController->setFogColor(var.value<QColor>());
     });

     m_mapMediaController->setFogColor(m_preferences->value("Fog_color", QColor(Qt::black)).value<QColor>());*/

    std::for_each(m_mediaControllers.begin(), m_mediaControllers.end(),
                  [game](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {
                      pair.second->setUndoStack(game->undoStack());
                      connect(game, &GameController::localIsGMChanged, pair.second,
                              &MediaControllerInterface::setLocalIsGM);
                      pair.second->setLocalIsGM(game->localIsGM());
                  });
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

void ContentController::newMedia(CleverURI::ContentType type, const std::map<QString, QVariant>& params)
{
    auto controller= m_mediaControllers[type];
    if(!controller)
        return;

    emit performCommand(new NewMediaController(type, controller, this, true, params));
}

void ContentController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(!uri)
        return;

    auto controller= m_mediaControllers[uri->getType()];
    if(!controller)
        return;

    emit performCommand(new OpenMediaController(uri, controller, this, true, args));
}
QAbstractItemModel* ContentController::model() const
{
    return m_contentModel.get();
}

ImageMediaController* ContentController::imagesCtrl() const
{
    return m_imageControllers.get();
}

VectorialMapMediaController* ContentController::vmapCtrl() const
{
    return m_vmapControllers.get();
}

CharacterSheetMediaController* ContentController::sheetCtrl() const
{
    return m_sheetMediaController.get();
}

WebpageMediaController* ContentController::webPageCtrl() const
{
    return m_webPageMediaController.get();
}

SharedNoteMediaController* ContentController::sharedCtrl() const
{
    return m_sharedNoteMediaController.get();
}

PdfMediaController* ContentController::pdfCtrl() const
{
    return m_pdfMediaController.get();
}

void ContentController::addContent(ResourcesNode* node)
{
    m_contentModel->addResource(node, QModelIndex());
}

void ContentController::removeContent(ResourcesNode* node)
{
    m_contentModel->removeNode(node);
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
    ModelHelper::saveSession(m_sessionPath, m_sessionName, m_contentModel.get());
}

void ContentController::loadSession()
{
    setSessionName(ModelHelper::loadSession(m_sessionPath, m_contentModel.get()));
}

QString ContentController::gameMasterId() const
{
    return m_gameMasterId;
}

QString ContentController::localId() const
{
    return m_localId;
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
    if(msg->action() == NetMsg::AddMedia || msg->action() == NetMsg::UpdateMediaProperty)
    {
        auto type= static_cast<CleverURI::ContentType>(msg->uint8());
        auto media= m_mediaControllers.at(type);
        media->processMessage(msg);
    }
    return result;
}

void ContentController::addImageAs(const QPixmap& map, CleverURI::ContentType type)
{
    if(type == CleverURI::PICTURE)
    {
        m_imageControllers->addImage(map);
    }
    else if(type == CleverURI::VMAP)
    {
        m_vmapControllers->addImageToMap(map);
    }
}

/*void ContentController::setActiveMediaController(AbstractMediaContainerController* mediaCtrl)
{
    std::find_if(m_mediaControllers.begin(), m_mediaControllers.end(),
                 [this](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {

                 });
    if()
}*/
