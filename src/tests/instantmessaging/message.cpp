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
#include "message.h"

Message::Message(QObject* parent) : QObject(parent)
{
    // m_comment= "This is a comment";
    m_command= "2d100";
    m_details= "20 70";
    m_result= "90";
    m_time= "13:30";
    m_owner= "obi";
    m_writerId= "aaa";
}

QString Message::comment() const
{
    return m_comment;
}

QString Message::command() const
{
    return m_command;
}

QString Message::details() const
{
    return m_details;
}

QString Message::result() const
{
    return m_result;
}

QString Message::time() const
{
    return m_time;
}

QString Message::ownerName() const
{
    return m_owner;
}

QString Message::writerId() const
{
    return m_writerId;
}

bool Refresh::refresh() const
{
    return m_refresh;
}

void Refresh::setRefresh(bool refresh)
{
    if(m_refresh == refresh)
        return;
    m_refresh= refresh;
    emit refreshChanged();
}
