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
#ifndef MEDIAMODEL_H
#define MEDIAMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QFileInfo>
#include <QList>
#include <QPointer>
#include <memory>

class QUndoCommand;
namespace campaign
{
class Media;
class MediaNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
public:
    enum NodeType
    {
        File,
        Directory
    };
    MediaNode(NodeType type, const QString& path, const QString& uuid= QString());

    NodeType nodeType() const;
    QString uuid() const;
    int childrenCount() const;
    QString parentPath() const;
    QString path() const;
    QString name() const;
    QVariant size() const;
    QDateTime modifiedTime() const;

    MediaNode* childAt(int i) const;
    void removeChild(int i);

    const std::vector<std::unique_ptr<MediaNode>>& children() const;
    void addChild(std::unique_ptr<MediaNode> node);
signals:
    void pathChanged();

public slots:
    void setPath(const QString& path);

private:
    NodeType m_type;
    QString m_path;
    QFileInfo m_info;
    QString m_uuid;
    std::vector<std::unique_ptr<MediaNode>> m_children;
};

class Campaign;
class MediaModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum DataRole
    {
        Role_Name= Qt::UserRole + 1,
        Role_Size,
        Role_Type,
        Role_AddedDate,
        Role_ModifiedDate,
        Role_Icon,
        Role_Path,
        Role_Uuid,
        Role_IsDir,
        Role_Unknown
    };
    explicit MediaModel(Campaign* campaign, QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void dataChangedFor(MediaNode* node);

    void setCampaign(Campaign* campaign);
signals:
    void campaignChanged();
    void performCommand(QUndoCommand* cmd);

private slots:
    void addMediaNode(Media* media);
    void removeMediaNode(const QString& id);

    void initDataFromCampaign();

private:
    QPointer<Campaign> m_campaign;
    std::unique_ptr<MediaNode> m_root;
    QStringList m_headers;
};

} // namespace campaign
#endif // MEDIAMODEL_H
