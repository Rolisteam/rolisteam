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
#include "formulanode.h"
namespace Formula
{
    FormulaNode::FormulaNode() : m_next(0) {}

    FormulaNode::~FormulaNode()
    {
        if(nullptr != m_next)
        {
            delete m_next;
            m_next= nullptr;
        }
    }

    FormulaNode* FormulaNode::next() const { return m_next; }

    void FormulaNode::setNext(FormulaNode* next) { m_next= next; }

    QVariant FormulaNode::getResult() { return QVariant(); }
    int FormulaNode::getPriority() { return 1; }
    FormulaNode* FormulaNode::getLatestNode(FormulaNode* node)
    {
        if(nullptr == node)
        {
            return nullptr;
        }
        while(nullptr != node->next())
        {
            node= node->next();
        }
        return node;
    }

} // namespace Formula
