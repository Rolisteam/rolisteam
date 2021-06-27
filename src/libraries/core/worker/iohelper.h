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

#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QString>
#include <QVariant>
#include <map>

class VMap;
class VectorialMapController;
class MediaControllerBase;
class ImageController;
class WebpageController;
class NoteController;
class PdfController;
class SharedNoteController;
class CharacterSheetController;
class ContentController;
class MediaManagerBase;
class ContentModel;
class MindMapController;
class DiceAlias;
class CharacterState;
class RolisteamTheme;
class GenericModel;

namespace campaign
{
class NonPlayableCharacter;
}

class IOHelper
{
public:
    IOHelper();

    static QByteArray loadFile(const QString& file);
    static void writeFile(const QString& path, const QByteArray& arry, bool override= true);
    static void moveFile(const QString& source, const QString& destination);
    static void removeFile(const QString& soursce);
    static QString copyFile(const QString& source, const QString& destination);
    static bool makeDir(const QString& dir);
    static QString shortNameFromPath(const QString& path);
    static QString absoluteToRelative(const QString& absolute, const QString& root);

    static QString readTextFile(const QString& file);

    // TOKEN
    /*static QJsonObject characterToToken(const campaign::NonPlayableCharacter* character, const QString& destination,
                                        const QMap<QString, QVariant>& map);
    static bool loadToken(campaign::NonPlayableCharacter* character, const QString& root,
                          std::map<QString, QVariant>& params);*/

    static QByteArray saveController(MediaControllerBase* media);
    static MediaControllerBase* loadController(const QByteArray& data);

    static QJsonObject byteArrayToJsonObj(const QByteArray& data);
    static QJsonObject textByteArrayToJsonObj(const QByteArray& data);
    static QJsonArray byteArrayToJsonArray(const QByteArray& data);
    static QByteArray jsonObjectToByteArray(const QJsonObject& obj);
    static QJsonArray loadJsonFileIntoArray(const QString& filename, bool& ok);
    static QJsonObject loadJsonFileIntoObject(const QString& filename, bool& ok);
    static void writeJsonArrayIntoFile(const QString& destination, const QJsonArray& array);
    static void writeJsonObjectIntoFile(const QString& destination, const QJsonObject& obj);

    static QJsonArray fetchLanguageModel();

    static const QMimeData* clipboardMineData();

    static void readCharacterSheetController(CharacterSheetController* ctrl, const QByteArray& array);
    static QByteArray pixmapToData(const QPixmap& pix);
    static QByteArray imageToData(const QImage& pix);
    static QPixmap dataToPixmap(const QByteArray& data);
    static QImage dataToImage(const QByteArray& data);
    static QPixmap readPixmapFromURL(const QUrl& url);
    static QPixmap readPixmapFromFile(const QString& url);
    static QString htmlToTitle(const QMimeData& data, const QString& defaultName);

    // Controller Generic method
    static void saveBase(MediaControllerBase* base, QDataStream& output);
    static void readBase(MediaControllerBase* base, QDataStream& input);

#ifdef WITH_PDF
    static void readPdfController(PdfController* ctrl, const QByteArray& array);
#endif
    static void readImageController(ImageController* ctrl, const QByteArray& array);
    static void readNoteController(NoteController* ctrl, const QByteArray& array);
    static void readSharedNoteController(SharedNoteController* ctrl, const QByteArray& array);
    static void readWebpageController(WebpageController* ctrl, const QByteArray& array);
    static void readMindmapController(MindMapController* ctrl, const QByteArray& array);

    // dice alias
    static QJsonObject diceAliasToJSonObject(DiceAlias* alias);

    // channel model

    // states
    static QJsonObject stateToJSonObject(CharacterState* state, const QString& root);

    // NPC
    static QJsonObject npcToJsonObject(const campaign::NonPlayableCharacter* npc, const QString& destination);
    static campaign::NonPlayableCharacter* jsonObjectToNpc(const QJsonObject& obj, const QString& rootDir);

    // read theme file
    static RolisteamTheme* jsonToTheme(const QJsonObject& json);
    static QJsonObject themeToObject(const RolisteamTheme* theme);
};

#endif // IOHELPER_H
