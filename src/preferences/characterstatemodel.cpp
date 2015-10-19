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
#include "characterstatemodel.h"
#include "network/networkmessagewriter.h"
#include "preferences/preferencesmanager.h"


#include "data/character.h"
#include <QDebug>

CharacterStateModel::CharacterStateModel()
	: m_isGM(false),m_stateList(new QList<CharacterState*>())
{
	m_header << tr("Label") << tr("Color")<< tr("Image");


}

CharacterStateModel::~CharacterStateModel()
{

}

QVariant CharacterStateModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid())
	{
		CharacterState* state = m_stateList->at(index.row());
        if((Qt::DisplayRole == role)||(Qt::EditRole == role))
		{
			if(NULL!=state)
            {
				if(index.column()==LABEL)
                {
					return state->getLabel();
                }
				else if(index.column()==COLOR)
                {
                    if(Qt::DisplayRole == role)
                        return QVariant();

                    return state->getColor();
                }
				else if(index.column()==PICTURE)
                {
					return state->getImage();
                }

            }
		}
        else if(Qt::BackgroundRole == role)
        {
            if(index.column()==COLOR)
            {
                return state->getColor();
            }
        }
	}
	return QVariant();

}
int CharacterStateModel::rowCount(const QModelIndex &parent) const
{
	return m_stateList->size();
}
int CharacterStateModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}
QVariant CharacterStateModel::headerData(int section, Qt::Orientation orientation, int role) const
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
void CharacterStateModel::setStates(QList<CharacterState*>* lst)
{
	m_stateList = lst;
}
void CharacterStateModel::appendState()
{
	/// @todo Init of State
	addState(new CharacterState());
	//tr("New Alias%1").arg(m_stateList->size()),"")
}
void CharacterStateModel::preferencesHasChanged(QString pref)
{
    if(pref=="isPlayer")
    {
        m_isGM =! PreferencesManager::getInstance()->value(pref,true).toBool();
    }
}

void CharacterStateModel::addState(CharacterState* alias)
{
	beginInsertRows(QModelIndex(),m_stateList->size(),m_stateList->size());
	m_stateList->append(alias);
    endInsertRows();
}
Qt::ItemFlags CharacterStateModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool CharacterStateModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    if(index.isValid())
    {
		CharacterState* state = m_stateList->at(index.row());
        if(role==Qt::EditRole)
        {
            switch (index.column())
            {
			case LABEL:
				state->setLabel(value.toString());
                result = true;
                break;
			case COLOR:
				state->setColor(value.value<QColor>());
                result = true;
                break;
			case PICTURE:
				state->setImage(value.value<QPixmap>());
                result = true;
                break;
            }
        }
        if((result)&&(m_isGM))
        {
            NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::addState);
            msg.int64(index.row());
			msg.string32(state->getLabel());
            //msg.byteArray32(state->getImage());
            msg.rgb(state->getColor());

            msg.sendAll();
        }
    }
    return result;
}
QList<CharacterState*>* CharacterStateModel::getCharacterStates()
{
	return m_stateList;
}
void CharacterStateModel::deleteState(QModelIndex& index)
{
    if(!index.isValid())
        return;
    beginRemoveRows(QModelIndex(),index.row(),index.row());
	m_stateList->removeAt(index.row());
    endRemoveRows();

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::removeDiceAlias);
    msg.int64(index.row());
    msg.sendAll();
}
void CharacterStateModel::upState(QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()-1))
    {
		m_stateList->swap(index.row(),index.row()-1);
		moveState(index.row(),index.row()-1);
        endMoveRows();
    }
}

void CharacterStateModel::downState(QModelIndex& index)
{
    if(!index.isValid())
        return;

	if(index.row()==m_stateList->size()-1)
        return;

    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()+2))
    {
		m_stateList->swap(index.row(),index.row()+1);
		moveState(index.row(),index.row()+1);
        endMoveRows();
    }
}

void CharacterStateModel::topState(QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),0))
    {
		CharacterState* dice = m_stateList->takeAt(index.row());
		moveState(index.row(),0);
		m_stateList->prepend(dice);
        endMoveRows();
    }
}

void CharacterStateModel::bottomState(QModelIndex& index)
{
    if(!index.isValid())
        return;
	if(index.row()==m_stateList->size()-1)
        return;
	if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),m_stateList->size()))
    {
		CharacterState* dice = m_stateList->takeAt(index.row());
		moveState(index.row(),m_stateList->size());
		m_stateList->append(dice);
        endMoveRows();
    }
}
void CharacterStateModel::setGM(bool b)
{
    m_isGM = b;
}
void CharacterStateModel::clear()
{
    beginResetModel();
	qDeleteAll(m_stateList->begin(),m_stateList->end());
	m_stateList->clear();
    endResetModel();
}
void CharacterStateModel::sendOffAllCharacterState(NetworkLink* link)
{
	foreach(CharacterState* alias,*m_stateList)
    {
        NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::addDiceAlias);
		/*msg.int64(m_stateList->indexOf(alias));
        msg.string32(alias->getCommand());
        msg.string32(alias->getValue());
        msg.int8(alias->isReplace());
        msg.int8(alias->isEnable());
		msg.sendTo(link);*/
    }
}
void CharacterStateModel::moveState(int from,int to)
{
    if(m_isGM)
    {
        NetworkMessageWriter msg(NetMsg::SharePreferencesCategory,NetMsg::moveDiceAlias);
        msg.int64(from);
        msg.int64(to);
        msg.sendAll();
    }
}
