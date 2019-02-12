/***************************************************************************
 *	Copyright (C) 2018 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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
#ifndef CLEVERURIMIMEDATA_H
#define CLEVERURIMIMEDATA_H

#include <QMimeData>
#include <QModelIndex>
#include <QObject>

class ResourcesNode;

class CleverUriMimeData : public QMimeData
{
    Q_OBJECT
public:
    CleverUriMimeData();
    void addResourceNode(ResourcesNode* m, const QModelIndex);
    QMap<QModelIndex, ResourcesNode*> getList() const;
    virtual bool hasFormat(const QString& mimeType) const;

private:
    QMap<QModelIndex, ResourcesNode*> m_mediaList;
};

#endif // CLEVERURIMIMEDATA_H
