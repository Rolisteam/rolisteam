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
#include "checkedlabel.h"

#include <QIcon>

constexpr char const* url_to_cross{"invalid"};
constexpr char const* url_to_validcheck{"apply"};
constexpr int iconSize{32};

CheckedLabel::CheckedLabel(QWidget* parent) : QLabel(parent)
{
    setScaledContents(true);
    connect(this, &CheckedLabel::validChanged, this, [this]() {
        setPixmap(m_valid ? QIcon::fromTheme(url_to_validcheck).pixmap(iconSize, iconSize) :
                            QIcon::fromTheme(url_to_cross).pixmap(iconSize, iconSize));
    });

    setPixmap(QIcon::fromTheme(url_to_cross).pixmap(iconSize, iconSize));
    setText(QString());
}

bool CheckedLabel::valid() const
{
    return m_valid;
}

void CheckedLabel::setValid(bool b)
{
    if(b == m_valid)
        return;
    m_valid= b;
    emit validChanged();
}
