/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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

#ifndef NOTECONTAINER_H
#define NOTECONTAINER_H

#include "data/mediacontainer.h"
#include "textedit.h"
#include <QWidget>

class NoteContainer : public MediaContainer
{
    Q_OBJECT
public:
    NoteContainer();
    bool readFileFromUri();
    void saveMedia();

    void readFromFile(QDataStream& data);
    void saveInto(QDataStream& out);

    void putDataIntoCleverUri();
public slots:
    void setFileName(QString);

private:
    TextEdit* m_edit;
};

#endif // NOTECONTAINER_H
