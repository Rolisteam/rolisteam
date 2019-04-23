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

#include "data/character.h"
#include "data/player.h"
#include "map/map.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "userlist/playersList.h"
#include "vmap/vmap.h"

#define MARGING 1
#define MINI_VALUE 25
#define RADIUS_CORNER 10
#define MAX_CORNER_ITEM 6
#define DIRECTION_RADIUS_HANDLE 4
#define ANGLE_HANDLE 5

void updateListAlias(QList<DiceAlias*>* list)
{
    auto preferences= PreferencesManager::getInstance();
    list->clear();
    int size= preferences->value("DiceAliasNumber", 0).toInt();
    for(int i= 0; i < size; ++i)
    {
        QString cmd= preferences->value(QString("DiceAlias_%1_command").arg(i), "").toString();
        QString value= preferences->value(QString("DiceAlias_%1_value").arg(i), "").toString();
        bool replace= preferences->value(QString("DiceAlias_%1_type").arg(i), true).toBool();
        bool enable= preferences->value(QString("DiceAlias_%1_enable").arg(i), true).toBool();
        list->append(new DiceAlias(cmd, value, replace, enable));
    }
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

CharacterItem::CharacterItem()
    : VisualItem(), m_character(nullptr), m_thumnails(nullptr), m_protectGeometryChange(false), m_visionChanged(false)
{
    createActions();
}

CharacterItem::CharacterItem(Character* m, const QPointF& pos, qreal diameter)
    : VisualItem()
    , m_center(pos)
    , m_diameter(diameter)
    , m_thumnails(nullptr)
    , m_protectGeometryChange(false)
    , m_visionChanged(false)
{
    setCharacter(m);
    setPos(m_center - QPointF(diameter / 2, diameter / 2));
    sizeChanged(diameter);
    createActions();
}

void CharacterItem::writeData(QDataStream& out) const
{
    out << m_center;
    int diam= static_cast<int>(m_diameter);
    out << diam;
    out << *m_thumnails;
    out << m_rect;
    out << opacity();
    out << static_cast<int>(m_layer);
    // out << zValue();
    if(nullptr != m_character)
    {
        out << true;
        m_character->writeData(out);
    }
    else
    {
        out << false;
    }
}

void CharacterItem::readData(QDataStream& in)
{
    in >> m_center;
    int diam;
    in >> diam;
    m_diameter= diam;
    m_thumnails= new QPixmap();
    in >> *m_thumnails;
    in >> m_rect;

    qreal opa= 0;
    in >> opa;
    setOpacity(opa);

    int tmp;
    in >> tmp;
    m_layer= static_cast<VisualItem::Layer>(tmp);
    bool hasCharacter;
    in >> hasCharacter;
    auto charact= new Character();
    if(hasCharacter)
    {
        charact->readData(in);
    }
    setCharacter(charact);
}
VisualItem::ItemType CharacterItem::getType() const
{
    return VisualItem::CHARACTER;
}
QRectF CharacterItem::boundingRect() const
{
    QRectF uni= m_rect.united(m_rectText);
    return uni;
}
QPainterPath CharacterItem::shape() const
{
    QPainterPath path;
    path.moveTo(0, 0);
    if((nullptr == m_thumnails) || (m_thumnails->isNull()))
    {
        path.addEllipse(boundingRect());
    }
    else
    {
        path.addRoundedRect(0, 0, m_diameter, m_diameter, m_diameter / 10, m_diameter / 10);
    }
    path.addRect(m_rectText);
    return path;
}
void CharacterItem::setNewEnd(QPointF& nend){
    Q_UNUSED(nend)
    // m_center = nend;
} QString CharacterItem::getSubTitle() const
{
    QString toShow;
    if(m_character->isNpc())
    {
        if(getOption(VisualItem::ShowNpcName).toBool())
        {
            toShow= m_character->name();
        }
        if(getOption(VisualItem::ShowNpcNumber).toBool())
        {
            toShow= QString::number(m_character->number());
        }
        if(getOption(VisualItem::ShowNpcName).toBool() && getOption(VisualItem::ShowNpcNumber).toBool())
        {
            toShow= QString("%1 - %2").arg(m_character->name()).arg(m_character->number());
        }
    }
    else if(getOption(VisualItem::ShowPcName).toBool())
    {
        toShow= m_character->name();
    }
    return toShow;
}
void CharacterItem::setChildrenVisible(bool b)
{
    VisualItem::setChildrenVisible(b);

    if(m_child && (getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE || isNpc()))
    {
        if(!getOption(VisualItem::LocalIsGM).toBool() || isNpc())
        {
            if(!m_child->isEmpty() && m_child->size() > DIRECTION_RADIUS_HANDLE)
            {
                m_child->at(DIRECTION_RADIUS_HANDLE)->setVisible(false);
            }
            if(!m_child->isEmpty() && m_child->size() > ANGLE_HANDLE)
            {
                m_child->at(ANGLE_HANDLE)->setVisible(false);
            }
        }
    }
}
#define PEN_WIDTH 6
#define PEN_RADIUS 3
void CharacterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(m_thumnails == nullptr)
    {
        generatedThumbnail();
    }
    bool hasFocusOrChildren= hasFocusOrChild();
    setChildrenVisible(hasFocusOrChildren);
    emit selectStateChange(hasFocusOrChildren);

    QString toShow= getSubTitle();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->save();
    if(m_character->hasAvatar())
    {
        painter->drawPixmap(m_rect, *m_thumnails, m_thumnails->rect());
    }
    else
    {
        painter->setPen(m_character->getColor());
        painter->setBrush(QBrush(m_character->getColor(), Qt::SolidPattern));
        painter->drawEllipse(m_rect);
    }

    QPen pen= painter->pen();
    pen.setWidth(PEN_WIDTH);
    if(nullptr != m_character)
    {
        if(nullptr != m_character->getState())
        {
            if(getOption(VisualItem::ShowHealthStatus).toBool())
            {
                toShow+= QString(" %1").arg(m_character->getState()->getLabel());
            }
            if(!m_character->getState()->getImage().isNull())
            {
                painter->drawPixmap(
                    m_rect, m_character->getState()->getImage(), m_character->getState()->getImage().rect());
            }
            else if(!m_character->hasAvatar())
            {
                pen.setColor(m_character->getState()->getColor());
                painter->setPen(pen);
                painter->drawEllipse(m_rect.adjusted(PEN_RADIUS, PEN_RADIUS, -PEN_RADIUS, -PEN_RADIUS));
            }
            else
            {
                pen.setWidth(PEN_WIDTH / 2);
                pen.setColor(m_character->getState()->getColor());
                painter->setPen(pen);
                int diam= static_cast<int>(m_diameter);
                painter->drawRoundedRect(0, 0, diam, diam, m_diameter / RADIUS_CORNER, m_diameter / RADIUS_CORNER);
            }
        }
        if(getOption(VisualItem::ShowInitScore).toBool() && m_character->hasInitScore())
        {
            painter->save();
            auto init= QString("%1").arg(m_character->getInitiativeScore());
            auto chColor= m_character->getColor();
            auto color= ContrastColor(chColor);
            painter->setPen(color);
            auto font= painter->font();
            font.setBold(true);
            font.setPointSizeF(font.pointSizeF() * 2);
            painter->setFont(font);
            auto tl= m_rect.topLeft().toPoint();
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
            toShow+= QString(" %1: %2").arg(tr("Init", "short for Initiative"), init);
            painter->restore();
        }
    }
    // QRectF rectText;
    QFontMetrics metric(painter->font());
    m_rectText.setRect(m_rect.center().x() - ((metric.boundingRect(toShow).width() + MARGING) / 2), m_rect.bottom(),
        metric.boundingRect(toShow).width() + MARGING + MARGING, metric.height());

    if(!toShow.isEmpty())
    {
        if(toShow != m_title)
        {
            m_title= toShow;
            if((m_propertiesHash->value(VisualItem::FogOfWarStatus).toBool() == false)
                || (m_propertiesHash->value(VisualItem::LocalIsGM).toBool() == true))
            {
                setToolTip(m_title);
            }
            else
            {
                setToolTip("");
            }
        }
        painter->setPen(m_character->getColor());
        painter->drawText(m_rectText, Qt::AlignCenter, toShow);
    }
    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        if(m_character->hasAvatar())
            painter->drawRect(m_rect);
        else
            painter->drawEllipse(m_rect);
        painter->restore();
    }

    if(getOption(VisualItem::ShowHealthBar).toBool())
    {
        if(nullptr != m_character)
        {
            auto max= m_character->getHealthPointsMax();
            auto color= m_character->getLifeColor();
            auto min= m_character->getHealthPointsMin();
            auto current= m_character->getHealthPointsCurrent();
            QPen pen= painter->pen();
            pen.setColor(color);

            if(min < max)
            {
                QRectF bar(m_rect.x(), m_rect.height() - PEN_WIDTH, m_rect.width(), PEN_WIDTH);
                painter->save();
                auto newWidth= (current - min) * bar.width() / (max - min);
                painter->drawRect(bar);
                QRectF value(m_rect.x(), m_rect.height() - PEN_WIDTH, newWidth, PEN_WIDTH);
                painter->fillRect(value, color);
                painter->restore();
            }
        }
    }
}
const QPointF& CharacterItem::getCenter() const
{
    return m_center;
}
void CharacterItem::sizeChanged(qreal m_size)
{
    m_diameter= m_size;
    m_rect.setRect(0, 0, m_diameter, m_diameter);
    generatedThumbnail();
    m_resizing= true;
}
void CharacterItem::visionChanged()
{
    m_visionChanged= true;
}
void CharacterItem::setSize(QSizeF size)
{
    m_protectGeometryChange= true;
    sizeChanged(size.width());
    updateChildPosition();
    m_protectGeometryChange= false;
    update();
}
void CharacterItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    VisualItem::setRectSize(x, y, w, h);
    m_diameter= m_rect.width();
    generatedThumbnail();
    updateChildPosition();
}
void CharacterItem::generatedThumbnail()
{
    if(m_thumnails != nullptr)
    {
        delete m_thumnails;
        m_thumnails= nullptr;
    }
    int diam= static_cast<int>(m_diameter);
    m_thumnails= new QPixmap(diam, diam);
    m_thumnails->fill(Qt::transparent);
    QPainter painter(m_thumnails);
    QBrush brush;
    if(m_character->getAvatar().isNull())
    {
        painter.setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        QImage img= m_character->getAvatar();
        brush.setTextureImage(img.scaled(diam, diam));
    }

    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, diam, diam, m_diameter / RADIUS_CORNER, m_diameter / RADIUS_CORNER);
}
qreal CharacterItem::getRadius() const
{
    return m_diameter / 2;
}
void CharacterItem::fillMessage(NetworkMessageWriter* msg)
{
    if(nullptr == m_character || nullptr == m_vision || nullptr == msg)
        return;

    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    msg->string16(m_character->getUuid());
    msg->real(m_diameter);

    msg->uint8(m_layer);
    msg->real(zValue());
    msg->real(opacity());

    // pos
    msg->real(pos().x());
    msg->real(pos().y());

    msg->real(m_center.x());
    msg->real(m_center.y());

    // rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

    // path
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_7);
    if((m_thumnails == nullptr) || (m_thumnails->isNull()))
    {
        generatedThumbnail();
    }
    in << *m_thumnails;
    msg->byteArray32(data);

    m_character->fill(*msg, true);
    m_vision->fill(msg);
}
void CharacterItem::readItem(NetworkMessageReader* msg)
{
    m_id= msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    QString idCharacter= msg->string16();
    m_diameter= msg->real();

    m_layer= static_cast<VisualItem::Layer>(msg->uint8());

    setZValue(msg->real());
    setOpacity(msg->real());

    qreal x= msg->real();
    qreal y= msg->real();

    setPos(x, y);
    // pos
    m_center.setX(msg->real());
    m_center.setY(msg->real());
    // rect

    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    // path
    QByteArray data;
    data= msg->byteArray32();

    QDataStream out(&data, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_7);
    m_thumnails= new QPixmap();
    out >> *m_thumnails;

    Character* tmp= PlayersList::instance()->getCharacter(idCharacter);

    if(nullptr != tmp)
    {
        tmp->read(*msg);
    }
    else
    {
        /// @todo This code may no longer be needed.
        tmp= new Character();
        QString id= tmp->read(*msg);
        tmp->setParentPerson(PlayersList::instance()->getPlayer(id));
    }
    setCharacter(tmp);
    generatedThumbnail();

    updateItemFlags();

    if(nullptr != m_vision)
    {
        m_vision->readMessage(msg);
    }
}
void CharacterItem::resizeContents(const QRectF& rect, TransformType)
{
    if(!rect.isValid())
        return;

    prepareGeometryChange();
    m_rect= rect;
    m_diameter= qMin(m_rect.width(), m_rect.height());
    sizeChanged(m_diameter);
    updateChildPosition();
}
QString CharacterItem::getCharacterId() const
{
    if(nullptr != m_character)
    {
        return m_character->getUuid();
    }
    return QString();
}
void CharacterItem::setNumber(int n)
{
    if(nullptr == m_character)
        return;

    m_character->setNumber(n);
}
QString CharacterItem::getName() const
{
    if(nullptr == m_character)
        return {};

    return m_character->name();
}
int CharacterItem::getNumber() const
{
    if(nullptr == m_character)
        return {};

    return m_character->number();
}
QVariant CharacterItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    QVariant newValue= value;
    if(change == QGraphicsItem::ItemPositionChange)
    {
        if(!getOption(VisualItem::CollisionStatus).toBool())
        {
            return VisualItem::itemChange(change, newValue);
        }
        m_oldPosition= pos();
        QList<QGraphicsItem*> list= collidingItems();

        // list.clear();
        QPainterPath path;
        path.addPath(mapToScene(shape()));
        path.connectPath(mapToScene(shape().translated(value.toPointF() - pos())));

        QGraphicsScene* currentScene= scene();
        list.append(currentScene->items(path));

        for(QGraphicsItem* item : list)
        {
            VisualItem* vItem= dynamic_cast<VisualItem*>(item);
            if((nullptr != vItem) && (vItem != this))
            {
                if((vItem->getLayer() == VisualItem::OBJECT))
                {
                    newValue= m_oldPosition;
                }
            }
        }
        QVariant var= VisualItem::itemChange(change, newValue);
        if(newValue != m_oldPosition)
        {
            emit positionChanged();
        }
        return var;
    }
    else
    {
        return VisualItem::itemChange(change, newValue);
    }
}
int CharacterItem::getChildPointCount() const
{
    return m_child->size();
}
void CharacterItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    QRectF rect= m_rect;
    if(m_protectGeometryChange)
        return;
    switch(static_cast<int>(pointId))
    {
    case 0:
        rect.setTopLeft(pos);
        break;
    case 1:
        pos.setY(rect.topRight().x() - pos.x());
        rect.setTopRight(pos);
        break;
    case 2:
        rect.setBottomRight(pos);
        break;
    case 3:
        pos.setX(rect.bottomLeft().y() - pos.y());
        rect.setBottomLeft(pos);
        break;
    case DIRECTION_RADIUS_HANDLE:
        if(pos.x() - (m_rect.width() / 2) < 0)
        {
            pos.setX(m_rect.width() / 2);
        }
        m_vision->setRadius(pos.x() - (getRadius() * 2) + m_child->at(4)->boundingRect().width() + m_rect.width() / 2);
        visionChanged();
        break;
    case ANGLE_HANDLE:
    {
        if(!qFuzzyCompare(pos.x(), ((m_vision->getRadius() + getRadius()) / 2)))
        {
            pos.setX((m_vision->getRadius() + getRadius()) / 2);
        }
        if(pos.y() < -360)
        {
            pos.setY(-360);
        }

        if(pos.y() > 0)
        {
            pos.setY(0);
        }
        qreal angle= 360 * (std::fabs(pos.y()) / 360);
        m_vision->setAngle(angle);
        visionChanged();
    }
    break;
    default:
        // emit geometryChangeOnUnkownChild(pointId,pos);
        break;
    }
    if(rect.width() < MINI_VALUE)
    {
        rect.setWidth(MINI_VALUE);
    }
    if(rect.height() < MINI_VALUE)
    {
        rect.setHeight(MINI_VALUE);
    }
    m_diameter= qMin(rect.width(), rect.height());
    sizeChanged(m_diameter);
    switch(static_cast<int>(pointId))
    {
    case 0:
        pos= m_rect.topLeft();
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(4)->setPos(
            m_vision->getRadius(), m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
        // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width()+m_rect.width()/2);
        break;
    case 1:
        pos= m_rect.topRight();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(4)->setPos(
            m_vision->getRadius(), m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
        // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 2:
        pos= m_rect.bottomRight();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(4)->setPos(
            m_vision->getRadius(), m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
        // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 3:
        pos= m_rect.bottomLeft();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(4)->setPos(
            m_vision->getRadius(), m_rect.height() / 2 - m_child->value(4)->boundingRect().height() / 2);
        // m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case DIRECTION_RADIUS_HANDLE:
        m_child->value(ANGLE_HANDLE)->setPos((m_vision->getRadius() + getRadius()) / 2, -m_vision->getAngle());
        break;
    case ANGLE_HANDLE:
        break;
    default:
        break;
    }

    setTransformOriginPoint(m_rect.center());
}
void CharacterItem::initChildPointItem()
{
    if(nullptr != m_child)
    {
        qDeleteAll(m_child->begin(), m_child->end());
        delete m_child;
    }
    m_child= new QVector<ChildPointItem*>();

    for(int i= 0; i < MAX_CORNER_ITEM; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(i, this, (i == DIRECTION_RADIUS_HANDLE));
        tmp->setMotion(ChildPointItem::ALL);
        tmp->setRotationEnable(true);
        m_child->append(tmp);
    }

    m_child->at(DIRECTION_RADIUS_HANDLE)->setMotion(ChildPointItem::X_AXIS);
    m_child->at(DIRECTION_RADIUS_HANDLE)->setRotationEnable(false);
    m_child->at(DIRECTION_RADIUS_HANDLE)->setVisible(false);

    m_child->at(ANGLE_HANDLE)->setMotion(ChildPointItem::Y_AXIS);
    m_child->at(ANGLE_HANDLE)->setRotationEnable(false);
    m_child->at(ANGLE_HANDLE)->setVisible(false);
    updateChildPosition();
}

void CharacterItem::initChildPointItemMotion()
{
    int i= 0;
    for(auto& itemChild : *m_child)
    {
        itemChild->setEditableItem(true);
        switch(i)
        {
        case DIRECTION_RADIUS_HANDLE:
            itemChild->setMotion(ChildPointItem::X_AXIS);
            break;
        case ANGLE_HANDLE:
            itemChild->setMotion(ChildPointItem::Y_AXIS);
            break;
        default:
            itemChild->setMotion(ChildPointItem::ALL);
            break;
        }
        itemChild->setRotationEnable(true);
    }
}

ChildPointItem* CharacterItem::getRadiusChildWidget()
{
    if(m_child->size() >= 5)
    {
        return m_child->value(DIRECTION_RADIUS_HANDLE);
    }
    return nullptr;
}
QColor CharacterItem::getColor()
{
    if(nullptr != m_character)
        return m_character->getColor();

    return {};
}

void CharacterItem::updateChildPosition()
{
    m_child->value(0)->setPos(m_rect.topLeft());
    m_child->value(0)->setPlacement(ChildPointItem::TopLeft);
    m_child->value(1)->setPos(m_rect.topRight());
    m_child->value(1)->setPlacement(ChildPointItem::TopRight);
    m_child->value(2)->setPos(m_rect.bottomRight());
    m_child->value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_child->value(3)->setPos(m_rect.bottomLeft());
    m_child->value(3)->setPlacement(ChildPointItem::ButtomLeft);

    m_child->value(DIRECTION_RADIUS_HANDLE)
        ->setPos(m_vision->getRadius() + getRadius(),
            m_rect.height() / 2 - m_child->value(DIRECTION_RADIUS_HANDLE)->boundingRect().height() / 2);

    m_child->value(ANGLE_HANDLE)->setPos((m_vision->getRadius() + getRadius()) / 2, -m_vision->getAngle());
    m_child->value(ANGLE_HANDLE)->setVisionHandler(true);

    setTransformOriginPoint(m_rect.center());

    update();
}
void CharacterItem::addActionContextMenu(QMenu& menu)
{
    QMenu* stateMenu= menu.addMenu(tr("Change State"));
    QList<CharacterState*>* listOfState= Character::getCharacterStateList();
    for(auto& state : *listOfState)
    {
        QAction* act
            = stateMenu->addAction(QIcon(*state->getPixmap()), state->getLabel(), this, SLOT(characterStateChange()));
        act->setData(listOfState->indexOf(state));
    }

    QMenu* user= menu.addMenu(tr("Affect to"));
    for(auto& character : PlayersList::instance()->getCharacterList())
    {
        QAction* act= user->addAction(character->name());
        act->setData(character->getUuid());

        connect(act, &QAction::triggered, this, &CharacterItem::changeCharacter);
    }
    QMenu* shape= menu.addMenu(tr("Vision Shape"));
    shape->addAction(m_visionShapeDisk);
    shape->addAction(m_visionShapeAngle);

    if(CharacterVision::DISK == m_vision->getShape())
    {
        m_visionShapeDisk->setChecked(true);
        m_visionShapeAngle->setChecked(false);
    }
    else
    {
        m_visionShapeDisk->setChecked(false);
        m_visionShapeAngle->setChecked(true);
    }
    if(getOption(VisualItem::LocalIsGM).toBool() && nullptr != m_character)
    {
        // Actions
        auto actionlist= m_character->getActionList();
        QMenu* actions= menu.addMenu(tr("Actions"));
        auto cmd= m_character->getInitCommand();
        auto act= actions->addAction(tr("Initiative"));
        act->setData(cmd);
        connect(act, &QAction::triggered, this, &CharacterItem::runInit);

        act= actions->addAction(tr("Clean Initiative"));
        connect(act, &QAction::triggered, this, &CharacterItem::cleanInit);

        if(!actionlist.isEmpty())
        {
            for(auto& charAction : actionlist)
            {
                auto act= actions->addAction(charAction->name());
                act->setData(charAction->command());
                connect(act, &QAction::triggered, this, &CharacterItem::runCommand);
            }
        }

        // Shapes
        auto shapeList= m_character->getShapeList();
        if(!shapeList.isEmpty())
        {
            QMenu* actions= menu.addMenu(tr("Shapes"));
            int i= 0;
            for(auto& charShape : shapeList)
            {
                auto act= actions->addAction(charShape->name());
                act->setData(i);
                connect(act, &QAction::triggered, this, &CharacterItem::setShape);
                ++i;
            }
            auto action= actions->addAction(tr("Clean Shape"));
            connect(action, &QAction::triggered, this, [=]() {
                if(nullptr == m_character)
                    return;
                m_character->setDefaultShape();
                update();
            });
        }
    }
}

void CharacterItem::runInit()
{
    if(nullptr == m_character)
        return;

    auto cmd= m_character->getInitCommand();

    updateListAlias(m_diceParser.getAliases());
    if(m_diceParser.parseLine(cmd))
    {
        m_diceParser.start();
        if(!m_diceParser.getErrorMap().isEmpty())
            qWarning() << m_diceParser.humanReadableError();
        auto result= m_diceParser.getLastIntegerResults();
        int sum= std::accumulate(result.begin(), result.end(), 0);
        m_character->setInitiativeScore(sum);
        update();
    }
}
void CharacterItem::cleanInit()
{
    if(nullptr == m_character)
        return;
    m_character->clearInitScore();
    update();
}
void CharacterItem::runCommand()
{
    if(nullptr == m_character)
        return;
    auto act= qobject_cast<QAction*>(sender());
    auto cmd= act->data().toString();

    emit runDiceCommand(cmd, m_character->getUuid());
}

void CharacterItem::setShape()
{
    if(nullptr == m_character)
        return;
    auto act= qobject_cast<QAction*>(sender());
    auto index= act->data().toInt();

    m_character->setCurrentShape(index);
    update();
}

void CharacterItem::changeCharacter()
{
    QAction* act= qobject_cast<QAction*>(sender());
    QString uuid= act->data().toString();

    Character* tmp= PlayersList::instance()->getCharacter(uuid);

    Character* old= m_character;
    if(nullptr != tmp)
    {
        setCharacter(tmp);
        generatedThumbnail();
        emit ownerChanged(old, this);
        emit itemGeometryChanged(this);
    }
}

void CharacterItem::createActions()
{
    /*auto effect= new QGraphicsDropShadowEffect();
    effect->setOffset(2., 2.);
    effect->setColor(QColor(0, 0, 0, 191));
    setGraphicsEffect(effect);*/
    updateListAlias(m_diceParser.getAliases());
    m_vision.reset(new CharacterVision(this));

    m_visionShapeDisk= new QAction(tr("Disk"), this);
    m_visionShapeDisk->setCheckable(true);
    m_visionShapeAngle= new QAction(tr("Conical"), this);
    m_visionShapeAngle->setCheckable(true);

    connect(m_visionShapeAngle, SIGNAL(triggered()), this, SLOT(changeVisionShape()));
    connect(m_visionShapeDisk, SIGNAL(triggered()), this, SLOT(changeVisionShape()));

    m_reduceLife= new QAction(tr("Reduce Life"), this);
    m_increaseLife= new QAction(tr("Increase Life"), this);

    connect(m_reduceLife, &QAction::triggered, this, [this]() {
        if(nullptr == m_character)
            return;
        auto i= m_character->getHealthPointsCurrent();
        m_character->setHealthPointsCurrent(i);
    });

    connect(m_increaseLife, &QAction::triggered, this, [this]() {
        if(nullptr == m_character)
            return;
        auto i= m_character->getHealthPointsCurrent();
        m_character->setHealthPointsCurrent(i);
    });

    connect(
        PlayersList::instance(), SIGNAL(characterDeleted(Character*)), this, SLOT(characterHasBeenDeleted(Character*)));
}
void CharacterItem::changeVisionShape()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(act == m_visionShapeDisk)
    {
        m_visionShapeAngle->setChecked(false);
        m_visionShapeDisk->setChecked(true);
        m_vision->setShape(CharacterVision::DISK);
    }
    else if(act == m_visionShapeAngle)
    {
        m_visionShapeAngle->setChecked(true);
        m_visionShapeDisk->setChecked(false);
        m_vision->setShape(CharacterVision::ANGLE);
    }
}

void CharacterItem::characterStateChange()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    if(nullptr == m_character)
        return;

    int index= act->data().toInt();

    CharacterState* state= Character::getCharacterStateList()->at(index);
    m_character->setState(state);

    NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::VMapCategory, NetMsg::CharacterStateChanged);
    msg->string8(getMapId());
    msg->string8(m_id);
    msg->string8(m_character->getUuid());
    msg->uint16(static_cast<quint16>(index));
    msg->sendToServer();
}

