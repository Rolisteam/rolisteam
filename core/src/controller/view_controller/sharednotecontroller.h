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
#ifndef SHAREDNOTECONTROLLER_H
#define SHAREDNOTECONTROLLER_H

#include <QAbstractItemModel>
#include <QObject>
#include <memory>

#include "abstractmediacontroller.h"

class PlayerModel;
class SharedNoteController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* playerModel READ playerModel NOTIFY playerModelChanged)
public:
    SharedNoteController(CleverURI* uri, QObject* parent= nullptr);
    ~SharedNoteController() override;

    void saveData() const override;
    void loadData() const override;

    QAbstractItemModel* playerModel() const;

signals:
    void playerModelChanged();

private:
    std::unique_ptr<PlayerModel> m_model;
};

#endif // SHAREDNOTECONTROLLER_H
