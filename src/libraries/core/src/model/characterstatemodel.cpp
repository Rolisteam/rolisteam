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
#include "model/characterstatemodel.h"

#include <QBuffer>
#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>

CharacterStateModel::CharacterStateModel(QObject* parent) : QAbstractListModel(parent)
{
    m_header << tr("Label") << tr("Color") << tr("Image");
}

CharacterStateModel::~CharacterStateModel() {}

int CharacterStateModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}
QVariant CharacterStateModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};
    QSet<int> allowedRole{Qt::EditRole, Qt::DisplayRole, Qt::BackgroundRole, LABEL, COLOR, PICTUREPATH, PICTURE, ID};

    if(!allowedRole.contains(role))
        return {};

    QVariant var;
    auto const& state= m_stateList[static_cast<std::size_t>(index.row())];
    auto c= index.column();

    int realRole= role;
    if(role < LABEL)
        realRole= LABEL + c;

    switch(realRole)
    {
    case LABEL:
        var= state->label();
        break;
    case ID:
        var= state->id();
        break;
    case COLOR:
        var= state->color();
        break;
    case PICTURE:
    {
        QPixmap map(state->imagePath());
        var= QVariant::fromValue(map);
    }
    break;
    case PICTUREPATH:
        var= state->imagePath();
        break;
    }
    return var;
}
int CharacterStateModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_stateList.size());
    return 0;
}

QVariant CharacterStateModel::headerData(int section, Qt::Orientation orientation, int role) const
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
int CharacterStateModel::indexFromId(const QString& id) const
{
    auto it= std::find_if(std::begin(m_stateList), std::end(m_stateList),
                          [id](const std::unique_ptr<CharacterState>& state) { return id == state->id(); });

    if(it == std::end(m_stateList))
        return -1;

    return std::distance(std::begin(m_stateList), it);
}
/*void CharacterStateModel::setStates(QList<CharacterState*>* lst)
{
    beginResetModel();
    std::transform(lst->begin(), lst->end(), std::back_inserter(m_stateList),
                   [](CharacterState* state) -> CharacterStateInfo {
                       CharacterStateInfo a= makeStateInfo(state, false);
                       // TODO c++14 list initializer
                       return a;
                   });
    endResetModel();
}*/

/*void CharacterStateModel::preferencesHasChanged(const QString& pref)
{
    if(pref == "isPlayer")
    {

    }
}*/
/*NetWorkReceiver::SendType CharacterStateModel::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;

    if(nullptr == msg)
        return NetWorkReceiver::NONE;

    switch(msg->action())
    {
    case NetMsg::addCharacterState:
        processAddState(msg);
        break;
    case NetMsg::removeCharacterState:
        processRemoveState(msg);
        break;
    case NetMsg::moveCharacterState:
        processMoveState(msg);
        break;
    case NetMsg::CharactereStateModel:
        processModelState(msg);
        break;
    default:
        break;
    }

    return type;
}
*/

void CharacterStateModel::appendState(CharacterState&& stateRef)
{
    auto id= stateRef.id();
    auto isInside= std::find_if(std::begin(m_stateList), std::end(m_stateList),
                                [id](const std::unique_ptr<CharacterState>& state) { return state->id() == id; });

    if(isInside != std::end(m_stateList))
        return;

    auto pos= static_cast<int>(m_stateList.size());
    beginInsertRows(QModelIndex(), pos, pos);
    m_stateList.push_back(std::make_unique<CharacterState>(stateRef));
    endInsertRows();
    auto const& it= m_stateList.end() - 1;
    emit characterStateAdded((*it).get(), pos);
}
Qt::ItemFlags CharacterStateModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool CharacterStateModel::setData(const QModelIndex& idx, const QVariant& value, int role)
{
    qDebug() << "setData states:" << idx << value << role;
    bool result= false;
    if(!idx.isValid())
        return result;

    auto const& state= m_stateList[idx.row()];
    if(role != Qt::EditRole)
        return result;

    QVector<int> roles{LABEL, COLOR, PICTUREPATH, PICTURE, ID};

    switch(roles[idx.column()])
    {
    case LABEL:
        state->setLabel(value.toString());
        result= true;
        break;
    case COLOR:
        state->setColor(value.value<QColor>());
        result= true;
        break;
    case PICTUREPATH:
        state->setImagePath(value.toString());
        result= true;
        break;
    }

    emit dataChanged(index(idx.row(), 0), index(idx.row(), columnCount()),
                     QVector<int>() << role << roles[idx.column()]);
    emit stateChanged();
    return result;
}

