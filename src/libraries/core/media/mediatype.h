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
#ifndef MEDIATYPE_H
#define MEDIATYPE_H

#include <QHash>
#include <QObject>

namespace Core
{
Q_NAMESPACE
enum PermissionMode
{
    GM_ONLY,
    PC_MOVE,
    PC_ALL
};
Q_ENUM_NS(PermissionMode)

/*    TOKEN,
    SONG,
    SONGLIST,*/
enum class ContentType : int
{
    VECTORIALMAP,
    PICTURE,
    NOTES,
    CHARACTERSHEET,
    SHAREDNOTE,
    PDF,
    WEBVIEW,
    INSTANTMESSAGING,
    MINDMAP,
    UNKNOWN
};
Q_ENUM_NS(ContentType)

enum class MediaType : int
{
    Unknown,
    TokenFile,
    ImageFile,
    MapFile,
    TextFile,
    CharacterSheetFile,
    PdfFile,
    WebpageFile,
    MindmapFile,
    AudioFile,
    PlayListFile
};
Q_ENUM_NS(MediaType)

enum class SharingPermission : quint8
{
    None,
    ReadOnly,
    ReadWrite
};

inline uint qHash(Core::ContentType type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

inline uint qHash(Core::MediaType type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

enum class LoadingMode
{
    Internal,
    Linked
};
Q_ENUM_NS(LoadingMode)

enum State
{
    Unloaded,
    Hidden,
    Displayed
};
Q_ENUM_NS(State)

enum VisibilityMode
{
    HIDDEN,
    FOGOFWAR,
    ALL
};
Q_ENUM_NS(VisibilityMode)

enum class GridPattern : quint8
{
    NONE,
    SQUARE,
    HEXAGON,
    OCTOGON
};
Q_ENUM_NS(GridPattern)

enum ScaleUnit
{
    M,
    KM,
    CM,
    MILE,
    YARD,
    INCH,
    FEET,
    PX
};
Q_ENUM_NS(ScaleUnit)

enum CharacterScope
{
    SelectionOnly,
    AllCharacter,
    AllNPC
};

enum class EditionMode : int
{
    Painting,
    Mask,
    Unmask
};

enum SelectableTool
{
    PEN,
    LINE,
    EMPTYRECT,
    FILLRECT,
    EMPTYELLIPSE,
    FILLEDELLIPSE,
    TEXT,
    HANDLER,
    NonPlayableCharacter,
    PlayableCharacter,
    IMAGE,
    RULE,
    PATH,
    ANCHOR,
    TEXTBORDER,
    PIPETTE,
    BUCKET,
    HIGHLIGHTER
};
Q_ENUM_NS(SelectableTool)

enum Properties
{
    ShowNpcName,
    ShowPcName,
    ShowNpcNumber,
    ShowHealthStatus,
    ShowInitScore,
    ShowGrid,
    LocalIsGM,
    GridPatternProperty,
    GridColor,
    GridSize,
    Scale,
    Unit,
    EnableCharacterVision,
    PermissionModeProperty,
    FogOfWarStatus,
    CollisionStatus,
    GridAbove,
    HideOtherLayers,
    VisibilityModeProperty,
    ShowHealthBar,
    MapLayer
};
Q_ENUM_NS(Properties)

enum class Layer : quint8
{
    GROUND,
    OBJECT,
    CHARACTER_LAYER,
    FOG,
    GRIDLAYER,
    NONE
};
Q_ENUM_NS(Layer)

enum class ArrowDirection : quint8
{
    StartToEnd,
    EndToStart,
    Both
};
Q_ENUM_NS(ArrowDirection)

namespace keys
{
constexpr char const* KEY_PATH{"path"};
constexpr char const* KEY_SAVEAS{"saveas"};
constexpr char const* KEY_URL{"URL"};
constexpr char const* KEY_UUID{"uuid"};
constexpr char const* KEY_POSITION{"position"};
constexpr char const* KEY_TOOL{"tool"};
constexpr char const* KEY_RECT{"rect"};
constexpr char const* KEY_DATA{"data"};
constexpr char const* KEY_OWNERID{"ownerId"};
constexpr char const* KEY_INTERNAL{"internal"};
constexpr char const* KEY_TYPE{"type"};
constexpr char const* KEY_NAME{"name"};
constexpr char const* KEY_LOCALID{"localId"};
constexpr char const* KEY_GMID{"gamemasterId"};
constexpr char const* KEY_SERIALIZED{"serializedData"};
} // namespace keys

namespace extentions
{
constexpr char const* EXT_TOKEN{".rtok"};
constexpr char const* EXT_MAP{".vmap"};
constexpr char const* EXT_IMG{".png"};
constexpr char const* EXT_TEXT{".txt"};
constexpr char const* EXT_SHEET{".rcs"};
constexpr char const* EXT_MINDMAP{".rmap"};
constexpr char const* EXT_PLAYLIST{".m3u"};
constexpr char const* EXT_SHAREDNOTE{".md"};
constexpr char const* EXT_PDF{".pdf"};
constexpr char const* EXT_WEBVIEW{".html"};
} // namespace extentions

namespace JsonKey
{
constexpr char const* JSON_NAME{"name"};
constexpr char const* JSON_MEDIAS{"medias"};
constexpr char const* JSON_CURRENT_CHAPTER{"chapter"};
constexpr char const* JSON_MEDIA_PATH{"path"};
constexpr char const* JSON_MEDIA_CREATIONTIME{"ctime"};
constexpr char const* JSON_MEDIA_ID{"uuid"};

constexpr char const* JSON_NPC_ID{"uuid"};
constexpr char const* JSON_NPC_NAME{"name"};
constexpr char const* JSON_NPC_DESCRIPTION{"description"};
constexpr char const* JSON_NPC_INITCOMMAND{"initCmd"};
constexpr char const* JSON_NPC_INITVALUE{"initVal"};
constexpr char const* JSON_NPC_COLOR{"color"};
constexpr char const* JSON_NPC_HP{"hp"};
constexpr char const* JSON_NPC_MAXHP{"maxhp"};
constexpr char const* JSON_NPC_MINHP{"minhp"};
constexpr char const* JSON_NPC_DIST_PER_TURN{"distperturn"};
constexpr char const* JSON_NPC_STATEID{"stateid"};
constexpr char const* JSON_NPC_LIFECOLOR{"lifecolor"};
constexpr char const* JSON_NPC_AVATAR{"avataruri"};
constexpr char const* JSON_NPC_TAGS{"tags"};
constexpr char const* JSON_NPC_TOKEN{"tokenpath"};

// action
constexpr char const* JSON_NPC_ACTIONS{"actions"};
constexpr char const* JSON_NPC_ACTION_COMMAND{"command"};
constexpr char const* JSON_NPC_ACTION_NAME{"name"};

// property
constexpr char const* JSON_NPC_PROPERTIES{"properties"};
constexpr char const* JSON_NPC_PROPERTY_NAME{"name"};
constexpr char const* JSON_NPC_PROPERTY_VALUE{"value"};

// shape
constexpr char const* JSON_NPC_SHAPES{"shapes"};
constexpr char const* JSON_NPC_SHAPE_NAME{"name"};
constexpr char const* JSON_NPC_SHAPE_DATAIMG{"dataImg"};

constexpr char const* JSON_TOKEN_SIZE{"size"};
} // namespace JsonKey

namespace jsonNetwork
{
constexpr char const* JSON_TYPE{"type"};
constexpr char const* JSON_TYPE_CHANNEL{"channel"};
constexpr char const* JSON_TYPE_CLIENT{"client"};

constexpr char const* JSON_NAME{"name"};
constexpr char const* JSON_GM{"type"};
constexpr char const* JSON_ADMIN{"admin"};
constexpr char const* JSON_ID{"id"};
constexpr char const* JSON_IDPLAYER{"idPlayer"};

constexpr char const* JSON_PASSWORD{"password"};
constexpr char const* JSON_DESCRIPTION{"description"};
constexpr char const* JSON_USERLISTED{"usersListed"};
constexpr char const* JSON_MEMORYSIZE{"memorySize"};
constexpr char const* JSON_LOCKED{"locked"};
constexpr char const* JSON_CHANNELS{"channels"};
constexpr char const* JSON_CHILDREN{"children"};
} // namespace jsonNetwork

namespace jsonDice
{
constexpr char const* JSON_COMMAND{"command"};
constexpr char const* JSON_ERROR{"error"};
constexpr char const* JSON_SCALAR{"scalar"};
constexpr char const* JSON_INSTRUCTION{"instructions"};
constexpr char const* JSON_DICE_FACE{"face"};
constexpr char const* JSON_DICE_VALUE{"value"};
} // namespace jsonDice

namespace mimedata
{
constexpr char const* MIME_KEY_NPC_ID{"rolisteam/non-playable-character-uuid"};
// constexpr char const* MIME_KEY_IMAGE_DATA{"rolisteam/image-data"};
constexpr char const* MIME_KEY_DICE_ALIAS_DATA{"rolisteam/dice-command"};
constexpr char const* MIME_KEY_MEDIA_UUID{"rolisteam/media-uuid"};
constexpr char const* MIME_KEY_PERSON_DATA{"rolisteam/userlist-item"};
} // namespace mimedata
} // namespace Core

#endif // MEDIATYPE_H
