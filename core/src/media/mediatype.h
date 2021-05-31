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
    ONLINEPICTURE,
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
constexpr char const* KEY_TYPE{"type"};
constexpr char const* KEY_NAME{"name"};
constexpr char const* KEY_LOCALID{"localId"};
constexpr char const* KEY_GMID{"gamemasterId"};
} // namespace keys

namespace extentions
{
constexpr char const* EXT_TOKEN{".rtok"};
constexpr char const* EXT_MAP{".vmap"};
constexpr char const* EXT_IMG{".png"};
constexpr char const* EXT_TEXT{".txt"};
constexpr char const* EXT_SHEET{".rcs"};
constexpr char const* EXT_MINDMAP{".rmindmap"};
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
} // namespace JsonKey

} // namespace Core

#endif // MEDIATYPE_H
