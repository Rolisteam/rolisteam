/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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

#include "data/cleveruri.h"
#include "sessionview.h"

#include <QDockWidget>
#include <QPointer>
#include <QSettings>

namespace Ui
{
class SessionDock;
}
class QHBoxLayout;
class ContentController;
class QTreeView;
class SessionItemModel;
class Chapter;
class Session;
/**
 * @brief SessionManager is a dockwidget which displays all loaded resources in the current session. It provides
 * shortcut to open them. It is part of the MVC architecture. It manages the view and the model.
 */
class SessionDock : public QDockWidget
{
    Q_OBJECT
public:
    /**
     * @brief default constructor
     */
    explicit SessionDock(ContentController* ctrl, QWidget* parent= nullptr);

    /**
     * @brief ~SessionManager
     */
    virtual ~SessionDock();
    /**
     * @brief the current chapter is defined by users
     * @return address to the chapter
     */
    // Chapter* getCurrentChapter();
    /**
     * @brief add resource into the model and the session.
     * @param urifile: file's uri
     * @param type : type of content: picture, maps etc...
     */
    // void addRessource(ResourcesNode* uri);
    /**
     * @brief updateCleverUri
     */
    // void updateCleverUri(CleverURI*);
    /**
     * @brief removeResource
     * @param uri
     */
    // void removeResource(CleverURI* uri);
public slots:
    /**
     * @brief resourceClosed
     */
    // void resourceClosed(CleverURI*);

signals:
    /**
     * @brief overriden method to notify the widget's state
     */
    void changeVisibility(bool);
    /**
     * @brief emitted when the application must open a resource
     */
    void openResource(ResourcesNode*, bool);

protected:
    /**
     * @brief catches the closeevent to manage it as hiding event
     */
    void closeEvent(QCloseEvent* event);

private:
    Ui::SessionDock* m_ui;
    QPointer<ContentController> m_ctrl;
};

#endif // SESSIONMANAGER_H
