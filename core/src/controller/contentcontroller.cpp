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

#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/mediacontrollerinterface.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "gamecontroller.h"
#include "preferences/preferencesmanager.h"
#include "preferencescontroller.h"
#include "session/sessionitemmodel.h"

#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "undoCmd/newmediacontroller.h"
#include "undoCmd/openmediacontroller.h"
#include "worker/modelhelper.h"

ContentController::ContentController(NetworkController* networkCtrl, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_contentModel(new SessionItemModel)
    , m_imageControllers(new ImageMediaController)
    , m_vmapControllers(new VectorialMapMediaController(networkCtrl))
    , m_sessionName(tr("Unknown"))
{
    m_mediaControllers.insert({CleverURI::PICTURE, m_imageControllers.get()});
    m_mediaControllers.insert({CleverURI::VMAP, m_vmapControllers.get()});
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

    std::for_each(m_mediaControllers.begin(), m_mediaControllers.end(),
                  [this, game](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {
                      pair.second->setUndoStack(game->undoStack());
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

void ContentController::processMediaMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::addMedia)
    {
        auto type= static_cast<CleverURI::ContentType>(msg->uint8());
        switch(type)
        {
            /* case CleverURI::MAP:
             {
                 MapFrame* mapf= new MapFrame();
                 mapf->readMessage(*msg);
                 prepareMap(mapf);
                 addMediaToMdiArea(mapf, false);
                 mapf->setVisible(true);
             }
             break;
             case CleverURI::VMAP:
             {
                 VMapFrame* mapFrame= new VMapFrame(false);
                 mapFrame->readMessage(*msg); // create the vmap
                 prepareVMap(mapFrame);
                 addMediaToMdiArea(mapFrame, false);
             }
             break;
             case CleverURI::CHAT:
                 break;
             case CleverURI::ONLINEPICTURE:*/
        case CleverURI::PICTURE:
        {
            /*Image* image= new Image(m_mdiArea);
            image->readMessage(*msg);
            addMediaToMdiArea(image, false);
            image->setVisible(true);*/
        }
        break;
            /*#ifdef HAVE_WEBVIEW
                    case CleverURI::WEBVIEW:
                    {
                        auto webv= new WebView(WebView::RemoteView, m_mdiArea);
                        webv->setMediaId(msg->string8());
                        webv->readMessage(*msg);
                        addMediaToMdiArea(webv, false);
                    }
                    break;
            #endif
            #ifdef WITH_PDF
                    case CleverURI::PDF:
                    {
                        auto pdf= new PdfViewer(m_mdiArea);
                        pdf->readMessage(*msg);
                        addMediaToMdiArea(pdf, false);
                        pdf->setVisible(true);
                    }
                    break;
            #endif
                    case CleverURI::CHARACTERSHEET:
                        break;
                    case CleverURI::SHAREDNOTE:
                        break;
                    case CleverURI::TEXT:
                    // case CleverURI::SCENARIO:
                    case CleverURI::SONG:
                    case CleverURI::SONGLIST:
                    case CleverURI::NONE:
                        break;*/
        default:
            break;
        }
    }
    else if(msg->action() == NetMsg::closeMedia)
    {
        // closeMediaContainer(msg->string8(), false);
    }
}

/*void ContentController::setActiveMediaController(AbstractMediaContainerController* mediaCtrl)
{
    std::find_if(m_mediaControllers.begin(), m_mediaControllers.end(),
                 [this](const std::pair<CleverURI::ContentType, MediaControllerInterface*>& pair) {

                 });
    if()
}*/
