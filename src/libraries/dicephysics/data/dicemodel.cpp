/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "dicemodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QVector3D>

namespace
{
namespace json
{
constexpr auto color{"color"};
constexpr auto rx{"rx"};
constexpr auto rz{"rz"};
constexpr auto px{"px"};
constexpr auto py{"py"};
constexpr auto pz{"pz"};
constexpr auto faces{"faces"};
constexpr auto value{"value"};
constexpr auto unstableTime{"time"};
constexpr auto unstableRot{"rotation"};
constexpr auto unstableDistance{"distance"};

} // namespace json
} // namespace

DiceModel::DiceModel(QObject* parent) : QAbstractListModel(parent)
{
    // connect(this, &DiceModel::animationTimeChanged, this, [this]() { m_timer->setInterval(m_animationTime); });
    connect(&m_timer, &QTimer::timeout, this,
            [this]()
            {
                setIndex(0);
                m_timer.stop();
            });
    m_timer.setInterval(30000);
}

int DiceModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    auto& dices= currentModel();
    return dices.size();
}

QVariant DiceModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto& dices= currentModel();

    auto const& r= dices.at(index.row());
    QVariant var;
    if(role == Color)
        var= r->color();
    else if(role == Ctrl)
        var= QVariant::fromValue(r.get());
    else if(role == Type)
        var= QVariant::fromValue(r->face());
    else if(role == Stable)
        var= r->stable();
    else if(role == Value)
        var= r->value();

    return var; // QVariant::fromValue(r);
}

/*bool DiceModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto const& r= m_dices.at(index.row());
    QVariant var;

    else if(role == Stable)
        var= r->stable();
    else if(role == Value)
        var= r->value();

    return var;
}*/

QHash<int, QByteArray> DiceModel::roleNames() const
{
    return {{Ctrl, "ctrl"}, {Type, "type"}, {Color, "baseCol"}, {Stable, "stable"}, {Value, "value"}};
}

void DiceModel::addDice(DiceController::DiceType type, QColor color)
{
    auto dice= new DiceController(type);
    auto x= QRandomGenerator::global()->bounded(-m_size.width() / 2, m_size.width() / 2);
    auto z= QRandomGenerator::global()->bounded(-m_size.height() / 2, m_size.height() / 2);
    dice->setPosition(QVector3D(x, 10, z));
    dice->setColor(color);
    addDice(dice);
}

void DiceModel::addDice(DiceController* ctrl)
{
    setIndex(0);
    std::unique_ptr<DiceController> p{ctrl};
    auto& dices= currentModel();
    connect(ctrl, &DiceController::selectedChanged, this, &DiceModel::selectionChanged);
    connect(ctrl, &DiceController::stableChanged, this, &DiceModel::checkStable);
    beginInsertRows(QModelIndex(), dices.size(), dices.size());
    dices.push_back(std::move(p));
    endInsertRows();
    emit diceCountChanged();
}

void DiceModel::removeDice(DiceController::DiceType type)
{
    setIndex(0);
    auto& dices= currentModel();
    auto it= std::find_if(std::begin(dices), std::end(dices),
                          [type](const std::unique_ptr<DiceController>& item) { return type == item->face(); });

    if(it == std::end(dices))
        return;

    auto pos= std::distance(std::begin(dices), it);
    beginRemoveRows(QModelIndex(), pos, pos);
    dices.erase(it);
    endRemoveRows();
    emit diceCountChanged();
}

int DiceModel::diceCount(DiceController::DiceType type) const
{
    auto const& dices= currentModel();
    return std::accumulate(std::begin(dices), std::end(dices), 0,
                           [type](int i, const std::unique_ptr<DiceController>& ctrl)
                           { return i + ((ctrl->face() == type) ? 1 : 0); });
}

QList<DiceController*> DiceModel::selection() const
{
    QList<DiceController*> res;
    for(auto const& ctrl : currentModel())
    {
        if(ctrl->selected())
            res.append(ctrl.get());
    }
    return res;
}

void DiceModel::setDiceData(const QByteArray& data)
{
    setIndex(1);

    beginResetModel();
    auto& model= currentModel();
    model.clear();
    fetchModel(data, model, m_size);
    endResetModel();

    m_timer.start();
}

