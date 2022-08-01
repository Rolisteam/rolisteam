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
#ifndef REMOVEIMAGEFROMNODECOMMAND_H
#define REMOVEIMAGEFROMNODECOMMAND_H

#include <QUndoCommand>

#include "mindmap/mindmap_global.h"
#include <QPixmap>
#include <QPointer>
namespace mindmap
{
class ImageModel;
class MindItemModel;
class MINDMAP_EXPORT RemoveImageFromNodeCommand : public QUndoCommand
{
public:
    RemoveImageFromNodeCommand(MindItemModel* nodeModel, mindmap::ImageModel* imgModel, const QString& id);
    void undo() override;
    void redo() override;

private:
    QPointer<MindItemModel> m_nodeModel;
    QString m_id;
    QPointer<mindmap::ImageModel> m_imgModel;
    QPixmap m_pixmap;
};
} // namespace mindmap
#endif // REMOVEIMAGEFROMNODECOMMAND_H
