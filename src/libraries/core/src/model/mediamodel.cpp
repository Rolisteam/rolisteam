/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "model/mediamodel.h"

#include "data/campaign.h"
#include "data/media.h"
#include "undoCmd/renamecampaignmedia.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QString>
#include <QStringList>

namespace campaign
{
namespace
{
QIcon IconFromMedia(Media* media)
{
    if(!media)
        return {};

    static auto hash= QHash<Core::MediaType, QString>({
        {Core::MediaType::Unknown, "unknown"},
        {Core::MediaType::TokenFile, "contact"},
        {Core::MediaType::ImageFile, "image-x-generic"},
        {Core::MediaType::MapFile, "vmap"},
        {Core::MediaType::TextFile, "text-x-generic"},
        {Core::MediaType::MindmapFile, "mindmap"},
        {Core::MediaType::CharacterSheetFile, "treeview"},
        {Core::MediaType::WebpageFile, "text-html"},
        {Core::MediaType::PdfFile, "pdfLogo"},
        {Core::MediaType::AudioFile, "audio-x-generic"},
        {Core::MediaType::PlayListFile, "playlist"},
    });

    return QIcon::fromTheme(hash.value(media->type()));
}

QString getParentPath(const QString& path)
{
    auto canon= QFileInfo(path).dir().canonicalPath();
    auto abso= QFileInfo(path).dir().absolutePath();
    return canon.isEmpty() ? abso : canon;
}

QStringList buildParentList(const QString& path, const QString& root)
{
    QStringList res;
    if(!path.startsWith(root) || path == root)
        return res;

    auto current= path;
    auto pastCurrent= current;
    do
    {
        pastCurrent= current;
        res.prepend(current);
        current= getParentPath(current);
    } while(current != root && pastCurrent != current);

    return res;
}

int findIndexOf(MediaNode* parent, MediaNode* child)
{
    if(!parent || !child)
        return -1;

    auto const& children= parent->children();
    auto it= std::find_if(std::begin(children), std::end(children),
                          [child](const std::unique_ptr<MediaNode>& tmp) { return tmp.get() == child; });

    if(it == std::end(children))
        return -1;

    return std::distance(std::begin(children), it);
}

MediaNode* findNode(const QString& path, MediaNode* root)
{
    auto rootPath= root->path();
    if(path == rootPath)
        return root;

    QStringList parents= buildParentList(path, rootPath);

    auto tmp= root;
    for(const auto& currentPath : parents)
    {
        auto const& children= tmp->children();
        auto it= std::find_if(std::begin(children), std::end(children),
                              [currentPath](const std::unique_ptr<MediaNode>& child)
                              { return child->path() == currentPath; });

        if(it == std::end(children))
            return nullptr;

        tmp= (*it).get();
    }

    if(tmp == root)
        tmp= nullptr;

    return tmp;
}

MediaNode* findNodeFromId(const QString& id, MediaNode* root)
{
    MediaNode* node= nullptr;
    if(root->uuid() == id)
    {
        node= root;
    }
    else
    {
        auto const& children= root->children();
        for(quint32 i= 0; i < children.size() && !node; ++i)
        {
            auto const& child= children.at(i);
            node= findNodeFromId(id, child.get());
        }
    }
    return node;
}
} // namespace

MediaNode::MediaNode(MediaNode::NodeType type, const QString& path, const QString& uuid)
    : m_type(type), m_path(path), m_uuid(uuid)
{
    m_info= QFileInfo(m_path);
}

MediaNode::NodeType MediaNode::nodeType() const
{
    return m_type;
}

void MediaNode::setPath(const QString& path)
{
    if(path == m_path)
        return;
    m_path= path;
    m_info= QFileInfo(m_path);
    emit pathChanged();
}

void MediaNode::addChild(std::unique_ptr<MediaNode> node)
{
    m_children.push_back(std::move(node));
}

QString MediaNode::uuid() const
{
    return m_uuid;
}

int MediaNode::childrenCount() const
{
    return m_children.size();
}

QString MediaNode::parentPath() const
{
    QDir info= m_info.dir();
    return info.absolutePath();
}

void MediaNode::removeChild(int i)
{
    Q_ASSERT(!m_children.empty());
    Q_ASSERT(i < m_children.size());
    m_children.erase(m_children.begin() + i);
}

QDateTime MediaNode::modifiedTime() const
{
    return m_info.metadataChangeTime();
}

QString MediaNode::path() const
{
    return m_path;
}

QString MediaNode::name() const
{
    return m_info.baseName();
}

QVariant MediaNode::size() const
{
    return m_type == Directory ? QVariant() : QVariant::fromValue(QLocale().formattedDataSize(m_info.size()));
}

MediaNode* MediaNode::childAt(int i) const
{
    if(i > static_cast<int>(m_children.size()) || i < 0 || m_children.empty())
        return nullptr;

    return m_children[i].get();
}

const std::vector<std::unique_ptr<MediaNode>>& MediaNode::children() const
{
    return m_children;
}

MediaModel::MediaModel(Campaign* campaign, QObject* parent)
    : QAbstractItemModel(parent)
    , m_campaign(campaign)
    , m_root(
          new MediaNode(campaign::MediaNode::Directory, m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT)))
{
    m_headers= QStringList{tr("Name"), tr("Size"), tr("Type"), tr("Date Added"), tr("Date Modified")};

    connect(m_campaign, &campaign::Campaign::mediaAdded, this, &MediaModel::addMediaNode);
    connect(m_campaign, &campaign::Campaign::mediaRemoved, this, &MediaModel::removeMediaNode);
    connect(m_campaign, &campaign::Campaign::rootDirectoryChanged, this,
            [this]()
            {
                beginResetModel();
                m_root.reset(new MediaNode(campaign::MediaNode::Directory,
                                           m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT)));
                endResetModel();
            });
}

