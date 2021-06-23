/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "campaign.h"

#include "data/media.h"
#include "data/rolisteamtheme.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/nonplayablecharactermodel.h"

namespace campaign
{

Campaign::Campaign(QObject* parent)
    : QObject(parent)
    , m_diceModel(new DiceAliasModel)
    , m_stateModel(new CharacterStateModel)
    , m_npcModel(new campaign::NonPlayableCharacterModel)
{
}

QString Campaign::name() const
{
    return m_name;
}

Campaign::~Campaign()= default;

QString Campaign::rootDirectory() const
{
    return m_root;
}

QString Campaign::currentChapter() const
{
    return m_currentChapter;
}

DiceAliasModel* Campaign::diceAliases() const
{
    return m_diceModel.get();
}

NonPlayableCharacterModel* Campaign::npcModel() const
{
    return m_npcModel.get();
}

CharacterStateModel* Campaign::stateModel() const
{
    return m_stateModel.get();
}

RolisteamTheme* Campaign::currentTheme() const
{
    return m_theme.get();
}

Campaign::State Campaign::state() const
{
    return m_state;
}

QString Campaign::currentStorePath() const
{
    QStringList list{m_root, m_currentChapter};
    return list.join("/");
}

void Campaign::addMedia(std::unique_ptr<Media> media)
{
    if(nullptr != mediaFromPath(media->path()))
        return;

    auto p= media.get();
    m_mediaList.push_back(std::move(media));
    emit mediaAdded(p);
}

void Campaign::removeMedia(const QString& id)
{
    auto it= std::find_if(std::begin(m_mediaList), std::end(m_mediaList),
                          [id](const std::unique_ptr<campaign::Media>& p) { return id == p->id(); });
    if(it != std::end(m_mediaList))
    {
        m_mediaList.erase(it);
        emit mediaRemoved(id);
    }
}

const std::vector<std::unique_ptr<Media>>& Campaign::medias() const
{
    return m_mediaList;
}

Media* Campaign::mediaFromPath(const QString& path) const
{
    auto it= std::find_if(std::begin(m_mediaList), std::end(m_mediaList),
                          [path](const std::unique_ptr<campaign::Media>& p) { return path == p->path(); });

    return (it != std::end(m_mediaList)) ? (*it).get() : nullptr;
}

Media* Campaign::mediaFromUuid(const QString& uuid) const
{
    auto it= std::find_if(std::begin(m_mediaList), std::end(m_mediaList),
                          [uuid](const std::unique_ptr<campaign::Media>& p) { return uuid == p->id(); });
    return (it != std::end(m_mediaList)) ? (*it).get() : nullptr;
}

QString Campaign::directory(Place place) const
{
    QString res;
    switch(place)
    {
    case Place::MEDIA_ROOT:
        res= QString("%1/%2").arg(rootDirectory(), MEDIA_ROOT);
        break;
    case Place::STATE_ROOT:
        res= QString("%1/%2").arg(rootDirectory(), STATE_ROOT);
        break;
    case Place::TRASH_ROOT:
        res= QString("%1/%2").arg(rootDirectory(), TRASH_FOLDER);
        break;
    case Place::NPC_ROOT:
        res= QString("%1/%2").arg(rootDirectory(), CHARACTER_ROOT);
        break;
    }
    return res;
}

void Campaign::setRootDirectory(const QString& root)
{
    if(m_root == root)
        return;
    m_root= root;
    emit rootDirectoryChanged();
}

void Campaign::setCurrentChapter(const QString& chapter)
{
    if(m_currentChapter == chapter)
        return;
    m_currentChapter= chapter;
    emit currentChapterChanged();
}

void Campaign::setCurrentTheme(RolisteamTheme* themeuri)
{
    if(themeuri == m_theme.get())
        return;
    m_theme.reset(themeuri);
    emit currentThemeChanged();
}

void Campaign::setState(Campaign::State state)
{
    if(state == m_state)
        return;
    m_state= state;
    emit stateChanged();
}

void Campaign::setName(const QString& name)
{
    if(m_name == name)
        return;
    m_name= name;
    emit nameChanged();
}

void Campaign::postError(const QString& msg)
{
    emit errorOccured(msg);
}

void Campaign::addAlias()
{
    m_diceModel->appendAlias(DiceAlias{tr("Pattern"), tr("Command"), tr("Comment")});
}

void Campaign::deleteAlias(const QModelIndex& index)
{
    m_diceModel->deleteAlias(index);
}

QString Campaign::convertAlias(const QString& str)
{
    /*m_diceParser->constAliases();
return m_diceParser->convertAlias(str);*/
}

void Campaign::addState()
{
    m_stateModel->appendState(CharacterState());
}

void Campaign::deleteState(const QModelIndex& index)
{
    m_stateModel->deleteState(index);
}

void Campaign::moveState(const QModelIndex& index, campaign::Campaign::Move move)
{
    switch(move)
    {
    case campaign::Campaign::Move::UP:
        m_stateModel->upState(index);
        break;
    case campaign::Campaign::Move::DOWN:
        m_stateModel->downState(index);
        break;
    case campaign::Campaign::Move::TOP:
        m_stateModel->topState(index);
        break;
    case campaign::Campaign::Move::BOTTOM:
        m_stateModel->bottomState(index);
        break;
    }
}

void Campaign::moveAlias(const QModelIndex& index, Move move)
{
    switch(move)
    {
    case campaign::Campaign::Move::UP:
        m_diceModel->upAlias(index);
        break;
    case campaign::Campaign::Move::DOWN:
        m_diceModel->downAlias(index);
        break;
    case campaign::Campaign::Move::TOP:
        m_diceModel->topAlias(index);
        break;
    case campaign::Campaign::Move::BOTTOM:
        m_diceModel->bottomAlias(index);
        break;
    }
}

void Campaign::addCharacter()
{
    // TODO command
    m_npcModel->append();
}

void Campaign::removeCharacter(const QString& id)
{
    // TODO command
    m_npcModel->removeNpc(id);
}
} // namespace campaign
