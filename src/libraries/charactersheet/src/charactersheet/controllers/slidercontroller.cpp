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
#include "charactersheet/controllers/slidercontroller.h"

SliderController::SliderController(bool addCount, QObject* parent)
    : FieldController{TreeSheetItem::SliderItem, addCount, parent}
{
}

qreal SliderController::from() const
{
    return m_from;
}

void SliderController::setfrom(qreal newFrom)
{
    if(qFuzzyCompare(m_from, newFrom))
        return;
    m_from= newFrom;
    emit fromChanged();
}

qreal SliderController::to() const
{
    return m_to;
}

void SliderController::setTo(qreal newTo)
{
    if(qFuzzyCompare(m_to, newTo))
        return;
    m_to= newTo;
    emit toChanged();
}

qreal SliderController::step() const
{
    return m_step;
}

void SliderController::setStep(qreal newStep)
{
    if(qFuzzyCompare(m_step, newStep))
        return;
    m_step= newStep;
    emit stepChanged();
}