void CharacterItem::updateCharacter()
{
    if(nullptr == m_character)
        return;

    NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::VMapCategory, NetMsg::CharacterChanged);
    msg->string8(getMapId());
    msg->string8(m_id);
    msg->string8(m_character->getUuid());
    m_character->fill(*msg, true);
    msg->sendToServer();
}

VisualItem* CharacterItem::getItemCopy()
{
    CharacterItem* charactItem= new CharacterItem(m_character, pos(), m_diameter);
    charactItem->setPos(pos());
    return charactItem;
}

QString CharacterItem::getParentId() const
{
    if(nullptr != m_character)
    {
        Person* pers= m_character->parentPerson();
        if(nullptr != pers)
        {
            return pers->getUuid();
        }
    }
    return QString();
}
void CharacterItem::readCharacterStateChanged(NetworkMessageReader& msg)
{
    int index= msg.uint16();
    if(nullptr != m_character)
    {
        QList<CharacterState*>* list= Character::getCharacterStateList();
        if(nullptr != list)
        {
            if(!list->isEmpty() && index < list->size())
            {
                CharacterState* state= Character::getCharacterStateList()->at(index);
                if(nullptr != state)
                {
                    m_character->setState(state);
                    update();
                }
            }
        }
    }
}

void CharacterItem::readCharacterChanged(NetworkMessageReader& msg)
{
    if(nullptr == m_character)
        return;

    m_character->read(msg);
    update();
}

