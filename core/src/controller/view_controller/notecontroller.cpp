/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "notecontroller.h"

#include "worker/iohelper.h"
//#include <QtConcurrent>

NoteController::NoteController(const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::NOTES, parent)
{
    /*auto future= QtConcurrent::run(IOHelper::loadFile, m_path);
    auto watcher= new QFutureWatcher<QByteArray>();
    connect(watcher, &QFutureWatcher<QByteArray>::finished, this, [this, watcher]() {
        auto result= watcher->result();

        if(!result.isEmpty())
            setText(QString(result));

        watcher->deleteLater();
    });*/
}

void NoteController::saveData() const {}

void NoteController::loadData() const {}

QString NoteController::text() const
{
    return m_text;
}

void NoteController::setText(const QString& text)
{
    if(text == m_text)
        return;
    m_text= text;
    emit textChanged(m_text);
}
