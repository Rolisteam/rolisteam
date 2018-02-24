/***************************************************************************
*	Copyright (C) 2009 by Renaud Guezennec                             *
*   http://www.rolisteam.org/contact                   *
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
#include "dicealiasmodel.h"
#include "network/networkmessagewriter.h"
#include "preferences/preferencesmanager.h"

#include <QDebug>

DiceAliasModel::DiceAliasModel(QObject* parent)
    : QAbstractListModel(parent),m_diceAliasList(new QList<DiceAlias*>()),m_isGM(false)
{
    m_header << tr("Pattern") << tr("Value")<< tr("Regular Expression")<< tr("Disable") << tr("Comments");
}

DiceAliasModel::~DiceAliasModel()
{
   /* if(m_diceAliasList!=nullptr)
    {
        qDeleteAll(*m_diceAliasList);
        delete m_diceAliasList;
        m_diceAliasList = nullptr;
    }*/
    /// @todo QSharedPointer to be used
}

QVariant DiceAliasModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid())
	{
        DiceAlias* diceAlias = m_diceAliasList->at(index.row());
        if((Qt::DisplayRole == role)||(Qt::EditRole == role))
		{
            if(nullptr!=diceAlias)
            {
                if(index.column()==PATTERN)
                {
                    return diceAlias->getCommand();
                }
                else if(index.column()==VALUE)
                {
                    return diceAlias->getValue();
                }
                else if(index.column()==METHOD)
                {
                    return !diceAlias->isReplace();
                }
                else if(index.column()==DISABLE)
                {
                    return !diceAlias->isEnable();
                }
                else if(index.column()==COMMENT)
                {
                    return diceAlias->getComment();
                }
            }
		}
        else if(Qt::TextAlignmentRole== role)
        {
            if(index.column()==METHOD)
            {
                return Qt::AlignCenter;
            }
        }
	}
	return QVariant();

}
int DiceAliasModel::rowCount(const QModelIndex &) const
{
    return m_diceAliasList->size();
}
int DiceAliasModel::columnCount(const QModelIndex &) const
{
    return m_header.size();
}
QVariant DiceAliasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(orientation==Qt::Horizontal)
        {
            return m_header.at(section);
        }
    }
    return QVariant();
}
void DiceAliasModel::setAliases(QList<DiceAlias*>* lst)
{
    m_diceAliasList = lst;
}
void DiceAliasModel::appendAlias()
{
    addAlias(new DiceAlias(tr("New Alias%1").arg(m_diceAliasList->size()),""));
}
void DiceAliasModel::preferencesHasChanged(QString pref)
{
    if(pref=="isPlayer")
    {
        m_isGM =! PreferencesManager::getInstance()->value(pref,true).toBool();
    }
}

void DiceAliasModel::addAlias(DiceAlias* alias)
{
    beginInsertRows(QModelIndex(),m_diceAliasList->size(),m_diceAliasList->size());
    m_diceAliasList->append(alias);
    endInsertRows();
}
Qt::ItemFlags DiceAliasModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool DiceAliasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    if(index.isValid())
    {
        DiceAlias* diceAlias = m_diceAliasList->at(index.row());
        if(role==Qt::EditRole)
        {
            switch (index.column())
            {
            case PATTERN:
                diceAlias->setCommand(value.toString());
                result = true;
                break;
            case VALUE:
                diceAlias->setValue(value.toString());
                result = true;
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
                result = true;
                break;
            case DISABLE:
                diceAlias->setEnable(!value.toBool());
                result = true;
                break;
            case COMMENT:
                diceAlias->setComment(value.toString());
                result = true;
                break;
            }
        }
        if((result)&&(m_isGM))
        {
            NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::addDiceAlias);
            msg.int64(index.row());
            msg.string32(diceAlias->getCommand());
            msg.string32(diceAlias->getValue());
            msg.int8(diceAlias->isReplace());
            msg.int8(diceAlias->isEnable());
            msg.string32(diceAlias->getComment());
            msg.sendAll();
        }
    }
    return result;
}
QList<DiceAlias*>* DiceAliasModel::getAliases()
{
    return m_diceAliasList;
}
void DiceAliasModel::deleteAlias(QModelIndex& index)
{
    if(!index.isValid())
        return;
    beginRemoveRows(QModelIndex(),index.row(),index.row());
    m_diceAliasList->removeAt(index.row());
    endRemoveRows();

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::removeDiceAlias);
    msg.int64(index.row());
    msg.sendAll();
}
void DiceAliasModel::upAlias(QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()-1))
    {
        m_diceAliasList->swap(index.row(),index.row()-1);
        moveAlias(index.row(),index.row()-1);
        endMoveRows();
    }
}

void DiceAliasModel::downAlias(QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row()==m_diceAliasList->size()-1)
        return;

    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()+2))
    {
        m_diceAliasList->swap(index.row(),index.row()+1);
        moveAlias(index.row(),index.row()+1);
        endMoveRows();
    }
}

void DiceAliasModel::topAlias(QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),0))
    {
        DiceAlias* dice = m_diceAliasList->takeAt(index.row());
        moveAlias(index.row(),0);
        m_diceAliasList->prepend(dice);
        endMoveRows();
    }
}

void DiceAliasModel::bottomAlias(QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row()==m_diceAliasList->size()-1)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),m_diceAliasList->size()))
    {
        DiceAlias* dice = m_diceAliasList->takeAt(index.row());
        moveAlias(index.row(),m_diceAliasList->size());
        m_diceAliasList->append(dice);
        endMoveRows();
    }
}
void DiceAliasModel::setGM(bool b)
{
    m_isGM = b;
}
void DiceAliasModel::clear()
{
    beginResetModel();
    qDeleteAll(m_diceAliasList->begin(),m_diceAliasList->end());
    m_diceAliasList->clear();
    endResetModel();
}
void DiceAliasModel::sendOffAllDiceAlias()
{
    for(DiceAlias* alias : *m_diceAliasList)
    {
        NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::addDiceAlias);
        msg.int64(m_diceAliasList->indexOf(alias));
        msg.string32(alias->getCommand());
        msg.string32(alias->getValue());
        msg.int8(alias->isReplace());
        msg.int8(alias->isEnable());
        msg.string32(alias->getComment());
        msg.sendAll();
    }
}
void DiceAliasModel::moveAlias(int from,int to)
{
    if(m_isGM)
    {
        NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::moveDiceAlias);
        msg.int64(from);
        msg.int64(to);
        msg.sendAll();
    }
}
