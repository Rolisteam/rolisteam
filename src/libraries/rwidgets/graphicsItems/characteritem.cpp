/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "characteritem.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "diceparser/dicealias.h"
#include "visualitem.h"

#define MARGING 1
#define MINI_VALUE 25
#define RADIUS_CORNER 10
#define MAX_CORNER_ITEM 6
#define DIRECTION_RADIUS_HANDLE 4
#define ANGLE_HANDLE 5
#define PEN_WIDTH 6
#define PEN_RADIUS 3

void updateListAlias(QList<DiceAlias*>& list)
{
    /*auto preferences= PreferencesManager::getInstance();
    list.clear();
    int size= preferences->value("DiceAliasNumber", 0).toInt();
    for(int i= 0; i < size; ++i)
    {
        QString cmd= preferences->value(QString("DiceAlias_%1_command").arg(i), "").toString();
        QString value= preferences->value(QString("DiceAlias_%1_value").arg(i), "").toString();
        bool replace= preferences->value(QString("DiceAlias_%1_type").arg(i), true).toBool();
        bool enable= preferences->value(QString("DiceAlias_%1_enable").arg(i), true).toBool();
        //list.append(new DiceAlias(cmd, value, replace, enable));
    }*/
}

QRect makeSquare(QRect rect)
{
    if(rect.width() < rect.height())
        rect.setWidth(rect.height());
    else
        rect.setHeight(rect.width());

    return rect;
}

