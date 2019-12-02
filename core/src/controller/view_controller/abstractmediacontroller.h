/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef ABSTRACTMEDIACONTROLLER_H
#define ABSTRACTMEDIACONTROLLER_H

#include <QObject>

class CleverURI;

class AbstractMediaContainerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(CleverURI* uri READ uri WRITE setUri NOTIFY uriChanged)
public:
    AbstractMediaContainerController(CleverURI* uri, QObject* parent= nullptr);
    QString name() const;
    QString uuid() const;
    CleverURI* uri() const;

    virtual void saveData() const= 0;
    virtual void loadData() const= 0;

signals:
    void nameChanged();
    void uuidChanged();
    void uriChanged();

public slots:
    void setUri(CleverURI* uri);

private:
    QString m_name;
    CleverURI* m_uri;
};

#endif // ABSTRACTMEDIACONTROLLER_H
