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
#include <QKeyEvent>
#include <QSettings>
#include <QTranslator>
#include <iostream>

#include "controller/networkcontroller.h"
#include "rep_maincontrollersource_replica.h"

QString eventToText(QKeyEvent* event)
{
    auto mod= event->modifiers();
    auto key= event->text();

    static QHash<int, QString> data{
        {Qt::Key_Escape, "Escape"},
        {Qt::Key_Space, "Space"},
        {Qt::Key_Plus, "Plus"},
        {Qt::Key_Comma, ","},
        {Qt::Key_Minus, "Minus"},
        {Qt::Key_Period, "."},
        {Qt::Key_nobreakspace, "Space"},
        {Qt::Key_Slash, "/"},
        {Qt::ShiftModifier, "Shift"},
        {Qt::ControlModifier, "Ctrl"},
        {Qt::AltModifier, "Alt"},
        {Qt::MetaModifier, "Meta"},
        {Qt::KeypadModifier, "KeyPad"},
        {Qt::Key_Tab, "Tab"},
        {Qt::Key_Backtab, "Backtab"},
        {Qt::Key_Backspace, "Backspace"},
        {Qt::Key_Return, "Return"},
        {Qt::Key_Enter, "Enter"},
        {Qt::Key_Delete, "Delete"},
        {Qt::Key_Insert, "Insert"},
        {Qt::Key_Pause, "Pause"},
        {Qt::Key_Print, "Print"},
        {Qt::Key_Clear, "Clear"},
        {Qt::Key_Home, "Home"},
        {Qt::Key_End, "End"},
        {Qt::Key_Left, "Left"},
        {Qt::Key_Up, "Up"},
        {Qt::Key_Right, "Right"},
        {Qt::Key_Down, "Down"},
        {Qt::Key_PageUp, "PageUp"},
        {Qt::Key_PageDown, "PageDown"},
        {Qt::Key_CapsLock, "CapsLock"},
        {Qt::Key_NumLock, "NumLock"},
        {Qt::Key_ScrollLock, "ScrollLock"},
        {Qt::Key_F1, "F1"},
        {Qt::Key_F2, "F2"},
        {Qt::Key_F3, "F3"},
        {Qt::Key_F4, "F4"},
        {Qt::Key_F5, "F5"},
        {Qt::Key_F6, "F6"},
        {Qt::Key_F7, "F7"},
        {Qt::Key_F8, "F8"},
        {Qt::Key_F9, "F9"},
        {Qt::Key_F10, "F10"},
        {Qt::Key_F11, "F11"},
        {Qt::Key_F12, "F12"},
        {Qt::Key_F13, "F13"},
        {Qt::Key_F14, "F14"},
        {Qt::Key_F15, "F15"},
        {Qt::Key_F16, "F16"},
        {Qt::Key_F17, "F17"},
        {Qt::Key_F18, "F18"},
        {Qt::Key_F19, "F19"},
        {Qt::Key_F20, "F20"},
        {Qt::Key_F21, "F21"},
        {Qt::Key_F22, "F22"},
        {Qt::Key_F23, "F23"},
        {Qt::Key_F24, "F24"},
        {Qt::Key_F25, "F25"},
        {Qt::Key_F26, "F26"},
        {Qt::Key_F27, "F27"},
        {Qt::Key_F28, "F28"},
        {Qt::Key_F29, "F29"},
        {Qt::Key_F30, "F30"},
        {Qt::Key_F31, "F31"},
        {Qt::Key_F32, "F32"},
        {Qt::Key_F33, "F33"},
        {Qt::Key_F34, "F34"},
        {Qt::Key_F35, "F35"},
    };

    QStringList result;
    for(auto m : {Qt::ShiftModifier, Qt::ControlModifier, Qt::AltModifier, Qt::MetaModifier, Qt::KeypadModifier})
        if(mod & m)
            result+= data.value(m);

    if(key.isEmpty() || data.contains(event->key()))
        result+= data.value(event->key());
    else
        result+= key;

    if(result.size() <= 1)
        return {};

    return result.join("+");
}

RolisteamApplication::RolisteamApplication(const QString& appName, const QString& version, int& argn, char* argv[])
    : QApplication(argn, argv), m_game(GameController(appName, version, clipboard()))
{
    setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    connect(&m_game, &GameController::closingApp, this, [this]() { emit quitApp(); });
    connect(m_game.networkController(), &NetworkController::connectedChanged, this,
            [this](bool connected)
            {
                qDebug() << "app connection status changed" << connected;
                emit connectStatusChanged(connected);
            });
#ifdef Q_OS_WIN
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << ":/resources");
    QIcon::setThemeName("rolistheme");
#endif

    qDebug() << QIcon::themeSearchPaths() << QIcon::themeName();

    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");

    setApplicationName(appName);
    setOrganizationName(appName);
    setWindowIcon(QIcon(":/resources/rolisteam/logo/500-symbole.png")); // QIcon::fromTheme("500-symbole"));

    setApplicationVersion(version);
    readSettings();
#ifdef QT_DEBUG
    // create remote object node
    m_repNode.connectToNode(QUrl(QStringLiteral("local:replica"))); // connect with remote host node

    m_replica.reset(m_repNode.acquire<MainControllerSourceReplica>());
#endif
}

bool RolisteamApplication::notify(QObject* receiver, QEvent* e)
{
    try
    {
#ifdef QT_DEBUG
        if(m_replica)
            keyEventFilter(e);
#endif
        return QApplication::notify(receiver, e);
    }
    catch(...)
    {
        std::exception_ptr p= std::current_exception();
        std::clog << (p ? typeid(p).name() : "null") << std::endl;
        qDebug() << e->type() << receiver << " other";
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

    QList<QPair<QString, bool>> params;

    if(hasCustomfile && !system)
    {
        params= {{customFile, false}};
    }
    else if(!system && !paths.isEmpty())
    {
        std::transform(std::begin(paths), std::end(paths), std::back_inserter(params),
                       [](const QString& val) -> QPair<QString, bool> {
                           return {val, false};
                       });
    }
    else
    {
        params= {{"rolisteam", true}};
    }

    setTranslator(params);
}

void RolisteamApplication::setTranslator(const QList<QPair<QString, bool>>& list)
{
    for(auto trans : std::as_const(m_translators))
        removeTranslator(trans);

    qDeleteAll(m_translators);
    m_translators.clear();

    for(const auto& pair : list)
    {
        QTranslator* trans= new QTranslator(this);
        bool success= false;
        if(pair.second)
            success= trans->load(QLocale(), pair.first, "_", ":/translations", ".qm");
        else
            success= trans->load(pair.first); // direct file

        if(!success)
        {
            qDebug() << "error loading translator:" << pair.first;
            delete trans;
            continue;
        }
        installTranslator(trans);
        m_translators.append(trans);
    }
}

void RolisteamApplication::configureEnginePostLoad(QQmlApplicationEngine* engine)
{
    Q_UNUSED(engine)
    // engine->addstuff
}
#ifdef QT_DEBUG
void RolisteamApplication::keyEventFilter(QEvent* e)
{
    if(e->type() != QEvent::KeyPress)
        return;
    auto ke= dynamic_cast<QKeyEvent*>(e);
    if(!ke)
        return;
    auto l= eventToText(ke);
    if(l.isEmpty())
        return;
    m_replica->pushHotKey(l);
}
#endif
