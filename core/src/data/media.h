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
#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>

#include "media/mediatype.h"

namespace campaign
{
class Media : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(Core::MediaType type READ type CONSTANT)
    Q_PROPERTY(QString id READ id CONSTANT)
public:
    explicit Media(const QString& id, const QString& name, const QString& path, Core::MediaType type,
                   QObject* parent= nullptr);

    QString name() const;
    QString path() const;
    Core::MediaType type() const;
    QString id() const;

    void setPath(const QString& path);
    void setName(const QString& name);

signals:
    void nameChanged();
    void pathChanged();

private:
    QString m_id;
    Core::MediaType m_type;
    QString m_name;
    QString m_path;
};
} // namespace campaign
#endif // MEDIA_H
