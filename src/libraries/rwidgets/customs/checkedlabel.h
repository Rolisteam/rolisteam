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
#ifndef CHECKEDLABEL_H
#define CHECKEDLABEL_H

#include <QLabel>
#include <QWidget>

class CheckedLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ valid WRITE setValid NOTIFY validChanged)
public:
    CheckedLabel(QWidget* parent= nullptr);

    bool valid() const;

public slots:
    void setValid(bool b);

signals:
    void validChanged();

private:
    bool m_valid= false;
};

#endif // CHECKEDLABEL_H