QVariant MediaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return {};

    if(Qt::DisplayRole == role)
    {
        return m_headers[section];
    }

    return {};
}

QModelIndex MediaModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    MediaNode* parentNode= nullptr;

    if(!parent.isValid())
        parentNode= m_root.get();
    else
        parentNode= static_cast<MediaNode*>(parent.internalPointer());

    auto childNode= parentNode->childAt(row);

    if(childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex MediaModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto indexNode= static_cast<MediaNode*>(index.internalPointer());
    auto parentPath= indexNode->parentPath();

    // QFileInfo dir(parentPath);

    if(parentPath.isEmpty() || parentPath == m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT)
       || parentPath.size() < m_campaign->rootDirectory().size())
    {
        return {};
    }

    auto node= findNode(parentPath, m_root.get());
    int i= findIndexOf(node, indexNode);

    if(node == nullptr)
        return {};

    return createIndex(i, 0, node);
}

int MediaModel::rowCount(const QModelIndex& parent) const
{
    MediaNode* mediaNode= nullptr;
    if(!parent.isValid())
        mediaNode= m_root.get();
    else
        mediaNode= static_cast<MediaNode*>(parent.internalPointer());

    return mediaNode->childrenCount();
}

int MediaModel::columnCount(const QModelIndex& parent) const
{
    /*if(!parent.isValid())
        return 0;*/
    Q_UNUSED(parent)

    return m_headers.size();
}

QVariant MediaModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto mediaNode= static_cast<MediaNode*>(index.internalPointer());

    int realrole= Role_Unknown;
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        realrole= index.column() + Role_Name;
    }
    else if(role == Qt::DecorationRole && index.column() == 0)
    {
        realrole= Role_Icon;
    }
    else
    {
        realrole= role;
    }

    auto media= m_campaign->mediaFromUuid(mediaNode->uuid());

    if(!media)
        return {};

    QVariant res;
    switch(realrole)
    {
    case Role_Name:
        res= mediaNode->name();
        break;
    case Role_Size:
        res= mediaNode->size();
        break;
    case Role_Type:
        res= (mediaNode->nodeType() == MediaNode::Directory) ? QVariant::fromValue(Core::MediaType::Unknown) :
                                                               QVariant::fromValue(media->type());
        break;
    case Role_AddedDate:
        res= (mediaNode->nodeType() == MediaNode::Directory) ? QVariant() : media->addedTime();
        break;
    case Role_ModifiedDate:
        res= mediaNode->modifiedTime();
        break;
    case Role_Icon:
        res= (mediaNode->nodeType() == MediaNode::Directory) ? QIcon::fromTheme("folder") : IconFromMedia(media);
        break;
    case Role_Path:
        res= mediaNode->path();
        break;
    case Role_Uuid:
        res= mediaNode->uuid();
        break;
    case Role_IsDir:
        res= (mediaNode->nodeType() == MediaNode::Directory);
        break;
    case Role_Unknown:
        break;
    }

    return res;
}

