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
#ifndef ROLISTEAMAPPLICATION_H
#define ROLISTEAMAPPLICATION_H

#include <QApplication>

#include "controller/gamecontroller.h"

class QQmlApplicationEngine;
#ifdef QT_DEBUG
class MainControllerSourceReplica;
#include <QRemoteObjectNode>
#endif
class RolisteamApplication : public QApplication
{
    Q_OBJECT
public:
    RolisteamApplication(const QString& appName, const QString& version, int& argn, char* argv[]);

    bool notify(QObject* receiver, QEvent* e);

    GameController* gameCtrl();

    void readSettings();

    void setTranslator(const QStringList& list);

signals:
    void quitApp();
    void connectStatusChanged(bool b);

protected:
    void configureEnginePostLoad(QQmlApplicationEngine* engine);
#ifdef QT_DEBUG
    void keyEventFilter(QEvent* e);
#endif

private:
    QList<QTranslator*> m_translators;
    GameController m_game;
#ifdef QT_DEBUG
    QRemoteObjectNode m_repNode;
    QSharedPointer<MainControllerSourceReplica> m_replica;
#endif
};

#endif // ROLISTEAMAPPLICATION_H
