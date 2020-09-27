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
#include "autosavecontroller.h"

#include <QTimer>

#include "preferences/preferencesmanager.h"

AutoSaveController::AutoSaveController(PreferencesManager* pref, QObject* parent)
    : QObject(parent), m_preferences(pref), m_timer(new QTimer)
{
    m_preferences->registerLambda("AutoSave", [this](QVariant val) {
        auto autoSave= val.toBool();
        if(autoSave)
            m_timer->start();
        else
            m_timer->stop();
    });

    m_preferences->registerLambda("AutoSaveTime", [this](QVariant val) {
        auto time= val.toInt();
        m_timer->setInterval(time);
    });

    connect(m_timer.get(), &QTimer::timeout, this, &AutoSaveController::saveData);

    auto autoSaveEnable= m_preferences->value("AutoSave", true).toBool();
    auto autoSaveTime= m_preferences->value("AutoSaveTime", 600).toInt();

    m_timer->setInterval(autoSaveTime);

    if(autoSaveEnable)
        m_timer->start();
}
