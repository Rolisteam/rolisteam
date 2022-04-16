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
#include "rolisteamapplication.h"

#include <QDebug>
#include <QIcon>
#include <QSettings>
#include <QTranslator>
#include <iostream>

#include "controller/networkcontroller.h"

RolisteamApplication::RolisteamApplication(const QString& appName, const QString& version, int& argn, char* argv[])
    : QApplication(argn, argv), m_game(GameController(appName, version, clipboard()))
{
    setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    connect(&m_game, &GameController::closingApp, this, [this]() { setState(ApplicationState::Exit); });
    connect(m_game.networkController(), &NetworkController::connectedChanged, this, [this](bool connected) {
        setState(connected ? ApplicationState::Playing : ApplicationState::SelectProfile);
    });

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");

    setApplicationName(appName);
    setOrganizationName(appName);
    setWindowIcon(QIcon::fromTheme("500-symbole"));

    setApplicationVersion(version);
    readSettings();
}

bool RolisteamApplication::notify(QObject* receiver, QEvent* e)
{
    try
    {
        return QApplication::notify(receiver, e);
    }
    catch(...)
    {
        std::exception_ptr p= std::current_exception();
        std::clog << (p ? typeid(p).name() : "null") << std::endl;
        // qDebug() << e->type() << receiver << " other";
        return false;
    }
}

GameController* RolisteamApplication::gameCtrl()
{
    return &m_game;
}

void RolisteamApplication::readSettings()
{
    QSettings settings(applicationName(), QString("%1_%2/preferences").arg(applicationName(), applicationVersion()));
    settings.beginGroup("rolisteam/preferences");
    int size= settings.beginReadArray("preferenceMap");
    QHash<QString, QVariant> optionDictionary;
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString key= settings.value("key").toString();
        QVariant value= settings.value("value");
        optionDictionary.insert(key, value);
    }
    settings.endArray();
    settings.endGroup();

    auto system= optionDictionary.value("i18n_system").toBool();
    auto paths= optionDictionary.value("i18n_path").toStringList();
    auto hasCustomfile= optionDictionary.value("i18n_hasCustomfile").toBool();
    auto customFile= optionDictionary.value("i18n_customfile").toString();

    if(hasCustomfile && !system)
    {
        setTranslator({customFile});
    }
    else if(!system)
    {
        setTranslator(paths);
    }
    else
    {
        QLocale locale;
        setTranslator({":/translations/rolisteam_" + locale.name(), ":/translations/qt_" + locale.name()});
    }
}

RolisteamApplication::ApplicationState RolisteamApplication::state() const
{
    return m_state;
}

void RolisteamApplication::setTranslator(const QStringList& list)
{
    for(auto trans : qAsConst(m_translators))
        removeTranslator(trans);

    qDeleteAll(m_translators);
    m_translators.clear();

    for(const auto& info : list)
    {
        QTranslator* trans= new QTranslator(this);
        trans->load(info);
        if(trans->isEmpty())
        {
            delete trans;
            continue;
        }
        installTranslator(trans);
        m_translators.append(trans);
    }
}

void RolisteamApplication::setState(ApplicationState state)
{
    if(m_state == state)
        return;
    m_state= state;
    emit stateChanged();
}

void RolisteamApplication::configureEnginePostLoad(QQmlApplicationEngine* engine)
{
    Q_UNUSED(engine)
    // engine->addstuff
}
