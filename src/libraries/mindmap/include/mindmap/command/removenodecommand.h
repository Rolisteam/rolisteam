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
#ifndef REMOVENODECOMMAND_H
#define REMOVENODECOMMAND_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <mindmap/mindmap_global.h>
namespace mindmap
{
class MindNode;
class Link;
class BoxModel;
class LinkModel;
class MINDMAP_EXPORT RemoveNodeCommand : public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    RemoveNodeCommand(const QString& idmap, const std::vector<MindNode*>& selection, BoxModel* nodeModel,
                      LinkModel* linkModel);
    void undo() override;
    void redo() override;

signals:
    void addNodes(const QString& mapid, QList<mindmap::MindNode*> nodes, QList<mindmap::Link*> links);
    void removeNodes(const QString& mapid, QStringList nodes, QStringList links);

private:
    QList<QPointer<MindNode>> m_selection;
    QList<QPointer<Link>> m_links;
    BoxModel* m_nodeModel= nullptr;
    LinkModel* m_linkModel= nullptr;
    QString m_idmap;
};
} // namespace mindmap
#endif // REMOVENODECOMMAND_H
