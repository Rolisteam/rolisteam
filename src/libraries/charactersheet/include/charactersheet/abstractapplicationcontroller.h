/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef ABSTRACTAPPLICATIONCONTROLLER_H
#define ABSTRACTAPPLICATIONCONTROLLER_H

#include <QObject>
#include <QQmlEngine>
#include <charactersheet/charactersheet_global.h>

class CHARACTERSHEET_EXPORT AbstractApplicationController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QString characterId READ characterId WRITE setCharacterId NOTIFY characterIdChanged FINAL)
    QML_ELEMENT
    QML_UNCREATABLE("Abstract Class")
public:
    explicit AbstractApplicationController(QObject* p= nullptr) : QObject(p) {}

    virtual void msgToGM(const QString& msg)= 0;
    virtual void msgToAll(const QString& msg)= 0;
    virtual void rollDice(const QString& cmd, bool gmOnly= false)= 0;
    virtual qreal zoomLevel() const= 0;
    virtual void setZoomLevel(qreal newZoomLevel)= 0;
    virtual QString characterId() const= 0;
    virtual void setCharacterId(const QString& newCharacterId)= 0;

signals:
    void zoomLevelChanged();
    void characterIdChanged();
};

#endif // ABSTRACTAPPLICATIONCONTROLLER_H
