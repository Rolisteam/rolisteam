/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDockWidget>
#include <QSettings>
#include "cleveruri.h"
#include "sessionview.h"

class QHBoxLayout;
class QTreeView;
class SessionItemModel;
class Chapter;
class Session;
class PreferencesManager;
class SessionManager : public QDockWidget
{
    Q_OBJECT
public:
    SessionManager();

    Chapter* getCurrentChapter();
    void setCurrentSession(Session* s);

    void readSettings(QSettings & m);

    /**
     * @brief Save parameters for next executions.
     */
    void writeSettings(QSettings & m);

     CleverURI* addRessource(QString& urifile, CleverURI::ContentType type);

     const QList<CleverURI*>& getRecentFiles();
 public slots:
     void addChapter(QModelIndex&);
     void openResources(QModelIndex&);

     void removeSelectedItem();


signals:
     void changeVisibility(bool);
     void openFile(CleverURI*);
protected:
     void closeEvent ( QCloseEvent * event );

private:
    SessionView* m_view;
    QHBoxLayout* m_layout;
    QList<CleverURI*> m_recentlist;
    QWidget* m_internal;
    int m_recentfileCount;

    SessionItemModel* m_model;
    Session* m_currentSession;

    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif // SESSIONMANAGER_H
