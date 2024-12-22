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
#ifndef IOHELPER_H
#define IOHELPER_H

#include "data/campaign.h"
#include "media/mediatype.h"
#include "model/playermodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QString>
#include <QVariant>
#include <core_global.h>
#include <map>

class MediaControllerBase;
class ImageController;
class WebpageController;
class NoteController;
class PdfController;
class SharedNoteController;
class CharacterSheetController;
class ContentController;
class ContentModel;
class MindMapController;
class DiceAlias;
class CharacterState;
class RolisteamTheme;
class GenericModel;
class AudioPlayerController;

namespace mindmap
{
class MindMapControllerBase;
}

namespace campaign
{
class NonPlayableCharacter;
} // namespace campaign

class CORE_EXPORT IOHelper
{
public:
    IOHelper();

    // Import data from campaign
    static bool mergePlayList(const QString& source, const QString& dest);
    static bool copyArrayModelAndFile(const QString& source, const QString& sourceDir, const QString& dest,
                                      const QString& destDir);
    static QStringList mediaList(const QString& source, Core::MediaType type);

    // json to file
    static QJsonObject byteArrayToJsonObj(const QByteArray& data);
    static QJsonObject textByteArrayToJsonObj(const QByteArray& data);
    static QJsonArray byteArrayToJsonArray(const QByteArray& data);
    static QByteArray jsonObjectToByteArray(const QJsonObject& obj);
    static QByteArray jsonArrayToByteArray(const QJsonArray& obj);
    static QJsonArray loadJsonFileIntoArray(const QString& filename, bool& ok);
    static QJsonObject loadJsonFileIntoObject(const QString& filename, bool& ok);
    static void writeJsonArrayIntoFile(const QString& destination, const QJsonArray& array);
    static bool writeByteArrayIntoFile(const QString& destination, const QByteArray& array);
    static void writeJsonObjectIntoFile(const QString& destination, const QJsonObject& obj);
    static void saveMediaBaseIntoJSon(MediaControllerBase* base, QJsonObject& obj);
    static QJsonArray fetchLanguageModel();

    static const QMimeData* clipboardMineData();

    // image to file
    static QByteArray pixmapToData(const QPixmap& pix);
    static QByteArray imageToData(const QImage& pix);
    static QPixmap dataToPixmap(const QByteArray& data);
    static QImage readImageFromURL(const QUrl& url);
    static QImage readImageFromFile(const QString& url);
    static QString htmlToTitle(const QMimeData& data, const QString& defaultName);

    // Controller Generic method
    static QByteArray saveController(MediaControllerBase* media);
    // static MediaControllerBase* loadController(const QByteArray& data);
    static void saveBase(MediaControllerBase* base, QDataStream& output);
    static void readBase(MediaControllerBase* base, QDataStream& input);
    static void readBaseFromJson(MediaControllerBase* base, const QJsonObject& data);

    static void readPdfController(PdfController* ctrl, const QByteArray& array);
    static void readImageController(ImageController* ctrl, const QByteArray& array);
    static void readNoteController(NoteController* ctrl, const QByteArray& array);
    static void readSharedNoteController(SharedNoteController* ctrl, const QByteArray& array);
    static void readWebpageController(WebpageController* ctrl, const QByteArray& array);
    static void readMindmapControllerBase(mindmap::MindMapControllerBase* ctrl, const QJsonObject& objCtrl);
    static QJsonObject readMindmapController(MindMapController* ctrl, const QByteArray& array);

    // dice alias
    static QJsonObject diceAliasToJSonObject(DiceAlias* alias);

    // states
    static QJsonObject stateToJSonObject(CharacterState* state, const QString& root);

    // NPC
    static QJsonObject npcToJsonObject(const campaign::NonPlayableCharacter* npc, const QString& destination);
    static campaign::NonPlayableCharacter* jsonObjectToNpc(const QJsonObject& obj, const QString& rootDir);

    // read theme file
    static RolisteamTheme* jsonToTheme(const QJsonObject& json);
    static QJsonObject themeToObject(const RolisteamTheme* theme);

    // audio
    static QUrl findSong(const QString& name, QStringList list);
    static QList<QUrl> readM3uPlayList(const QString& filepath);
    static void writePlaylist(const QString& path, const QList<QUrl>& url);
    static QJsonObject saveAudioPlayerController(AudioPlayerController* controller);
    static void fetchAudioPlayerController(AudioPlayerController* controller, const QJsonObject& obj);

    // charactersheet
    static void readCharacterSheetController(CharacterSheetController* ctrl, const QByteArray& array);

    // Campaign
    static QString copyImageFileIntoCampaign(const QString& path, const QString& dest);

    // character
    static Character* dupplicateCharacter(const Character* obj);
};

#endif // IOHELPER_H