QColor ContrastColor(QColor color)
{
    int d= 0;

    // Counting the perceptive luminance - human eye favors green color...
    double luminance= (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;

    if(luminance > 0.5)
        d= 0; // bright colors - black font
    else
        d= 255; // dark colors - white font

    return QColor(d, d, d);
}

//////////////////
// code of CharacterItem class.
/////////////////
CharacterItem::CharacterItem(vmap::CharacterItemController* ctrl)
    : VisualItem(ctrl)
    , m_itemCtrl(ctrl)
    , m_visionShapeDisk(new QAction(tr("Disk")))
    , m_visionShapeAngle(new QAction(tr("Angle")))
    , m_reduceLife(new QAction(tr("Reduce Life")))
    , m_increaseLife(new QAction(tr("Increase Life")))
{
    if(!m_itemCtrl)
        return;

    m_mapCtrl= m_itemCtrl->mapController();

    m_visionShapeDisk->setCheckable(true);
    m_visionShapeAngle->setCheckable(true);

    connect(m_visionShapeAngle.get(), &QAction::triggered, m_itemCtrl,
            [this]() { m_itemCtrl->setVisionShape(CharacterVision::ANGLE); });
    connect(m_visionShapeDisk.get(), &QAction::triggered, m_itemCtrl,
            [this]() { m_itemCtrl->setVisionShape(CharacterVision::DISK); });

    m_itemCtrl->setFont(QFont());

    connect(m_itemCtrl, &vmap::CharacterItemController::thumnailRectChanged, this, &CharacterItem::updateChildPosition);

    if(m_itemCtrl->playableCharacter())
    {
        auto vision= m_itemCtrl->vision();

        connect(vision, &CharacterVision::radiusChanged, this, &CharacterItem::updateChildPosition);
        connect(vision, &CharacterVision::angleChanged, this, &CharacterItem::updateChildPosition);
    }

    auto updateLambda= [this]() { update(); };
    connect(m_itemCtrl, &vmap::CharacterItemController::sideChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::stateColorChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::numberChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::thumnailRectChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::visionChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::visionShapeChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::textRectChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::textChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::hasAvatarChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::avatarChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::fontChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::radiusChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::remoteChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::stateIdChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::stateImageChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::modifiedChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::visibleChanged, this, updateLambda);
    connect(m_itemCtrl, &vmap::CharacterItemController::healthStatusVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::initScoreVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::healthBarVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::npcNameVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::npcNameChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::npcNumberVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::pcNameVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::stateLabelVisibleChanged, this, updateLambda);
    connect(m_mapCtrl, &VectorialMapController::characterVisionChanged, this, updateLambda);

    // createActions();
    for(int i= 0; i <= CharacterItem::SightLenght; ++i)
    {
        auto c= ChildPointItem::Control::Geometry;
        if(i == CharacterItem::SightAngle || i == CharacterItem::SightLenght)
            c= ChildPointItem::Control::Vision;
        ChildPointItem* tmp= new ChildPointItem(m_itemCtrl, i, this, c);
        tmp->setMotion(ChildPointItem::MOUSE);

        m_children.append(tmp);
    }
    updateChildPosition();
}

CharacterItem::~CharacterItem()
{
    qDebug() << "charcteritem destroyed";
}

QRectF CharacterItem::boundingRect() const
{
    return m_itemCtrl ? m_itemCtrl->thumnailRect().united(m_itemCtrl->textRect()) : QRectF();
}
QPainterPath CharacterItem::shape() const
{
    return m_itemCtrl->shape();
}

void CharacterItem::setNewEnd(const QPointF&) {}

void CharacterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    bool hasFocusOrChildren= hasFocusOrChild();
    setChildrenVisible(hasFocusOrChildren);
    emit selectStateChange(hasFocusOrChildren);
    auto rect= m_itemCtrl->rect();

    if(!m_itemCtrl)
        return;

    QString textToShow= m_itemCtrl->text();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->save();
    // Avatar
    if(m_itemCtrl->hasAvatar())
    {
        painter->drawImage(rect, *m_itemCtrl->avatar(), m_itemCtrl->avatar()->rect());
    }
    else
    {
        painter->setPen(m_itemCtrl->color());
        painter->setBrush(QBrush(m_itemCtrl->color(), Qt::SolidPattern));
        painter->drawEllipse(rect);
    }

    QPen pen= painter->pen();
    pen.setWidth(PEN_WIDTH);
    auto character= m_itemCtrl->character();
    if(nullptr != character)
    {

        if(!character->hasAvatar())
        {
            pen.setColor(m_itemCtrl->stateColor());
            painter->setPen(pen);
            painter->drawEllipse(m_itemCtrl->thumnailRect().adjusted(PEN_RADIUS, PEN_RADIUS, -PEN_RADIUS, -PEN_RADIUS));
        }
        else
        {
            pen.setWidth(PEN_WIDTH / 2);
            pen.setColor(m_itemCtrl->stateColor());
            painter->setPen(pen);
            int diam= static_cast<int>(m_itemCtrl->side());
            painter->drawRoundedRect(rect.x(), rect.y(), diam, diam, m_itemCtrl->side() / RADIUS_CORNER,
                                     m_itemCtrl->side() / RADIUS_CORNER);
        }

        auto stateImg= m_itemCtrl->stateImage();
        if(!stateImg.isNull())
        {
            painter->drawImage(rect, stateImg, stateImg.rect());
        }

        if(m_mapCtrl->initScoreVisible() && character->hasInitScore())
        {
            painter->save();
            auto init= QString("%1").arg(character->getInitiativeScore());
            auto chColor= character->getColor();
            auto color= ContrastColor(chColor);
            painter->setPen(color);
            auto font= painter->font();
            font.setBold(true);
            font.setPointSizeF(font.pointSizeF() * 2);
            painter->setFont(font);
            auto tl= rect.topLeft().toPoint();
            auto metric= painter->fontMetrics();
            auto rect= metric.boundingRect(init);
            rect.moveCenter(tl);
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(chColor, Qt::SolidPattern));
            auto square= makeSquare(rect);
            square.moveCenter(tl);
            painter->drawEllipse(square);
            painter->restore();
            painter->drawText(rect, Qt::AlignCenter, init);
            painter->restore();
        }
    }
    if(!textToShow.isEmpty())
    {
        setToolTip(textToShow);
        painter->setPen(m_itemCtrl->color());
        painter->drawText(m_itemCtrl->textRect(), Qt::AlignCenter, textToShow);
    }
    painter->restore();

    if(canBeMoved() && (option->state & QStyle::State_MouseOver || isSelected()))
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        if(m_itemCtrl->hasAvatar())
            painter->drawRect(rect);
        else
            painter->drawEllipse(rect);
        painter->restore();
    }

    if(m_itemCtrl->healthStatusVisible())
    {
        auto character= m_itemCtrl->character();
        if(nullptr == character)
            return;

        auto max= character->getHealthPointsMax();
        auto color= character->getLifeColor();
        auto min= character->getHealthPointsMin();
        auto current= character->getHealthPointsCurrent();
        QPen pen= painter->pen();
        pen.setColor(color);

        if(min < max)
        {
            QRectF bar(rect.x(), rect.height() + rect.y() - PEN_WIDTH, rect.width(), PEN_WIDTH);
            painter->save();
            auto newWidth= (current - min) * bar.width() / (max - min);
            painter->drawRect(bar);
            QRectF value(rect.x(), rect.height() + rect.y() - PEN_WIDTH, newWidth, PEN_WIDTH);
            painter->fillRect(value, color);
            painter->restore();
        }
    }
}

