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
#include "rcseapplicationcontroller.h"
#include "common/logcategory.h"

RcseApplicationController::RcseApplicationController(QObject* parent) : AbstractApplicationController(parent) {}

void RcseApplicationController::msgToGM(const QString& msg)
{
    qCInfo(RcseCat) << "Sent message:" << msg << "to GM";
}

void RcseApplicationController::msgToAll(const QString& msg)
{
    qCInfo(RcseCat) << "Sent message:" << msg << "to All";
}

void RcseApplicationController::rollDice(const QString& cmd, bool withAlias, bool gmOnly)
{
    auto f= [](bool b, const QString& yes, const QString& no) { return b ? yes : no; };
    qCInfo(RcseCat) << "Roll dice command:" << cmd << " With Alias:" << f(withAlias, tr("Yes"), tr("No")) << " to "
                    << f(gmOnly, tr("only the GM"), tr("All"));
}

qreal RcseApplicationController::zoomLevel() const
{
    return m_zoomLevel;
}

void RcseApplicationController::setZoomLevel(qreal newZoomLevel)
{
    if(qFuzzyCompare(newZoomLevel, m_zoomLevel))
        return;
    m_zoomLevel= newZoomLevel;
    emit zoomLevelChanged();
}
