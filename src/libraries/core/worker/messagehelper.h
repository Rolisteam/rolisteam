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
#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QHash>
#include <QPixmap>
#include <QString>
#include <QVariant>

#include "media/mediatype.h"
#include "model/imagemodel.h"
#include "network/networkmessage.h"

namespace vmap
{
class RectController;
class TextController;
class LineController;
class PathController;
class ImageController;
class CharacterItemController;
class EllipseController;
} // namespace vmap

namespace mindmap
{
class Link;
class MindNode;
} // namespace mindmap

class DiceAliasModel;
class DiceAlias;
class CharacterStateModel;
class CharacterState;
class MediaControllerBase;
class ImageController;
class NetworkMessageWriter;
class NetworkMessageReader;
class PlayerModel;
class WebpageController;
class PdfController;
class VectorialMapController;
class Character;
class CharacterSheet;
class CharacterSheetController;
class SharedNoteController;
class MindMapController;
class MessageHelper
{
public:
    MessageHelper();

    static void sendOffGoodBye();
    static void closeMedia(const QString& id, Core::ContentType type);

    static void sendOffAllDiceAlias(DiceAliasModel* model);
    static void sendOffDiceAliasRemoved(int i);
    static void sendOffDiceAliasMoved(int i, int j);
    static void sendOffOneDiceAlias(const DiceAlias* da, int row);

    static void sendOffAllCharacterState(CharacterStateModel* model);
    static void sendOffOneCharacterState(const CharacterState* da, int row);
    static void sendOffCharacterStateRemoved(const QString& id);
    static void sendOffCharacterStateMoved(int i, int j);

    static QString readPlayerId(NetworkMessageReader& msg);
    static void sendOffMediaControllerBase(const MediaControllerBase* ctrl, NetworkMessageWriter& msg);

    // vmap
    static QHash<QString, QVariant> readVectorialMapData(NetworkMessageReader* msg);
    static void sendOffVMap(VectorialMapController* ctrl);

    static const std::map<QString, QVariant> readRect(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readLine(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readEllipse(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readImage(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readText(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readPath(NetworkMessageReader* msg);
    static const std::map<QString, QVariant> readCharacter(NetworkMessageReader* msg);

    static void sendOffRect(const vmap::RectController* ctrl, const QString& mapId);
    static void sendOffLine(const vmap::LineController* ctrl, const QString& mapId);
    static void sendOffEllispe(const vmap::EllipseController* ctrl, const QString& mapId);
    static void sendOffText(const vmap::TextController* ctrl, const QString& mapId);
    static void sendOffPath(const vmap::PathController* ctrl, const QString& mapId);
    static void sendOffImage(const vmap::ImageController* ctrl, const QString& mapId);
    static void sendOffCharacter(const vmap::CharacterItemController* ctrl, const QString& mapId);

    // media
    static QString readMediaId(NetworkMessageReader* msg);
    static QHash<QString, QVariant> readMediaData(NetworkMessageReader* msg);

    // image
    static void sendOffImage(ImageController* ctrl);
    static QHash<QString, QVariant> readImageData(NetworkMessageReader* msg);

// PDF
#ifdef WITH_PDF
    static void sendOffPdfFile(PdfController* ctrl);
    static QHash<QString, QVariant> readPdfData(NetworkMessageReader* msg);
#endif

    // webpage
    static void shareWebpage(WebpageController* ctrl);
    static void updateWebpage(WebpageController* ctrl);
    static QHash<QString, QVariant> readWebPageData(NetworkMessageReader* msg);
    static void readUpdateWebpage(WebpageController* ctrl, NetworkMessageReader* msg);

    // charactersheet
    static void stopSharingSheet(const QString& mediaId, const QString& characterId);
    static void shareCharacterSheet(CharacterSheet* sheet, Character* character, CharacterSheetController* ctrl);
    static QHash<QString, QVariant> readCharacterSheet(NetworkMessageReader* msg);
    static void readUpdateField(CharacterSheetController* ctrl, NetworkMessageReader* msg);

    // sharedNotes
    static void shareNotesTo(const SharedNoteController* ctrl, const QStringList& recipiants);
    static QHash<QString, QVariant> readSharedNoteData(NetworkMessageReader* msg);
    static void closeNoteTo(SharedNoteController* sharedCtrl, const QString& id);

    // Mindmap
    static void sendOffMindmapToAll(MindMapController* ctrl);
    static void sendOffMindmapPermissionUpdate(MindMapController* ctrl);
    static void openMindmapTo(MindMapController* ctrl, const QString& id);
    static void closeMindmapTo(MindMapController* ctrl, const QString& id);
    static void sendOffMindmapPermissionUpdateTo(Core::SharingPermission perm, MindMapController* ctrl,
                                                 const QString& id);
    static QHash<QString, QVariant> readMindMap(NetworkMessageReader* msg);
    static void readMindMapAddItem(MindMapController* ctrl, NetworkMessageReader* msg);
    static void buildAddItemMessage(NetworkMessageWriter& msg, const QList<mindmap::MindNode*>& nodes,
                                    const QList<mindmap::Link*>& links);
    static void buildRemoveItemMessage(NetworkMessageWriter& msg, const QStringList& nodes, const QStringList& links);
    static void readMindMapRemoveMessage(MindMapController* ctrl, NetworkMessageReader* msg);

    // data
    static void updatePerson(NetworkMessageReader& data, PlayerModel* playerModel);
    static QStringList readIdList(NetworkMessageReader& data);
    static void fetchCharacterStatesFromNetwork(NetworkMessageReader* data, CharacterStateModel* model);
    static void fetchDiceAliasFromNetwork(NetworkMessageReader* data, QList<DiceAlias*>* list);

    // ImageModel
    static void sendOffImageInfo(const ImageInfo& info, MediaControllerBase* ctrl);
    static void sendOffRemoveImageInfo(const QString& id, MediaControllerBase* ctrl);
    static void readAddSubImage(ImageModel* model, NetworkMessageReader* msg);
    static void readRemoveSubImage(ImageModel* model, NetworkMessageReader* msg);

    // MusicPlayer
    static void sendOffPlaySong(const QString& songName, qint64 time, int player);
    static void sendOffMusicPlayerOrder(NetMsg::Action netAction, int player);
    static void sendOffTime(qint64 time, int player);
};

#endif // MESSAGEHELPER_H
