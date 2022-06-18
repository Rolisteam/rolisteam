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

#include "network_global.h"

namespace Core
{
Q_NAMESPACE
enum PermissionMode
{
    GM_ONLY,
    PC_MOVE,
    PC_ALL
};
NETWORK_EXPORT Q_ENUM_NS(PermissionMode)

    /*    TOKEN,
        SONG,
        SONGLIST,*/
    enum class ContentType : int {
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
NETWORK_EXPORT Q_ENUM_NS(ContentType)

    enum class MediaType : int {
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
NETWORK_EXPORT Q_ENUM_NS(MediaType)

    enum class SharingPermission : quint8 {
        None,
        ReadOnly,
        ReadWrite
    };

enum class CampaignAction : quint8
{
    NoneAction,
    ForgetAction,
    CreateAction,
    ManageAction,
    DeleteAction
};
NETWORK_EXPORT Q_ENUM_NS(CampaignAction)

    enum class CampaignDataCategory : quint8 {
        AudioPlayer1,
        AudioPlayer2,
        AudioPlayer3,
        Images,
        Maps,
        MindMaps,
        Notes,
        WebLink,
        PDFDoc,
        DiceAlias,
        CharacterStates,
        Themes,
        CharacterSheets,
        AntagonistList
    };
NETWORK_EXPORT Q_ENUM_NS(CampaignDataCategory)

