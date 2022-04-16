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

#ifndef SHAREDNOTECONTAINER_H
#define SHAREDNOTECONTAINER_H

#include "mediacontainers/mediacontainer.h"
#include "sharednote.h"

#include <QPointer>
#include <QWidget>

class SharedNoteController;
class SharedNoteContainer : public MediaContainer
{
    Q_OBJECT
public:
    SharedNoteContainer(SharedNoteController* ctrl, QWidget* parent= nullptr);
    virtual bool readFileFromUri();
    virtual void saveMedia(const QString&);
    void readFromFile(QDataStream& data);
    void saveInto(QDataStream& out);
    void setOwnerId(const QString& id);
    void updateNoteToAll();

protected slots:
    virtual void updateTitle();

private:
    QPointer<SharedNoteController> m_sharedCtrl;
    std::unique_ptr<SharedNote> m_edit;
};

#endif // SHAREDNOTECONTAINER_H
