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
#ifndef MEDIAFILTEREDMODEL_H
#define MEDIAFILTEREDMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "media/mediatype.h"

class MediaFilteredModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern NOTIFY patternChanged)
    Q_PROPERTY(Core::MediaType type READ type WRITE setType NOTIFY typeChanged)
public:
    MediaFilteredModel();

    Core::MediaType type() const;
    QString pattern() const;

public slots:
    void setType(Core::MediaType type);
    void setPattern(const QString& pattern);

signals:
    void typeChanged();
    void patternChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceparent) const override;

private:
    Core::MediaType m_type= Core::MediaType::Unknown;
    QString m_pattern;
};

#endif // MEDIAFILTEREDMODEL_H
