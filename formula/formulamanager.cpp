/***************************************************************************
    *   Copyright (C) 2016 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                                      *
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
#include "formulamanager.h"
namespace Formula
{
FormulaManager::FormulaManager()
    : m_startingNode(NULL)
{
    m_parsingTool = new ParsingToolFormula();
}

QVariant FormulaManager::getValue(QString i)
{
    m_formula = i;
    if(parseLine(i))
    {
        return startComputing();
    }
    return QVariant();
}

bool FormulaManager::parseLine(QString& str)
{
   return readFormula(str);
}

QVariant FormulaManager::startComputing()
{
    m_startingNode->run(NULL);

    FormulaNode* node = m_startingNode;
    while(NULL!=node->next())
    {
        node=node->next();
    }

    QVariant var =node->getResult();
    delete m_startingNode;
    return var;
}

bool FormulaManager::readFormula(QString &str)
{
    m_startingNode = new StartNode();
    FormulaNode* node = NULL;
    bool a = m_parsingTool->readFormula(str,node);

    m_startingNode->setNext(node);
    return a;
}
void FormulaManager::setConstantHash(QHash<QString,QString>* hash)
{
    if(NULL!=m_parsingTool)
    {
        m_parsingTool->setVariableHash(hash);
    }
}
}
