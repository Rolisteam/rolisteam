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
#ifndef SLIDERCONTROLLER_H
#define SLIDERCONTROLLER_H

#include <QObject>

#include "fieldcontroller.h"

class SliderController : public FieldController
{
    Q_OBJECT
    Q_PROPERTY(qreal from READ from WRITE setfrom NOTIFY fromChanged)
    Q_PROPERTY(qreal to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(qreal step READ step WRITE setStep NOTIFY stepChanged)

public:
    explicit SliderController(bool addCount= true, QObject* parent= nullptr);
    qreal from() const;
    void setfrom(qreal newFrom);
    qreal to() const;
    void setTo(qreal newTo);

    qreal step() const;
    void setStep(qreal newStep);

signals:
    void fromChanged();
    void toChanged();

    void stepChanged();

private:
    qreal m_from;
    qreal m_to;
    qreal m_step;
};

#endif // SLIDERCONTROLLER_H