    NETWORK_EXPORT inline uint qHash(Core::ContentType type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

NETWORK_EXPORT inline uint qHash(Core::MediaType type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

NETWORK_EXPORT inline uint qHash(Core::CampaignDataCategory type, uint seed)
{
    return ::qHash(static_cast<uint>(type), seed);
}

enum class LoadingMode
{
    Internal,
    Linked
};
NETWORK_EXPORT Q_ENUM_NS(LoadingMode)

    enum State {
        Unloaded,
        Hidden,
        Displayed
    };
NETWORK_EXPORT Q_ENUM_NS(State)

    enum VisibilityMode {
        HIDDEN,
        FOGOFWAR,
        ALL,
        NONE
    };
NETWORK_EXPORT Q_ENUM_NS(VisibilityMode)

    enum class GridPattern : quint8 {
        NONE,
        SQUARE,
        HEXAGON,
        OCTOGON
    };
NETWORK_EXPORT Q_ENUM_NS(GridPattern)

    enum ScaleUnit {
        M,
        KM,
        CM,
        MILE,
        YARD,
        INCH,
        FEET,
        PX
    };
NETWORK_EXPORT Q_ENUM_NS(ScaleUnit)

    enum CharacterScope {
        SelectionOnly,
        AllCharacter,
        AllNPC
    };
NETWORK_EXPORT Q_ENUM_NS(CharacterScope)

    enum class EditionMode : int {
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
NETWORK_EXPORT Q_ENUM_NS(SelectableTool)

    enum Properties {
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
NETWORK_EXPORT Q_ENUM_NS(Properties)

    enum class Layer : quint8 {
        GROUND,
        OBJECT,
        CHARACTER_LAYER,
        FOG,
        GRIDLAYER,
        NONE
    };
NETWORK_EXPORT Q_ENUM_NS(Layer)

    namespace keys
{
    constexpr char const* KEY_AVATAR{"avatar"};
    constexpr char const* KEY_BGCOLOR{"bgcolor"};
    constexpr char const* KEY_CHARACTERID{"characterId"};
    constexpr char const* KEY_CHARACTERVISION{"charactervision"};
    constexpr char const* KEY_COLLISION{"collision"};
    constexpr char const* KEY_COLOR{"color"};
    constexpr char const* KEY_DATA{"data"};
    constexpr char const* KEY_GMID{"gamemasterId"};
    constexpr char const* KEY_GRIDABOVE{"GridAbove"};
    constexpr char const* KEY_GRIDCOLOR{"gridColor"};
    constexpr char const* KEY_GRIDPATTERN{"gridPattern"};
    constexpr char const* KEY_GRIDSCALE{"gridScale"};
    constexpr char const* KEY_GRIDSIZE{"gridSize"};
    constexpr char const* KEY_GRIDVISIBILITY{"GridVisibility"};
    constexpr char const* KEY_ID{"id"};
    constexpr char const* KEY_IMAGEDATA{"imageData"};
    constexpr char const* KEY_INTERNAL{"internal"};
    constexpr char const* KEY_ITEMS{"items"};
    constexpr char const* KEY_LAYER{"layer"};
    constexpr char const* KEY_LOCALID{"localId"};
    constexpr char const* KEY_MARKDOWN{"markdown"};
    constexpr char const* KEY_MODE{"mode"};
    constexpr char const* KEY_NAME{"name"};
    constexpr char const* KEY_OWNERID{"ownerId"};
    constexpr char const* KEY_PATH{"path"};
    constexpr char const* KEY_PERMISSION{"permission"};
    constexpr char const* KEY_POSITION{"position"};
    constexpr char const* KEY_QML{"qml"};
    constexpr char const* KEY_RECT{"rect"};
    constexpr char const* KEY_ROOTSECTION{"rootSection"};
    constexpr char const* KEY_SAVEAS{"saveas"};
    constexpr char const* KEY_SCALE{"scale"};
    constexpr char const* KEY_SERIALIZED{"serializedData"};
    constexpr char const* KEY_SIGHT{"sight"};
    constexpr char const* KEY_STATE{"state"};
    constexpr char const* KEY_TEXT{"text"};
    constexpr char const* KEY_TOOL{"tool"};
    constexpr char const* KEY_TYPE{"type"};
    constexpr char const* KEY_UNIT{"unit"};
    constexpr char const* KEY_URL{"URL"};
    constexpr char const* KEY_UUID{"uuid"};
    constexpr char const* KEY_VISIBILITY{"visibility"};
    constexpr char const* KEY_ZINDEX{"zindex"};
} // namespace keys

namespace vmapkeys
{
constexpr char const* KEY_AVATAR{"avatar"};
constexpr char const* KEY_BORDERRECT{"borderRect"};
constexpr char const* KEY_BORDER{"border"};
constexpr char const* KEY_CHARACTER{"character"};
constexpr char const* KEY_CLOSED{"closed"};
constexpr char const* KEY_COLOR{"color"};
constexpr char const* KEY_COUNT{"count"};
constexpr char const* KEY_DATA{"data"};
constexpr char const* KEY_ENDPOINT{"endPoint"};
constexpr char const* KEY_FILLED{"filled"};
constexpr char const* KEY_FONT{"font"};
constexpr char const* KEY_HASAVATAR{"hasAvatar"};
constexpr char const* KEY_IMAGE{"img"};
constexpr char const* KEY_INITIALIZED{"initialized"};
constexpr char const* KEY_LAYER{"layer"};
constexpr char const* KEY_LOCKED{"locked"};
constexpr char const* KEY_MARKDOWN{"markdown"};
constexpr char const* KEY_NUMBER{"number"};
constexpr char const* KEY_OPACITY{"opacity"};
constexpr char const* KEY_ORIGINPOINT{"originPoint"};
constexpr char const* KEY_PATH{"path"};
constexpr char const* KEY_PENLINE{"penLine"};
constexpr char const* KEY_PENWIDTH{"penWidth"};
constexpr char const* KEY_PIXMAP{"pixmap"};
constexpr char const* KEY_PLAYABLECHARACTER{"playablecharacter"};
constexpr char const* KEY_POINTCOUNT{"pointCount"};
constexpr char const* KEY_POINTS{"points"};
constexpr char const* KEY_POS{"position"};
constexpr char const* KEY_RADIUS{"radius"};
constexpr char const* KEY_RATIO{"ratio"};
constexpr char const* KEY_RECT{"rect"};
constexpr char const* KEY_ROTATION{"rotation"};
constexpr char const* KEY_RX{"rx"};
constexpr char const* KEY_RY{"ry"};
constexpr char const* KEY_SELECTED{"selected"};
constexpr char const* KEY_SIDE{"side"};
constexpr char const* KEY_STARTPOINT{"startPoint"};
constexpr char const* KEY_STATECOLOR{"stateColor"};
constexpr char const* KEY_TEXTPOS{"textPos"};
constexpr char const* KEY_TEXTRECT{"textRect"};
constexpr char const* KEY_TEXT{"text"};
constexpr char const* KEY_TOOL{"tool"};
constexpr char const* KEY_ITEMTYPE{"itemtype"};
constexpr char const* KEY_UUID{"uuid"};
constexpr char const* KEY_VISIBLE{"visible"};

constexpr char const* KEY_CHARAC_NPC{"chac_npc"};
constexpr char const* KEY_CHARAC_ID{"chac_uuid"};
constexpr char const* KEY_CHARAC_NAME{"chac_name"};
constexpr char const* KEY_CHARAC_AVATAR{"chac_avatar"};
constexpr char const* KEY_CHARAC_HPCURRENT{"chac_hpcurrent"};
constexpr char const* KEY_CHARAC_HPMAX{"chac_hpmax"};
constexpr char const* KEY_CHARAC_HPMIN{"chac_hpmin"};
constexpr char const* KEY_CHARAC_INITSCORE{"chac_initscore"};
constexpr char const* KEY_CHARAC_DISTANCEPERTURN{"chac_distanceperturn"};
constexpr char const* KEY_CHARAC_STATEID{"chac_stateid"};
constexpr char const* KEY_CHARAC_LIFECOLOR{"chac_lifecolor"};
constexpr char const* KEY_CHARAC_INITCMD{"chac_init_command"};
constexpr char const* KEY_CHARAC_HASINIT{"chac_has_init"};

constexpr char const* KEY_VIS_POS{"vis_position"};
constexpr char const* KEY_VIS_ANGLE{"vis_angle"};
constexpr char const* KEY_VIS_SHAPE{"vis_shape"};
constexpr char const* KEY_VIS_VISIBLE{"vis_visible"};
constexpr char const* KEY_VIS_RADIUS{"vis_radiusv"};

} // namespace vmapkeys

namespace extentions
{
constexpr char const* EXT_TOKEN{".rtok"};
constexpr char const* EXT_MAP{".vmap"};
constexpr char const* EXT_TEXT{".txt"};
constexpr char const* EXT_SHEET{".rcs"};
constexpr char const* EXT_MINDMAP{".rmap"};
constexpr char const* EXT_PLAYLIST{".m3u"};
constexpr char const* EXT_SHAREDNOTE{".rsn"};
constexpr char const* EXT_PDF{".pdf"};
constexpr char const* EXT_WEBVIEW{".rweb"};

constexpr char const* EXT_HTML{".html"};
constexpr char const* EXT_XHTML{".xhtml"};
constexpr char const* EXT_HTM{".htm"};

constexpr char const* EXT_MARKDOWN{".md"};
constexpr char const* EXT_OPEN_DOCUMENT{".odt"};

constexpr char const* EXT_IMG_JPG{".jpg"};
constexpr char const* EXT_IMG_JPEG{".jpeg"};
constexpr char const* EXT_IMG_PNG{".png"};
constexpr char const* EXT_IMG_BMP{".bmp"};
constexpr char const* EXT_IMG_SVG{".svg"};
constexpr char const* EXT_IMG_GIF{".gif"};
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
namespace DiceAlias
{
// Dice Alias
constexpr char const* k_dice_pattern{"pattern"};
constexpr char const* k_dice_command{"command"};
constexpr char const* k_dice_comment{"comment"};
constexpr char const* k_dice_enabled{"enable"};
constexpr char const* k_dice_replacement{"replace"};
} // namespace DiceAlias

namespace jsonctrl
{
namespace base
{
constexpr char const* JSON_CONTENT_TYPE{"contentType"};
constexpr char const* JSON_UUID{"uuid"};
constexpr char const* JSON_PATH{"path"};
constexpr char const* JSON_NAME{"name"};
constexpr char const* JSON_OWNERID{"ownerid"};
} // namespace base
namespace Mindmap
{
constexpr char const* JSON_NODE_ID{"id"};
constexpr char const* JSON_NODE_X{"x"};
constexpr char const* JSON_NODE_Y{"y"};
constexpr char const* JSON_NODE_TEXT{"text"};
constexpr char const* JSON_NODE_IMAGE{"image"};
constexpr char const* JSON_NODE_VISIBLE{"visible"};
constexpr char const* JSON_NODE_OPEN{"open"};
constexpr char const* JSON_NODE_STYLE{"styleindex"};

constexpr char const* JSON_NODES{"nodes"};

constexpr char const* JSON_LINKS{"links"};
constexpr char const* JSON_LINK_IDSTART{"idStart"};
constexpr char const* JSON_LINK_IDEND{"idEnd"};
constexpr char const* JSON_LINK_VISIBLE{"visible"};
constexpr char const* JSON_LINK_DIRECTION{"direction"};
constexpr char const* JSON_LINK_TEXT{"text"};

constexpr char const* JSON_IMGS{"images"};
constexpr char const* JSON_IMG_ID{"id"};
constexpr char const* JSON_IMG_DATA{"data"};
constexpr char const* JSON_IMG_URL{"url"};
} // namespace Mindmap

namespace Audio
{
constexpr char const* JSON_AUDIO_VOLUME{"volume"};
constexpr char const* JSON_AUDIO_VISIBLE{"visible"};
constexpr char const* JSON_PLAYING_MODE{"playingMode"};
constexpr char const* JSON_AUDIO_URLS{"urls"};
} // namespace Audio
} // namespace jsonctrl

namespace mimedata
{
constexpr char const* MIME_KEY_NPC_ID{"rolisteam/non-playable-character-uuid"};
// constexpr char const* MIME_KEY_IMAGE_DATA{"rolisteam/image-data"};
constexpr char const* MIME_KEY_DICE_ALIAS_DATA{"rolisteam/dice-command"};
constexpr char const* MIME_KEY_MEDIA_UUID{"rolisteam/media-uuid"};
constexpr char const* MIME_KEY_PERSON_DATA{"rolisteam/userlist-item"};
} // namespace mimedata

namespace preferences
{
constexpr char const* KEY_DIRECTORY_AP1{"directories_audio_player1"};
constexpr char const* KEY_DIRECTORY_AP2{"directories_audio_player2"};
constexpr char const* KEY_DIRECTORY_AP3{"directories_audio_player3"};
constexpr char const* KEY_OPEN_DIRECTORY{"default_open_directory"};
} // namespace preferences

} // namespace Core

#endif // MEDIATYPE_H
