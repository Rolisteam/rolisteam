/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

#include <QDrag>

#include "playersListWidget.h"
#include "playerslistproxy.h"
#include "userlistdelegate.h"
#include "userlistview.h"

#include "data/character.h"
#include "playersList.h"
#include "preferences/preferencesmanager.h"
#include "rolisteammimedata.h"
#include <QInputDialog>

UserListView::UserListView(QWidget* parent) : QTreeView(parent), m_diceParser(new DiceParser)
{
    setHeaderHidden(true);

    m_addAvatarAct= new QAction(tr("Set Avatar..."), this);
    m_removeAvatarAct= new QAction(tr("Remove Avatar..."), this);
    connect(m_addAvatarAct, &QAction::triggered, this, &UserListView::addAvatar);
    connect(m_removeAvatarAct, &QAction::triggered, this, &UserListView::deleteAvatar);
    setIconSize(QSize(64, 64));

    std::vector<std::tuple<QString, QString, Type>> propertyList
        = {{"healthPoints", tr("Health Points"), Integer}, {"maxHP", tr("Health Points Maximum"), Integer},
            {"minHP", tr("Health Points Minimum"), Integer}, {"distancePerTurn", tr("Distance per turn"), Real},
            {"initCommand", tr("Initiative Command"), String}, {"hasInitiative", tr("Has initiative"), Boolean}};

    for(auto propertyName : propertyList)
    {
        auto act= new QAction(std::get<1>(propertyName), this);
        std::pair<QString, Type> pair(std::get<0>(propertyName), std::get<2>(propertyName));
        if(std::get<2>(propertyName) == Boolean)
            act->setCheckable(true);

        QVariant var;
        var.setValue(pair);
        act->setData(var);
        connect(act, &QAction::triggered, this, &UserListView::setPropertyValue);
        m_propertyActions.push_back(act);
    }
}

void UserListView::setState()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    auto index= currentIndex();
    auto stateIdx= act->data().toInt();
    auto state= Character::getStateFromIndex(stateIdx);
    QString uuid= index.data(PlayersList::IdentifierRole).toString();
    auto tmpPlayer= dynamic_cast<Character*>(PlayersList::instance()->getPerson(uuid));

    if(nullptr == tmpPlayer || nullptr == state)
        return;

    tmpPlayer->setState(state);
}

void UserListView::setPropertyValue()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    auto index= currentIndex();
    QVariant value= act->data();
    auto pair= value.value<std::pair<QString, Type>>();
    QString uuid= index.data(PlayersList::IdentifierRole).toString();
    Person* tmpperso= PlayersList::instance()->getPerson(uuid);

    if(nullptr == tmpperso)
        return;

    auto formerVal= tmpperso->property(pair.first.toLocal8Bit());
    QVariant var;
    bool ok;

    switch(pair.second)
    {
    case Boolean:
        var= !formerVal.toBool();
        ok= true;
        break;
    case Integer:
        var= QInputDialog::getInt(this, tr("Get value for %1 property").arg(pair.first), tr("Value:"),
            formerVal.toInt(), -2147483647, 2147483647, 1, &ok);
        break;
    case Real:
        var= QInputDialog::getDouble(this, tr("Get value for %1 property").arg(pair.first), tr("Value:"),
            formerVal.toDouble(), -2147483647, 2147483647, 1, &ok);
        break;
    case String:
        var= QInputDialog::getText(this, tr("Get value for %1 property").arg(pair.first), tr("Value:"),
            QLineEdit::Normal, formerVal.toString(), &ok);
        break;
    }

    if(!ok)
        return;

    tmpperso->setProperty(pair.first.toLocal8Bit(), var);
}

void UserListView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex tmp= indexAt(event->pos());
    int indentationValue= indentation();
    int icon= 0;
    if(iconSize().isValid())
        icon= iconSize().width();
    else
        icon= indentationValue;

    if(tmp.isValid())
    {
        event->pos();
        int depth= 1;
        while(tmp.parent().isValid())
        {
            depth++;
            tmp= tmp.parent();
        }
        if((depth * indentationValue < event->pos().x()) && ((depth)*indentationValue + icon >= event->pos().x()))
        {
            editCurrentItemColor();
        }
        else
            QTreeView::mouseDoubleClickEvent(event);
    }
    else
        QTreeView::mouseDoubleClickEvent(event);
}