void MediaModel::addMediaNode(Media* media)
{
    if(!media)
        return;
    auto id= media->id();
    auto path= media->path();
    auto node= findNode(path, m_root.get());

    if(node)
        return;
    MediaNode* parentNode= m_root.get();
    QModelIndex parentIdx;
    QStringList parents= buildParentList(getParentPath(path), m_root->path());
    for(const auto& parent : parents)
    {
        auto node= findNode(parent, m_root.get());
        int i= findIndexOf(parentNode, node);
        if(!node)
        {
            i= parentNode->childrenCount();
            beginInsertRows(parentIdx, i, i);
            std::unique_ptr<MediaNode> media(new MediaNode(MediaNode::Directory, parent));
            node= media.get();
            parentNode->addChild(std::move(media));
            endInsertRows();
        }
        parentIdx= index(i, 0, parentIdx);
        parentNode= node;
    }
    beginInsertRows(parentIdx, parentNode->childrenCount(), parentNode->childrenCount());
    std::unique_ptr<MediaNode> mediaFile(new MediaNode(MediaNode::File, path, id));
    parentNode->addChild(std::move(mediaFile));
    endInsertRows();
}

void MediaModel::removeMediaNode(const QString& id)
{
    auto node= findNodeFromId(id, m_root.get());
    if(!node)
        return;

    auto parentNode= findNode(node->parentPath(), m_root.get());
    int i= findIndexOf(parentNode, node);

    if(i < 0)
    {
        qDebug() << "Error: parent node not found!!";
        return;
    }

    auto idx= createIndex(i, 0, node);
    auto parentIdx= idx.parent();

    beginRemoveRows(parentIdx, idx.row(), idx.row());
    parentNode->removeChild(idx.row());
    endRemoveRows();
}

bool MediaModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool res= false;
    if(data(index, role) != value)
    {
        if(index.column() != 0)
            return res;

        auto mediaNode= static_cast<MediaNode*>(index.internalPointer());
        if(!mediaNode)
            return res;

        auto name= mediaNode->name();
        auto newPath= mediaNode->path().replace(name, value.toString());
        emit performCommand(new RenameCampaignMedia(mediaNode, newPath, mediaNode->path(), this, m_campaign));

        emit dataChanged(index, index, QVector<int>() << role);
        res= true;
    }
    return res;
}

Qt::ItemFlags MediaModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    auto editable= Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    auto nonEditable= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return index.column() == 0 ? editable : nonEditable;
}

void MediaModel::dataChangedFor(MediaNode* node)
{
    if(!node)
        return;

    auto id= node->uuid();
    auto path= node->path();

    MediaNode* parentNode= m_root.get();
    QModelIndex parentIdx;
    QStringList parents= buildParentList(getParentPath(path), m_root->path());
    for(const auto& parent : parents)
    {
        auto node= findNode(parent, m_root.get());
        int i= findIndexOf(parentNode, node);

        parentIdx= index(i, 0, parentIdx);
        parentNode= node;
    }
    auto r= findIndexOf(parentNode, node);
    auto idx= index(r, 0, parentIdx);
    emit dataChanged(idx, idx, QVector<int>());
}

void MediaModel::setCampaign(Campaign* campaign)
{
    if(m_campaign == campaign)
        return;
    m_campaign= campaign;
    emit campaignChanged();
}

void MediaModel::initDataFromCampaign()
{
    beginResetModel();
    m_root.reset(
        new MediaNode(campaign::MediaNode::Directory, m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT)));
    auto const& list= m_campaign->medias();
    std::for_each(std::begin(list), std::end(list),
                  [this](const std::unique_ptr<campaign::Media>& media) { addMediaNode(media.get()); });
    endResetModel();
}

} // namespace campaign
