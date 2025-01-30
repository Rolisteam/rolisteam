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
#ifndef RCSEAPPLICATIONCONTROLLER_H
#define RCSEAPPLICATIONCONTROLLER_H

#include <QObject>
#include <charactersheet/abstractapplicationcontroller.h>

class RcseApplicationController : public AbstractApplicationController
{
    Q_OBJECT
public:
    RcseApplicationController(QObject* parent= nullptr);

    void msgToGM(const QString& msg, const QString& characterId) override;
    void msgToAll(const QString& msg, const QString& characterId) override;
    void rollDice(const QString& cmd, const QString& characterId, bool gmOnly= false) override;
    qreal zoomLevel() const override;
    void setZoomLevel(qreal newZoomLevel) override;
    QString characterId() const;
    void setCharacterId(const QString& newCharacterId);

private:
    qreal m_zoomLevel= 1.0;

    // AbstractApplicationController interface
};

#endif // RCSEAPPLICATIONCONTROLLER_H