const std::vector<std::unique_ptr<DiceController>>& DiceModel::localModel() const
{
    return m_models.at(0);
}

std::vector<std::unique_ptr<DiceController>>& DiceModel::currentModel() const
{
    return m_models.at(m_index);
}

void DiceModel::checkStable()
{
    QList<DiceController*> rolled;
    for(auto const& ctrl : currentModel())
    {
        if(!ctrl->selected())
            continue;

        if(!ctrl->stable())
            return;

        rolled.append(ctrl.get());
    }
    emit diceRollChanged(diceControllerToData(rolled, m_size));
}

void DiceModel::setIndex(int idx)
{
    if(m_index == idx)
        return;

    beginResetModel();
    m_index= idx;
    endResetModel();
    emit diceCountChanged();
}

QSize DiceModel::size() const
{
    return m_size;
}

void DiceModel::setSize(const QSize& newSize)
{
    if(m_size == newSize)
        return;
    m_size= newSize;
    emit sizeChanged();
}

void DiceModel::fetchModel(const QByteArray& data, std::vector<std::unique_ptr<DiceController>>& model,
                           const QSize& size)
{
    QJsonDocument doc= QJsonDocument::fromJson(data);
    auto array= doc.array();

    for(auto objRef : std::as_const(array))
    {
        auto obj= objRef.toObject();

        auto ctrl= std::make_unique<DiceController>(static_cast<DiceController::DiceType>(obj[json::faces].toInt()));
        QVector3D rot{static_cast<float>(obj[json::rx].toDouble()), 0., static_cast<float>(obj[json::rz].toDouble())};
        QVector3D pos{static_cast<float>(obj[json::px].toDouble()) * (size.width() / 2),
                      static_cast<float>(obj[json::py].toDouble()),
                      static_cast<float>(obj[json::pz].toDouble()) * (size.height() / 2)};

        ctrl->setRotation(rot);
        ctrl->setColor(obj[json::color].toString());
        ctrl->setPosition(pos);

        model.push_back(std::move(ctrl));
    }
}

QHash<int, QList<int>> DiceModel::resultFromBytes(const QByteArray& data, QString& comment)
{
    QJsonDocument doc= QJsonDocument::fromJson(data);
    auto array= doc.array();
    QHash<int, QList<int>> res;

    for(auto ref : std::as_const(array))
    {
        auto obj= ref.toObject();

        auto val= res.value(obj[json::faces].toInt(), QList<int>());
        val.append(obj[json::value].toInt());
        res.insert(obj[json::faces].toInt(), val);

        comment= tr("# 3D Roll : Distance %1 - Time %2 - Rotation %3")
                     .arg(obj[json::unstableDistance].toDouble())
                     .arg(obj[json::unstableTime].toInteger())
                     .arg(obj[json::unstableRot].toDouble());
    }

    return res;
}

QByteArray DiceModel::diceControllerToData(const QList<DiceController*>& list, const QSize& size)
{
    QJsonArray array;
    for(auto ctrl : list)
    {
        QJsonObject obj;

        obj[json::color]= ctrl->color().name();

        auto rot= ctrl->rotation();
        obj[json::rx]= rot.x();
        obj[json::rz]= rot.z();

        auto pos= ctrl->position();
        obj[json::px]= static_cast<double>(pos.x() / (size.width() / 2));
        obj[json::py]= static_cast<double>(pos.y());
        obj[json::pz]= static_cast<double>(pos.z() / (size.height() / 2));

        auto [time, distance, offset]= ctrl->unstableData();
        obj[json::unstableDistance]= static_cast<double>(distance);
        obj[json::unstableTime]= static_cast<qint64>(time);
        obj[json::unstableRot]= static_cast<double>(offset);

        obj[json::faces]= static_cast<int>(ctrl->face());
        obj[json::value]= ctrl->value();

        array.append(obj);
    }

    QJsonDocument doc;
    doc.setArray(array);

    return doc.toJson();
}

int DiceModel::animationTime() const
{
    return m_timer.interval();
}

void DiceModel::setAnimationTime(int newAnimationTime)
{
    if(animationTime() == newAnimationTime)
        return;
    m_timer.setInterval(newAnimationTime);
    emit animationTimeChanged();
}
