/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef SESSIONITEMMODEL_H
#define SESSIONITEMMODEL_H

#include <QAbstractItemModel>
#include <QMimeData>
#include <media/mediatype.h>
#include <memory>

namespace session
{
Q_NAMESPACE
enum SessionItemType
{
    Media,
    Person,
    Chapter
};
Q_ENUM_NS(SessionItemType)

class SessionData
{
public:
    SessionData();

    void setUuid(const QString& uuid);
    QString uuid() const;

    void setName(const QString& name);
    QString name() const;

    void setPath(const QString& path);
    QString path() const;

    void setData(const QVariant& data);
    QVariant data() const;

    SessionItemType type() const;
    void setType(SessionItemType type);

private:
    QString m_name;
    QString m_uuid;
    QString m_path;
    QVariant m_data;
    SessionItemType m_type= Chapter;
};

class SessionItem
{
public:
    SessionItem();

    bool isLeaf() const;
    int childrenCount() const;
    SessionData* data() const;
    int indexOf(SessionItem*) const;
    void insertChildAt(int row, SessionItem*);
    SessionItem* childAt(int i) const;
    void setParentNode(SessionItem* parent);
    SessionItem* parentNode() const;
    void removeChild(SessionItem* item);
    bool contains(const QString& id) const;
    QString uuid() const;
    void clear();

    SessionItem* find(const QString& id) const;

    void setName(const QString& name);

private:
    std::vector<std::unique_ptr<SessionItem>> m_children;
    std::unique_ptr<SessionData> m_data;
    SessionItem* m_parent= nullptr;
};

/**
 * @brief Show opened resources and allow to sort and order them
 */
class SessionItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum COLUMN_TYPE
    {
        Name,
        Path
    };

    SessionItemModel();
    virtual ~SessionItemModel();
    virtual QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual int rowCount(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const;

    void remove(QModelIndex& index);
    void addResource(SessionItem* uri, const QModelIndex& index);
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    void clearData();

    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList& indexes) const;

    void addMedia(const QString& id, const QString& path, Core::ContentType type, const QString& name);
    void removeMedia(const QString& id);
public slots:
    void removeNode(SessionItem*);

signals:
    void openResource(SessionItem*, bool);

protected:
    bool moveMediaItem(QList<SessionItem*> items, const QModelIndex& parentToBe, int row,
                       QList<QModelIndex>& formerPosition);

private:
    std::unique_ptr<SessionItem> m_rootItem;
    QStringList m_header;
};
} // namespace session
#endif // SESSIONITEMMODEL_H