void CharacterItem::characterHasBeenDeleted(Character* pc)
{
    if(pc == m_character)
    {
        m_character= nullptr;
    }
}

void CharacterItem::addChildPoint(ChildPointItem* item)
{
    if(nullptr != m_child)
    {
        // item->setPointID(m_child->size());
        m_child->append(item);
    }
}
void CharacterItem::setDefaultVisionParameter(CharacterVision::SHAPE shape, qreal radius, qreal angle)
{
    m_vision->setAngle(angle);
    m_vision->setRadius(radius);
    m_vision->setShape(shape);
}
CharacterVision* CharacterItem::getVision() const
{
    return m_vision.get();
}
/*bool CharacterItem::canBeMoved() const
{
    bool movable = false;
    if(getOption(VisualItem::LocalIsGM).toBool())
    {
        movable = true;
    }
    else if((getOption(VisualItem::PermissionMode).toInt()==Map::PC_MOVE) && (isLocal()))
    {
        movable = true;
    }
    else if(getOption(VisualItem::PermissionMode).toInt()==Map::PC_ALL)
    {
        movable = true;
    }
    else
    {
        movable = VisualItem::canBeMoved();
    }
    return movable;
}*/
void CharacterItem::readPositionMsg(NetworkMessageReader* msg)
{
    auto z= zValue();
    VisualItem::readPositionMsg(msg);
    if(isLocal())
    {
        blockSignals(true);
        setZValue(z);
        blockSignals(false);
    }
    update();
}
bool CharacterItem::isLocal() const
{
    PlayersList* model= PlayersList::instance();
    if(nullptr == model)
        return false;

    return model->isLocal(m_character);
}
void CharacterItem::sendVisionMsg()
{
    if(hasPermissionToMove()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::VisionChanged);
        msg.string8(m_mapId);
        msg.string16(getCharacterId());
        msg.string16(m_id);
        m_vision->fill(&msg);
        msg.sendToServer();
    }
}
void CharacterItem::readVisionMsg(NetworkMessageReader* msg)
{
    m_vision->readMessage(msg);
    update();
}

