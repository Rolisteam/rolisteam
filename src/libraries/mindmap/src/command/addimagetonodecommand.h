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
#ifndef ADDIMAGETONODECOMMAND_H
#define ADDIMAGETONODECOMMAND_H

#include <QPixmap>
#include <QPointer>
#include <QUndoCommand>

class ImageModel;
namespace mindmap
{
class BoxModel;

class AddImageToNodeCommand : public QUndoCommand
{
public:
    AddImageToNodeCommand(BoxModel* nodeModel, ImageModel* imgModel, const QString& id, const QUrl& url);
    AddImageToNodeCommand(BoxModel* nodeModel, ImageModel* imgModel, const QString& id, const QPixmap& pix);

    void undo() override;
    void redo() override;

private:
    QPointer<BoxModel> m_nodeModel;
    QString m_id;
    QPointer<ImageModel> m_imgModel;
    QPixmap m_pixmap;
};
} // namespace mindmap
#endif // ADDIMAGETONODECOMMAND_H
