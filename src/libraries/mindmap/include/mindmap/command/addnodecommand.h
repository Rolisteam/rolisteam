/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#ifndef ADDNODECOMMAND_H
#define ADDNODECOMMAND_H

#include <QList>
#include <QPointer>
#include <QUndoCommand>

#include <mindmap/mindmap_global.h>

#include "mindmap/data/link.h"
#include "mindmap/data/mindnode.h"

namespace mindmap
{
class BoxModel;
class LinkModel;
class MINDMAP_EXPORT AddNodeCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    AddNodeCommand(const QString& idmap, BoxModel* nodeModel, LinkModel* linkModel, const QString& idParent);

    void setData(const QString& text, const QString& imgUrl);

    void undo() override;
    void redo() override;

signals:
    void addNodes(const QString& mapid, QList<mindmap::MindNode*> nodes, QList<mindmap::Link*> links);
    void removeNodes(const QString& mapid, QStringList nodes, QStringList links);

private:
    QString m_idmap;
    QPointer<mindmap::MindNode> m_mindNode;
    QPointer<mindmap::Link> m_link;
    QPointer<BoxModel> m_nodeModel;
    QPointer<LinkModel> m_linkModel;

    QString m_text;
    QString m_imgUrl;
    QString m_idParent;
};

} // namespace mindmap
#endif // ADDNODECOMMAND_H