void CharacterItem::endOfGeometryChange()
{
    if(m_visionChanged)
    {
        sendVisionMsg();
        m_visionChanged= false;
    }
    VisualItem::endOfGeometryChange();
}
void CharacterItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    if(canBeMoved())
    {
        if(nullptr != m_child)
        {
            initChildPointItemMotion();
            /*for(auto& itemChild : *m_child)
            {
                itemChild->setEditableItem(true);
                itemChild->setMotion(ChildPointItem::ALL);
                itemChild->setRotationEnable(true);
            }*/
        }
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
        connect(this, SIGNAL(xChanged()), this, SLOT(posChange()), Qt::UniqueConnection);
        connect(this, SIGNAL(yChanged()), this, SLOT(posChange()), Qt::UniqueConnection);
        connect(this, SIGNAL(rotationChanged()), this, SLOT(rotationChange()), Qt::UniqueConnection);
    }
    else
    {
        setFlag(QGraphicsItem::ItemIsMovable, false);
        disconnect(this, SIGNAL(xChanged()), this, SLOT(posChange()));
        disconnect(this, SIGNAL(yChanged()), this, SLOT(posChange()));
        disconnect(this, SIGNAL(rotationChanged()), this, SLOT(rotationChange()));
    }
}

void CharacterItem::setCharacter(Character* character)
{
    if(character == m_character)
        return;

    if(nullptr != m_character)
    {
        disconnect(m_character, nullptr, this, nullptr);
    }
    m_character= character;
    if(m_character)
    {
        connect(m_character, &Character::currentHealthPointsChanged, this, &CharacterItem::updateCharacter);
        connect(m_character, &Character::avatarChanged, this, &CharacterItem::updateCharacter);
        connect(m_character, &Character::avatarChanged, this, &CharacterItem::generatedThumbnail);
    }
}
Character* CharacterItem::getCharacter() const
{
    return m_character;
}

void CharacterItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if((nullptr != m_character) && (event->modifiers() & Qt::AltModifier))
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
        VisualItem::wheelEvent(event);
}

bool CharacterItem::isNpc()
{
    if(nullptr != m_character)
    {
        return m_character->isNpc();
    }
    return false;
}

bool CharacterItem::isPlayableCharacter()
{
    if(nullptr != m_character)
    {
        return !m_character->isNpc();
    }
    return false;
}
void CharacterItem::setTokenFile(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
        QJsonObject obj= doc.object();

        m_diameter= obj["size"].toDouble();
        m_rect.setHeight(m_diameter);
        m_rect.setWidth(m_diameter);
        if(nullptr == m_character)
        {
            auto character= new Character();
            setCharacter(character);
        }
        m_character->readTokenObj(obj);
    }
}
