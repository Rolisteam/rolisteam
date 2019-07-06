/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "setbackgroundimage.h"
#include "addpagecommand.h"
#include "controllers/editorcontroller.h"

#include <QDebug>

SetBackgroundCommand::SetBackgroundCommand(int i, EditorController* ctrl, const QUrl& url, QUndoCommand* parent)
    : QUndoCommand(parent), m_image(QPixmap(url.toLocalFile())), m_idx(i), m_ctrl(ctrl), m_filename(url.toLocalFile())
{
    if(m_image.isNull())
    {
        qInfo() << QStringLiteral("Oops! Something is wrong with that file: %1 - No image read").arg(url.toLocalFile());
        return;
    }
    init();
}
SetBackgroundCommand::SetBackgroundCommand(int i, EditorController* ctrl, const QPixmap& pix, const QString& fileName,
                                           QUndoCommand* parent)
    : QUndoCommand(parent), m_image(QPixmap(pix)), m_idx(i), m_ctrl(ctrl), m_filename(fileName)
{
    init();
}

void SetBackgroundCommand::init()
{
    if(m_ctrl->pageCount() <= static_cast<unsigned int>(m_idx))
    {
        m_subCommand= new AddPageCommand(m_ctrl, this);
    }

    if(nullptr == m_subCommand)
        setText(QObject::tr("Set background on Page #%1").arg(m_idx + 1));
    else
        setText(QObject::tr("Add Page #%1 and Set background on it").arg(m_idx + 1));
}

void SetBackgroundCommand::undo()
{
    QUndoCommand::undo();

    m_ctrl->setImageBackground(m_idx, QPixmap(), "");
}

void SetBackgroundCommand::redo()
{
    QUndoCommand::redo();
    m_ctrl->setImageBackground(m_idx, m_image, m_filename);
}
