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
#include "data/cleveruri.h"
#include "sessionview.h"

class QHBoxLayout;
class QTreeView;
class SessionItemModel;
class Chapter;
class Session;
class PreferencesManager;
/**
    * @brief SessionManager is a dockwidget which displays all loaded resources in the current session. It provides shortcut to open them. It is part of the MVC architecture. It manages the view and the model.
    */
class SessionManager : public QDockWidget
{
    Q_OBJECT
public:
    /**
    * @brief default constructor
    */
    SessionManager();
    /**
    * @brief the current chapter is defined by users
    * @return address to the chapter
    */
    Chapter* getCurrentChapter();
    /**
    * @brief add resource into the model and the session.
    * @param urifile: file's uri
    * @param type : type of content: picture, maps etc...
    */
    CleverURI* addRessource(CleverURI* uri);
    /**
    * @brief accessor to the recent files list
    */
    const QList<CleverURI*>& getRecentFiles();

    virtual void saveSession(QDataStream& out);
    virtual void loadSession(QDataStream& in);
public slots:
    /**
    * @brief add chapter into the given index
    * @param index position to add item
    */
    void addChapter(QModelIndex&);
    /**
    * @brief open the selected resources, if the given index is a chapter nothing happens
    * @param index of the resources
    */
    void openResources(QModelIndex&);
    /**
    * @brief set the current chapter. The selected item from the view become the current one.
    */
    void setCurrentChapter();
    
    /**
    * @brief removes selected items and all children
    */
    void removeSelectedItem();
    
    
signals:
    /**
    * @brief overriden method to notify the widget's state
    */
    void changeVisibility(bool);
    /**
    * @brief emitted when the application must open a resource
    */
    void openFile(CleverURI*);
protected:
    /**
    * @brief catches the closeevent to manage it as hiding event
    */
    void closeEvent ( QCloseEvent * event );
    
private:
    SessionView* m_view; /// pointer to the view
    SessionItemModel* m_model;/// model to manage the insertion, deleting operation
    /**
    * pointer to the unique instance of preference manager.
    */
    PreferencesManager* m_options;


    QWidget* m_internal;/// generic widget to bring together all the UI
    Chapter* m_currentChapter;/// current chapter pointer
    QHBoxLayout* m_layout;/// layout
};

#endif // SESSIONMANAGER_H
