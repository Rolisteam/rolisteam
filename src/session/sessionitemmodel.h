/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "data/cleveruri.h"
#include "data/chapter.h"


class CleverUriMimeData : public QMimeData
{
Q_OBJECT
public:
    CleverUriMimeData();
    void addCleverURI(CleverURI* m,const QModelIndex);
    QMap<QModelIndex,CleverURI*> getList() const;
    virtual bool hasFormat(const QString & mimeType) const;
private:
    QMap<QModelIndex,CleverURI*> m_mediaList;
};
/**
    * @brief subclassed model to fit the management of session (ressources)
    */
class SessionItemModel : public QAbstractItemModel
{
public:
    enum COLUMN_TYPE {Name,LoadingMode,Displayed,Path};
    /**
    * @brief default construtor
    */
    SessionItemModel();
    /**
    * @brief pure virtual method to create index according the given position and parent
    * @param row : row of the index (define position of the index into the parent's children list)
    * @param column : column of the index (define position of the index into the parent's children list)
    * @param parent : define the parent, if null parent is root item
    * @return the created index
    */
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    /**
    * @brief get the parent of the given index
    * @param child item
    * @return parent
    */
    virtual QModelIndex parent( const QModelIndex & index ) const;
    /**
    * @brief accessor to the rowcount
    * @param index
    * @return the number of children
    */
    virtual int rowCount(const QModelIndex&) const;
    /**
    * @brief accessor to the column count
    * @param ignored
    * @return always return 1
    */
    virtual int columnCount( const QModelIndex&) const;
    /**
    * @brief main method to provide data to the view.
    * @param index to display
    * @param role of data
    * @return appropriate data
    */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**
    * @brief provides data to header section
    */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    /**
    * @brief remove item
    */
    void remove(QModelIndex& index);
    /**
     * @brief addRessources
     * @param uri
     * @param index
     * @return
     */
    void addResource(ResourcesNode* uri,QModelIndex& index);
    /**
    * @brief Allows editing directly in the view
    * @param index position
    * @param new value
    * @param role
    */
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    /**
    * @brief otheridden method to define flag of items, chapter are selectable and enabled
    * resource are also checkable
    * @param position of the item
    */
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;



    virtual void saveModel(QDataStream& out);
    virtual void loadModel(QDataStream& in);

    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
protected:
    bool moveMediaItem(QList<CleverURI*> items, const QModelIndex &parentToBe, int row, QList<QModelIndex> &formerPosition);
private:
    Chapter* m_rootItem;/// root item address
    QStringList m_header;
};

#endif // SESSIONITEMMODEL_H
