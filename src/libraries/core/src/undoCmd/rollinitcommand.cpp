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
#include "undoCmd/rollinitcommand.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "data/character.h"
#include "diceparser_qobject/diceroller.h"
#include "worker/iohelper.h"

RollInitCommand::RollInitCommand(const QList<QPointer<vmap::CharacterItemController>>& data, DiceRoller* dice)
    : m_data(data), m_diceparser(dice)
{
    m_formerValues.reserve(m_data.size());
    std::transform(std::begin(m_data), std::end(m_data), std::back_inserter(m_formerValues),
                   [](const QPointer<vmap::CharacterItemController>& item) -> InitInfo
                   {
                       if(!item)
                           return {false, 0};
                       auto character= item->character();
                       if(character->hasInitScore())
                           return {true, character->getInitiativeScore()};
                       else
                           return {false, 0};
                   });
}

void RollInitCommand::undo()
{
    int i= 0;
    std::for_each(std::begin(m_data), std::end(m_data),
                  [&i, this](const QPointer<vmap::CharacterItemController>& item)
                  {
                      if(!item)
                          return;
                      auto character= item->character();
                      auto info= m_formerValues[i];
                      character->setInitiativeScore(info.m_score);
                      character->setHasInitiative(info.m_hasInit);
                      ++i;
                  });
}

void RollInitCommand::redo()
{
    std::for_each(std::begin(m_data), std::end(m_data),
                  [this](const QPointer<vmap::CharacterItemController>& item)
                  {
                      if(!item || !m_diceparser)
                          return;
                      auto diceparser= m_diceparser->parser();
                      auto character= item->character();
                      if(!character)
                          return;
                      auto cmd= character->initCommand();
                      if(diceparser->parseLine(cmd))
                      {
                          diceparser->start();
                          auto jsonstr= diceparser->resultAsJSon([](const QString& value, const QString&, bool)
                                                                 { return value; });
                          m_diceparser->readErrorAndWarning();
                          auto json= IOHelper::byteArrayToJsonObj(jsonstr.toLocal8Bit());
                          character->setInitiativeScore(static_cast<int>(json["scalar"].toDouble()));
                      }
                  });
}

/////////////////////////////////////////////////
//
// clean up command
//
/////////////////////////////////////////////////
CleanUpRollCommand::CleanUpRollCommand(const QList<QPointer<vmap::CharacterItemController>>& data) : m_data(data)
{
    m_formerValues.reserve(m_data.size());
    std::transform(std::begin(m_data), std::end(m_data), std::back_inserter(m_formerValues),
                   [](const QPointer<vmap::CharacterItemController>& item) -> InitInfo
                   {
                       if(!item)
                           return {false, 0};
                       auto character= item->character();
                       if(character->hasInitScore())
                           return {true, character->getInitiativeScore()};
                       else
                           return {false, 0};
                   });
}

void CleanUpRollCommand::undo()
{
    int i= 0;
    std::for_each(std::begin(m_data), std::end(m_data),
                  [&i, this](const QPointer<vmap::CharacterItemController>& item)
                  {
                      if(!item)
                          return;
                      auto character= item->character();
                      auto info= m_formerValues[i];
                      character->setInitiativeScore(info.m_score);
                      character->setHasInitiative(info.m_hasInit);
                      ++i;
                  });
}

void CleanUpRollCommand::redo()
{
    std::for_each(std::begin(m_data), std::end(m_data),
                  [](const QPointer<vmap::CharacterItemController>& item)
                  {
                      if(!item)
                          return;
                      auto character= item->character();
                      character->setHasInitiative(false);
                  });
}
