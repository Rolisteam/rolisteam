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

#include <QColorDialog>
#include <QDebug>
#include <QDrag>
#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

#include "controller/view_controller/imageselectorcontroller.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "userlistview.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"

#include "controller/playercontroller.h"
#include "data/character.h"
#include "data/rolisteammimedata.h"
#include "model/characterstatemodel.h"
#include "model/playermodel.h"
#include "preferences/preferencesmanager.h"
#include <QInputDialog>

UserListView::UserListView(QWidget* parent) : QTreeView(parent)
{
    setHeaderHidden(true);

    // set action
    m_addAvatarAct= std::make_unique<QAction>(tr("Set Avatar..."), this);
    m_removeAvatarAct= std::make_unique<QAction>(tr("Remove..."), this);
    m_changeName= std::make_unique<QAction>(tr("Edit name"), this);
    m_changeColor= std::make_unique<QAction>(tr("Change color…"), this);

    connect(m_addAvatarAct.get(), &QAction::triggered, this, &UserListView::addAvatar);
    connect(m_removeAvatarAct.get(), &QAction::triggered, this, &UserListView::deleteAvatar);
    connect(m_changeName.get(), &QAction::triggered, this, [this]() { edit(currentIndex()); });
    connect(m_changeColor.get(), &QAction::triggered, this, &UserListView::editCurrentItemColor);

    setIconSize(QSize(64, 64));

    std::vector<std::tuple<QString, QString, Type>> propertyList
        = {{"healthPoints", tr("Health Points"), Integer},    {"maxHP", tr("Health Points Maximum"), Integer},
           {"minHP", tr("Health Points Minimum"), Integer},   {"distancePerTurn", tr("Distance per turn"), Real},
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

void UserListView::setPlayerController(PlayerController* ctrl)
{
    m_ctrl= ctrl;
}

void UserListView::setState()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    auto index= currentIndex();
    auto stateId= act->data().toString();
    /*auto state= Character::getStateFromIndex(stateIdx);
    auto person= static_cast<Person*>(index.internalPointer());
    auto tmpPlayer= dynamic_cast<Character*>(person);

    if(nullptr == tmpPlayer || nullptr == state)
        return;*/

    model()->setData(index, stateId, PlayerModel::CharacterStateIdRole);

    // tmpPlayer->setStateId(stateId);
}

void UserListView::setPropertyValue()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    auto index= currentIndex();
    QVariant value= act->data();
    auto pair= value.value<std::pair<QString, Type>>();
    auto person= static_cast<Person*>(index.internalPointer());
    Person* tmpperso= dynamic_cast<Character*>(person);

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

    setCurrentIndex(index);

    auto tmpperso= index.data(PlayerModel::PersonPtrRole).value<Person*>();

    auto flags= index.flags();

    if(!(flags & Qt::ItemIsEditable) || tmpperso == nullptr)
        return;

    auto uuid= tmpperso->uuid();
    auto charact= dynamic_cast<Character*>(tmpperso);
    QMenu popMenu(this);

    /*if(nullptr != charact)
    {
        auto initCmd= charact->getInitCommand();
        if(!initCmd.isEmpty())
        {
            //auto act= popMenu.addAction(tr("Roll initiative"));
            connect(act, &QAction::triggered, this, [this, initCmd, charact]() {
                if(m_diceParser->parseLine(initCmd))
                {
                    m_diceParser->start();
                    auto valueList= m_diceParser->getSumOfDiceResult();
                    if(valueList.isEmpty())
                        return;

                    charact->setInitiativeScore(static_cast<int>(valueList.first()))
                }
            });
        }
    }*/

    popMenu.addAction(m_changeName.get());
    popMenu.addAction(m_changeColor.get());
    popMenu.addSeparator();
    popMenu.addAction(m_addAvatarAct.get());
    popMenu.addAction(m_removeAvatarAct.get());
    popMenu.addSeparator();
    if(nullptr != charact)
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

        auto stateModel= m_ctrl->characterStateModel();
        if(stateModel)
        {
            auto stateMenu= popMenu.addMenu(tr("State"));
            for(int i= 0; i < stateModel->rowCount(); ++i)
            {
                auto label= stateModel->index(i, 0).data(CharacterStateModel::LABEL).toString();
                auto id= stateModel->index(i, 0).data(CharacterStateModel::ID).toString();

                auto act= stateMenu->addAction(label);
                act->setCheckable(true);
                auto stateIdPerson= charact->stateId();
                act->setChecked(id == stateIdPerson);
                act->setData(id);
                connect(act, &QAction::triggered, this, &UserListView::setState);
            }

            popMenu.addSeparator();
        }
        auto actionList= charact->actionList();
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

        auto shapeList= charact->shapeList();
        if(!shapeList.isEmpty())
        {
            auto shapeMenu= popMenu.addMenu(tr("Shape"));
            for(auto shape : shapeList)
            {
                auto act= shapeMenu->addAction(shape->name());
                act->setCheckable(true);
                act->setChecked(charact->currentShape() == shape);
                connect(act, &QAction::triggered, this,
                        [&charact, shape]()
                        {
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
    ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
    ImageSelectorDialog dialog(&ctrl, this);
    if(QDialog::Accepted != dialog.exec())
        return;

    QModelIndex index= currentIndex();
    auto data= ctrl.finalImageData();
    if(!index.isValid() || data.isEmpty())
        return;

    Person* tmpperso= static_cast<Person*>(index.internalPointer());
    if(tmpperso)
        tmpperso->setAvatar(data);
}

void UserListView::deleteAvatar()
{
    QModelIndex index= currentIndex();
    if(index.isValid())
    {
        Person* tmpperso= static_cast<Person*>(index.internalPointer());
        if(tmpperso)
            tmpperso->setAvatar({});
    }
}
void UserListView::editCurrentItemColor()
{
    QModelIndex index= currentIndex();

    if(!index.isValid())
        return;

    QVariant valueVar= index.data(Qt::DecorationRole);

    QColor color= QColorDialog::getColor(valueVar.value<QColor>(), this);

    if(color.isValid())
    {
        model()->setData(index, QVariant(color), Qt::DecorationRole);
    }
}

void UserListView::mouseMoveEvent(QMouseEvent* event)
{
    QModelIndex tmp= indexAt(event->pos());

    QTreeView::mousePressEvent(event);

    if((event->buttons() == Qt::LeftButton) && (tmp.isValid()))
    {
        Person* tmpperso= static_cast<Person*>(tmp.internalPointer());
        if(nullptr != tmpperso)
        {
            QDrag* drag= new QDrag(this);
            RolisteamMimeData* mimeData= new RolisteamMimeData();

            mimeData->setPerson(tmpperso);
            drag->setMimeData(mimeData);
            drag->setPixmap(helper::utils::roundCornerImage(IOHelper::dataToPixmap(tmpperso->avatar())));
            drag->exec();
        }
    }
}
/*QPixmap UserListView::generateAvatar(Person* p)
{
    int diameter= 80;
    QPixmap img(diameter, diameter);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QBrush brush;
    if(p->avatar().isNull())
    {
        painter.setPen(p->getColor());
        brush.setColor(p->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        QImage img= IOHelper::dataToImage(p->avatar());
        brush.setTextureImage(img.scaled(diameter, diameter));
    }

    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, diameter, diameter, diameter / 10, diameter / 10);

    return img;
}*/