void CharacterItem::updateChildPosition()
{
    auto rect= m_itemCtrl->thumnailRect(); //(0, 0, m_itemCtrl->side(), m_itemCtrl->side());

    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);

    // setTransformOriginPoint(rect.center());
    if(m_itemCtrl->playableCharacter())
    {
        auto vision= m_itemCtrl->vision();
        if(vision)
        {
            m_children.value(DIRECTION_RADIUS_HANDLE)
                ->setPos(vision->radius() + m_itemCtrl->radius()
                             + m_children[DIRECTION_RADIUS_HANDLE]->boundingRect().width(),
                         m_itemCtrl->thumnailRect().height() / 2
                             - m_children[DIRECTION_RADIUS_HANDLE]->boundingRect().height() / 2);

            m_children[ANGLE_HANDLE]->setPos((vision->radius() + m_itemCtrl->radius()) / 2, -vision->angle());
        }
    }
    else
    {
        m_children[DIRECTION_RADIUS_HANDLE]->setVisible(false);
        m_children[ANGLE_HANDLE]->setVisible(false);
    }
    if(!m_itemCtrl->localIsGM())
    {
        setTransformOriginPoint(m_itemCtrl->thumnailRect().center());
    }
    update();
}
void CharacterItem::addActionContextMenu(QMenu& menu)
{
    QMenu* stateMenu= menu.addMenu(tr("Change State"));
    QList<CharacterState*>* listOfState= Character::getCharacterStateList();
    if(listOfState)
    {
        for(auto& state : *listOfState)
        {
            auto act= new QAction(QIcon(state->pixmap()), state->label(), this);
            act->setCheckable(true);

            if(m_itemCtrl->stateId() == state->id())
                act->setChecked(true);

            connect(act, &QAction::triggered, this,
                    [this, state](bool checked) { m_itemCtrl->setStateId(checked ? state->id() : QString()); });
            stateMenu->addAction(act);
        }
    }

    /*QMenu* user= menu.addMenu(tr("Transform into"));
    for(auto& character : PlayerModel::instance()->getCharacterList())
    {
        QAction* act= user->addAction(character->name());
        act->setData(character->getUuid());

        connect(act, &QAction::triggered, this, &CharacterItem::changeCharacter);
    }*/
    QMenu* shape= menu.addMenu(tr("Vision Shape"));
    shape->addAction(m_visionShapeDisk.get());
    shape->addAction(m_visionShapeAngle.get());

    m_visionShapeDisk->setChecked(CharacterVision::DISK == m_itemCtrl->visionShape());
    m_visionShapeAngle->setChecked(CharacterVision::ANGLE == m_itemCtrl->visionShape());

    if(m_ctrl->localIsGM())
    {
        // Actions
        auto actionlist= m_itemCtrl->actionList();
        if(!actionlist.isEmpty())
        {
            QMenu* actions= menu.addMenu(tr("Actions"));
            auto act= actions->addAction(tr("Initiative"));
            connect(act, &QAction::triggered, m_itemCtrl, &vmap::CharacterItemController::runInit);
            act= actions->addAction(tr("Clean Initiative"));
            connect(act, &QAction::triggered, m_itemCtrl, &vmap::CharacterItemController::cleanInit);

            int i= 0;
            std::for_each(std::begin(actionlist), std::end(actionlist),
                          [this, actions, &i](CharacterAction* charAction)
                          {
                              auto act= actions->addAction(charAction->name());
                              connect(act, &QAction::triggered, m_itemCtrl, [this, i]() { m_itemCtrl->runCommand(i); });
                              i++;
                          });
        }

        // Shapes
        auto shapeList= m_itemCtrl->shapeList();
        if(!shapeList.isEmpty())
        {
            QMenu* shapeMenu= menu.addMenu(tr("Shapes"));
            int i= 0;
            for(auto& charShape : shapeList)
            {
                auto act= shapeMenu->addAction(charShape->name());
                connect(act, &QAction::triggered, this, [i, this]() { m_itemCtrl->setShape(i); });
            }
            auto action= shapeMenu->addAction(tr("Clean Shape"));
            connect(action, &QAction::triggered, m_itemCtrl, &vmap::CharacterItemController::cleanShape);
        }
    }

    VisualItem::addActionContextMenu(menu);
}

void CharacterItem::changeCharacter()
{
    /*    QAction* act= qobject_cast<QAction*>(sender());
        QString uuid= act->data().toString();*/

    /*Character* tmp= PlayerModel::instance()->getCharacter(uuid);

    Character* old= m_character;
    if(nullptr != tmp)
    {
        setCharacter(tmp);
        generatedThumbnail();
        emit ownerChanged(old, this);
        emit itemGeometryChanged(this);
    }*/
}

void CharacterItem::addChildPoint(ChildPointItem* item)
{
    m_children.append(item);
}

void CharacterItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    /*    if((nullptr != m_character) && (event->modifiers() & Qt::AltModifier))
        {
            auto hp= m_character->getHealthPointsCurrent();
            auto delta= event->delta();
            if(delta > 0)
                ++hp;
            else
                --hp;
            m_character->setHealthPointsCurrent(hp);
            event->accept();
            update();
        }
        else
            VisualItem::wheelEvent(event);*/
}
