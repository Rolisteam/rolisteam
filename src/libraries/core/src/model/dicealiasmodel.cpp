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
#include "model/dicealiasmodel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

#include "diceparser/dicealias.h"

DiceAliasModel::DiceAliasModel(QObject* parent) : QAbstractListModel(parent)
{
    m_header << tr("Pattern") << tr("Value") << tr("Regular Expression") << tr("Disable") << tr("Comments");
}

DiceAliasModel::~DiceAliasModel()
{
    /* if(m_diceAliasList!=nullptr)
     {
         qDeleteAll(*m_diceAliasList);
         delete m_diceAliasList;
         m_diceAliasList = nullptr;
     }*/
}

QVariant DiceAliasModel::data(const QModelIndex& index, int role) const
{

    if(!index.isValid())
        return {};

    auto const& diceAlias= m_diceAliasList.at(index.row());

    if((Qt::DisplayRole == role) || (Qt::EditRole == role))
    {
        if(nullptr != diceAlias)
        {
            if(index.column() == PATTERN)
            {
                return diceAlias->pattern();
            }
            else if(index.column() == COMMAND)
            {
                return diceAlias->command();
            }
            else if(index.column() == METHOD)
            {
                return !diceAlias->isReplace();
            }
            else if(index.column() == DISABLE)
            {
                return !diceAlias->isEnable();
            }
            else if(index.column() == COMMENT)
            {
                return diceAlias->comment();
            }
        }
    }
    else if(Qt::TextAlignmentRole == role)
    {
        if(index.column() == METHOD)
        {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}
int DiceAliasModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_diceAliasList.size());
    return 0;
}
int DiceAliasModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}
QVariant DiceAliasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(orientation == Qt::Horizontal)
        {
            return m_header.at(section);
        }
    }
    return QVariant();
}

/*void DiceAliasModel::setAliases(QList<DiceAlias*>* lst)
{
    m_diceAliasList= lst;
}
void DiceAliasModel::appendAlias()
{
    addAlias(new DiceAlias(tr("New Alias%1").arg(m_diceAliasList->size()), ""));
}*/

/*void DiceAliasModel::preferencesHasChanged(const QString& pref)
{
    if(pref == "isPlayer")
    {
        m_isGM= !PreferencesManager::getInstance()->value(pref, true).toBool();
    }
}*/

void DiceAliasModel::appendAlias(DiceAlias&& alias)
{
    beginInsertRows(QModelIndex(), m_diceAliasList.size(), m_diceAliasList.size());
    m_diceAliasList.push_back(std::make_unique<DiceAlias>(alias));
    endInsertRows();

    auto const& it= m_diceAliasList.end() - 1;
    emit aliasAdded((*it).get(), std::distance(m_diceAliasList.begin(), it));
}
Qt::ItemFlags DiceAliasModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool DiceAliasModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool result= false;
    if(!index.isValid())
        return result;

    auto const& diceAlias= m_diceAliasList[index.row()];
    if(role == Qt::EditRole)
    {
        switch(index.column())
        {
        case PATTERN:
            diceAlias->setPattern(value.toString());
            result= true;
            break;
        case COMMAND:
            diceAlias->setCommand(value.toString());
            result= true;
            break;
        case METHOD:
            if(value.toBool())
            {
                diceAlias->setType(DiceAlias::REGEXP);
            }
            else
            {
                diceAlias->setType(DiceAlias::REPLACE);
            }
            result= true;
            break;
        case DISABLE:
            diceAlias->setEnable(!value.toBool());
            result= true;
            break;
        case COMMENT:
            diceAlias->setComment(value.toString());
            result= true;
            break;
        }
    }

    if(result)
    {
        emit dataChanged(index, index, QVector<int>() << role);
        emit aliasChanged(diceAlias.get(), index.row());
    }

    return result;
}

const std::vector<std::unique_ptr<DiceAlias>>& DiceAliasModel::aliases() const
{
    return m_diceAliasList;
}

void DiceAliasModel::deleteAlias(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    auto const& it= m_diceAliasList.begin() + index.row();
    m_diceAliasList.erase(it);
    endRemoveRows();

    emit aliasRemoved(index.row());

    /*NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::removeDiceAlias);
    msg.int64(index.row());
    msg.sendToServer();*/
}
void DiceAliasModel::upAlias(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() - 1))
    {
        std::iter_swap(m_diceAliasList.begin() + index.row(), m_diceAliasList.begin() + index.row() - 1);
        // m_diceAliasList->swapItemsAt(index.row(), index.row() - 1);
        endMoveRows();
        emit aliasMoved(index.row(), index.row() - 1);
    }
}

void DiceAliasModel::downAlias(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == static_cast<int>(m_diceAliasList.size()) - 1)
        return;

    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() + 2))
    {
        std::iter_swap(m_diceAliasList.begin() + index.row(), m_diceAliasList.begin() + index.row() + 1);
        endMoveRows();
        emit aliasMoved(index.row(), index.row() + 1);
    }
}

void DiceAliasModel::topAlias(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), 0))
    {
        std::iter_swap(m_diceAliasList.begin(), m_diceAliasList.begin() + index.row());
        endMoveRows();
        emit aliasMoved(index.row(), 0);
    }
}

void DiceAliasModel::bottomAlias(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    auto size= static_cast<int>(m_diceAliasList.size());
    if(index.row() == size - 1)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), size))
    {
        std::iter_swap(m_diceAliasList.end() - 1, m_diceAliasList.begin() + index.row());
        endMoveRows();
        emit aliasMoved(index.row(), size);
    }
}

/*void DiceAliasModel::setGM(bool b)
{
    m_isGM= b;
}*/

void DiceAliasModel::clear()
{
    beginResetModel();
    m_diceAliasList.clear();
    endResetModel();
}

/*void DiceAliasModel::load(const QJsonObject& obj)
{
    QJsonArray aliases= obj["aliases"].toArray();

    for(auto aliasRef : aliases)
    {
        auto alias= aliasRef.toObject();
        auto da= new DiceAlias(alias["command"].toString(), alias["value"].toString(), alias["replace"].toBool(),
                               alias["enable"].toBool());
        da->setComment(alias["comment"].toString());
        addAlias(da);
    }
}

void DiceAliasModel::save(QJsonObject& obj)
{
    QJsonArray dices;
    for(auto& dice : *m_diceAliasList)
    {
        QJsonObject diceObj;
        diceObj["command"]= dice->getCommand();
        diceObj["value"]= dice->getValue();
        diceObj["replace"]= dice->isReplace();
        diceObj["enable"]= dice->isEnable();
        diceObj["comment"]= dice->getComment();
        dices.append(diceObj);
    }
    obj["aliases"]= dices;
}
void DiceAliasModel::moveAlias(int from, int to)
{
    if(!m_isGM)
        return;

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::moveDiceAlias);
    msg.int64(from);
    msg.int64(to);
    msg.sendToServer();
}*/
