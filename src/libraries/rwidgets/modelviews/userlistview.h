/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include "rwidgets_global.h"
#include <QPointer>
#include <QTreeView>
#include <memory>
// class PlayerOnMapModel;
class UserListDelegate;
class Person;
class CharacterAction;
class PlayerController;
/**
 * @brief custom view to display tree person
 */
class RWIDGET_EXPORT UserListView : public QTreeView
{
    Q_OBJECT
public:
    enum Type
    {
        Integer,
        Boolean,
        Real,
        String
    };
    /**
     * @brief default constructor
     */
    explicit UserListView(QWidget* parent= nullptr);
    void setPlayerController(PlayerController* ctrl);
public slots:
    /**
     * @brief called to change the current color
     */
    void editCurrentItemColor();

signals:
    void runDiceForCharacter(const QString& dice, const QString& uuid);

protected slots:
    /**
     * @brief defines new behaviours for mouseDoubleClickEvent
     */
    virtual void mouseDoubleClickEvent(QMouseEvent*) override;
    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    /**
     * @brief contextMenuEvent
     * @param e
     */
    void contextMenuEvent(QContextMenuEvent* e) override;
    /**
     * @brief setPropertyValue
     */
    void setPropertyValue();
    /**
     * @brief setState
     */
    void setState();

    void addAvatar();
    void deleteAvatar();

private:
    QPointer<PlayerController> m_ctrl;
    QAction* m_addAvatarAct= nullptr;
    QAction* m_removeAvatarAct= nullptr;
    // QAction* m_rollInit= nullptr;
    QAction* m_defineCurrentHp= nullptr;
    QAction* m_changeState= nullptr;
    QAction* m_removeInit= nullptr;
    std::vector<QAction*> m_propertyActions;
};

Q_DECLARE_METATYPE(UserListView::Type)

#endif // USERLISTVIEW_H