const std::vector<std::unique_ptr<CharacterState>>& CharacterStateModel::statesList() const
{
    return m_stateList;
}
void CharacterStateModel::deleteState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    auto const& it= m_stateList.begin() + index.row();
    auto id= (*it)->id();
    m_stateList.erase(it);
    endRemoveRows();

    emit stateRemoved(id);
}
void CharacterStateModel::upState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() - 1))
    {
        std::iter_swap(m_stateList.begin() + index.row(), m_stateList.begin() + index.row() - 1);
        endMoveRows();
        emit stateMoved(index.row(), index.row() - 1);
    }
}

void CharacterStateModel::downState(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == static_cast<int>(m_stateList.size()) - 1)
        return;

    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), index.row() + 2))
    {
        std::iter_swap(m_stateList.begin() + index.row(), m_stateList.begin() + index.row() + 1);
        endMoveRows();
        emit stateMoved(index.row(), index.row() + 1);
    }
}

void CharacterStateModel::topState(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    if(index.row() == 0)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), 0))
    {
        std::iter_swap(m_stateList.begin(), m_stateList.begin() + index.row());
        endMoveRows();
        emit stateMoved(index.row(), 0);
    }
}

void CharacterStateModel::bottomState(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    auto size= static_cast<int>(m_stateList.size());
    if(index.row() == size - 1)
        return;
    if(beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(), size))
    {
        std::iter_swap(m_stateList.end() - 1, m_stateList.begin() + index.row());
        endMoveRows();
        emit stateMoved(index.row(), size);
    }
}

void CharacterStateModel::moveState(int from, int to)
{
    if((from >= 0) && (from < static_cast<int>(m_stateList.size())))
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        std::iter_swap(m_stateList.begin() + to, m_stateList.begin() + from);
        endMoveRows();
    }
}

void CharacterStateModel::removeStateAt(int i)
{
    deleteState(index(i, 0));
}

QString CharacterStateModel::stateLabelFromId(const QString& id)
{
    if(id.isEmpty())
        return {};

    auto it= std::find_if(std::begin(m_stateList), std::end(m_stateList),
                          [id](const std::unique_ptr<CharacterState>& state) { return state->id() == id; });

    if(it == std::end(m_stateList))
        return {};

    return (*it)->label();
}

void CharacterStateModel::clear()
{
    beginResetModel();
    m_stateList.clear();
    endResetModel();
}
/*
void CharacterStateModel::processAddState(NetworkMessageReader* msg)
{
    CharacterState* state= new CharacterState();

    quint64 id= msg->uint64();
    state->setLabel(msg->string32());
    state->setColor(msg->rgb());
    bool hasImage= static_cast<bool>(msg->uint8());
    if(hasImage)
    {
        QByteArray array= msg->byteArray32();
        QPixmap pix;
        pix.loadFromData(array);
        state->setImage(pix);
    }
    m_stateList.push_back(makeStateInfo(state, true));
}
void CharacterStateModel::processMoveState(NetworkMessageReader* msg)
{
    int from= msg->int64();
    int to= msg->int64();

    if((from >= 0) && (from < m_stateList.size()))
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        std::iter_swap(m_stateList.begin() + to, m_stateList.begin() + from);
        endMoveRows();
    }
}
void CharacterStateModel::processRemoveState(NetworkMessageReader* msg)
{
    int pos= static_cast<int>(msg->int64());
    if(m_stateList.size() < pos)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);
    m_stateList.erase(m_stateList.begin() + pos);
    endRemoveRows();
}

void CharacterStateModel::sendOffAllCharacterState()
{
     for(auto& state : *m_stateList)
      {
          NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addState);
          msg.uint64(m_stateList->indexOf(state));
          msg.string32(state->label());
          msg.rgb(state->getColor().rgb());
          if(state->hasImage())
          {
              msg.uint8(static_cast<quint8>(true));

              QByteArray array;
              QBuffer buffer(&array);
              if(!state->getPixmap()->save(&buffer, "PNG"))
              {
                  qWarning("error during encoding png");
              }
              msg.byteArray32(array);
          }
          else
          {
              msg.uint8(static_cast<quint8>(false));
          }
          msg.sendToServer();
      }
        if(result) // isGM
        {
            NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addCharacterState);
            msg.int64(index.row());
            msg.string32(stateInfo.state->label());
            msg.rgb(stateInfo.state->getColor().rgb());
            msg.uint8(stateInfo.state->hasImage());
            if(stateInfo.state->hasImage())
            {
                auto img= stateInfo.state->getImage();
                QByteArray array;
                QBuffer buffer(&array);
                img.save(&buffer, "PNG");
                msg.byteArray32(array);
            }
            msg.sendToServer();
        }
}*/

// void CharacterStateModel::moveState(int from, int to)
//{
// if(m_isGM)
// {
/* NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::moveCharacterState);
 msg.int64(from);
 msg.int64(to);
 msg.sendToServer();*/
// }
//}
