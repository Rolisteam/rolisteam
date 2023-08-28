/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "notecontainer.h"

#include "controller/view_controller/notecontroller.h"

NoteContainer::NoteContainer(NoteController* note, QWidget* parent)
    : MediaContainer(note, MediaContainer::ContainerType::NoteContainer, parent)
    , m_noteCtrl(note)
    , m_edit(new TextEdit(note))
{
#ifdef Q_OS_MAC
    m_edit->menuBar()->setNativeMenuBar(false);
#endif
    setWidget(m_edit.get());
    setWindowIcon(QIcon::fromTheme("notes"));
    connect(m_edit.get(), &TextEdit::fileNameChanged, this, [this]() { setWindowModified(false); });

    auto func= [this]() { setWindowTitle(tr("%1 - Note").arg(m_noteCtrl->name())); };
    connect(m_noteCtrl, &NoteController::nameChanged, this, func);
    func();
}
