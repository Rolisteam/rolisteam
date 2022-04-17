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
#ifndef CHARACTERSHEETUPDATER_H
#define CHARACTERSHEETUPDATER_H

#include <QObject>
#include <core_global.h>
class CharacterSheet;
class NetworkMessageReader;
class  CORE_EXPORT CharacterSheetUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool updating READ updating WRITE setUpdating NOTIFY updatingChanged)

public:
    enum class UpdateMode : quint8
    {
        ALL,
        ONE,
        Many
    };
    Q_ENUM(UpdateMode)

    CharacterSheetUpdater(const QString& id, QObject* parent= nullptr);

    void setMediaId(const QString& id);

    void addCharacterSheetUpdate(CharacterSheet* sheet, CharacterSheetUpdater::UpdateMode mode,
                                 const QStringList& list);

    void readUpdateMessage(NetworkMessageReader* reader, CharacterSheet* sheet);

    bool updating() const;

public slots:
    void setUpdating(bool b);

signals:
    void updatingChanged(bool updating);

private:
    bool m_updating= true;
    QString m_mediaId;
};

#endif // CHARACTERSHEETUPDATER_H