void UserListView::contextMenuEvent(QContextMenuEvent* e)
{
    QModelIndex index= indexAt(e->pos());
    if(!index.isValid())
        return;

    QString uuid= index.data(PlayersList::IdentifierRole).toString();

    Person* tmpperso= PlayersList::instance()->getPerson(uuid);

    auto flags= index.flags();

    if(!(flags & Qt::ItemIsEditable) || tmpperso == nullptr)
        return;

    auto charact= dynamic_cast<Character*>(tmpperso);
    QMenu popMenu(this);

    auto initCmd= charact->getInitCommand();
    if(!initCmd.isEmpty())
    {
        auto act= popMenu.addAction(tr("Roll initiative"));
        connect(act, &QAction::triggered, this, [this, initCmd, charact]() {
            if(m_diceParser->parseLine(initCmd))
            {
                m_diceParser->start();
                auto valueList= m_diceParser->getSumOfDiceResult();
                if(valueList.isEmpty())
                    return;

                charact->setInitiativeScore(static_cast<int>(valueList.first()));
            }
        });
    }

    popMenu.addAction(m_addAvatarAct);
    popMenu.addAction(m_removeAvatarAct);
    if(tmpperso->isLeaf())
    {
        popMenu.addSeparator();
        auto menu= popMenu.addMenu(tr("Set Property"));
        for(auto pro : m_propertyActions)
        {
            if(pro->isCheckable())
            {
                auto data= pro->data().value<std::pair<QString, Type>>();
                pro->setChecked(tmpperso->property(data.first.toLocal8Bit()).toBool());
            }
            menu->addAction(pro);
        }

        auto stateMenu= popMenu.addMenu(tr("State"));
        auto stateList= Character::getCharacterStateList();
        int i= 0;
        for(auto state : *stateList)
        {
            auto act= stateMenu->addAction(state->getLabel());
            act->setCheckable(true);
            auto statePerson= tmpperso->property("state").value<CharacterState*>();
            act->setChecked(state == statePerson);
            act->setData(i++);
            connect(act, &QAction::triggered, this, &UserListView::setState);
        }

        if(charact)
        {
            popMenu.addSeparator();
            auto actionList= charact->getActionList();
            if(!actionList.isEmpty())
            {
                auto actionMenu= popMenu.addMenu(tr("Action"));
                for(auto action : actionList)
                {
                    auto act= actionMenu->addAction(action->name());
                    connect(act, &QAction::triggered, this,
                        [this, &action, uuid]() { emit runDiceForCharacter(action->command(), uuid); });
                }
            }

            auto shapeMenu= popMenu.addMenu(tr("Shape"));
            auto shapeList= charact->getShapeList();
            for(auto shape : shapeList)
            {
                auto act= shapeMenu->addAction(shape->name());
                act->setCheckable(true);
                act->setChecked(charact->currentShape() == shape);
                connect(act, &QAction::triggered, this, [&charact, shape]() {
                    if(charact->currentShape() == shape)
                        charact->setCurrentShape(nullptr);
                    else
                        charact->setCurrentShape(shape);
                });
            }
        }
    }
    popMenu.exec(e->globalPos());
}

void UserListView::addAvatar()
{
    /// @TODO: Here! options manager is required to get access to the photo directory
    PreferencesManager* m_preferencesManager= PreferencesManager::getInstance();
    QString directory(".");
    if(nullptr != m_preferencesManager)
    {
        directory= m_preferencesManager->value("imageDirectory", QDir::homePath()).toString();
    }
    QString path= QFileDialog::getOpenFileName(
        this, tr("Avatar"), directory, tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp *.svg)"));
    QModelIndex index= currentIndex();
    if(path.isEmpty())
        return;
    if(index.isValid())
    {
        QString uuid= index.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso= PlayersList::instance()->getPerson(uuid);
        QImage im(path);
        tmpperso->setAvatar(im);
        emit m_model->dataChanged(index, index);
    }
}

void UserListView::deleteAvatar()
{
    QModelIndex index= currentIndex();
    if(index.isValid())
    {
        QString uuid= index.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso= PlayersList::instance()->getPerson(uuid);
        QImage im;
        tmpperso->setAvatar(im);
    }
}
void UserListView::editCurrentItemColor()
{
    QModelIndex index= currentIndex();

    if(!index.isValid())
        return;

    // QString uuid = index.data(PlayersList::IdentifierRole).toString();
    // Person* tmpperso = PlayersList::instance()->getPerson(uuid);

    QVariant valueVar= index.data(Qt::DecorationRole);

    QColor color= QColorDialog::getColor(valueVar.value<QColor>(), this);

    if(color.isValid())
    {
        model()->setData(index, QVariant(color), Qt::DecorationRole);
        // tmpperso->setColor(color);
    }
}
void UserListView::setPlayersListModel(PlayersListWidgetModel* model)
{
    QTreeView::setModel(model);
    m_model= model;
}

void UserListView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex tmp= indexAt(event->pos());

    QTreeView::mousePressEvent(event);

    if((event->buttons() == Qt::LeftButton) && (tmp.isValid()))
    {
        QString uuid= tmp.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso= PlayersList::instance()->getCharacter(uuid);
        if(nullptr != tmpperso)
        {
            QDrag* drag= new QDrag(this);
            RolisteamMimeData* mimeData= new RolisteamMimeData();

            mimeData->setPerson(tmpperso);
            drag->setMimeData(mimeData);
            drag->setPixmap(generateAvatar(tmpperso));

            // Qt::DropAction dropAction =
            drag->exec();
        }
    }
}
QPixmap UserListView::generateAvatar(Person* p)
{
    int diameter= 80;
    QPixmap img(diameter, diameter);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QBrush brush;
    if(p->getAvatar().isNull())
    {
        painter.setPen(p->getColor());
        brush.setColor(p->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        QImage img= p->getAvatar();
        brush.setTextureImage(img.scaled(diameter, diameter));
    }

    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, diameter, diameter, diameter / 10, diameter / 10);

    return img;
}
