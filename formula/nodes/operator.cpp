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
#include "operator.h"
namespace Formula
{
OperatorFNode::OperatorFNode()
{

}

ParsingToolFormula::FormulaOperator OperatorFNode::getOperator() const
{
    return m_operator;
}

void OperatorFNode::setOperator(const ParsingToolFormula::FormulaOperator &ope)
{
    m_operator= ope;
}

bool OperatorFNode::run(FormulaNode *previous)
{
    for(auto node : m_parameters)
    {
        if(nullptr!=node)
        {
            node->run(this);
        }
    }

    switch(m_operator)
    {
    case ParsingToolFormula::MIN:
        min();
        break;
    case ParsingToolFormula::MAX:
        max();
        break;
    case ParsingToolFormula::ABS:
        absFunction();
        break;
    case ParsingToolFormula::FLOOR:
        floorFunction();
        break;
    case ParsingToolFormula::CEIL:
        ceilFunction();
        break;
    case ParsingToolFormula::AVG:
        avg();
        break;
    case ParsingToolFormula::CONCAT:
        concatenate();
        break;
    }
    if(nullptr!=m_next)
    {
        m_next->run(this);
    }
    return true;
}

void OperatorFNode::addParameter(FormulaNode *node)
{
    m_parameters.append(node);
}
void OperatorFNode::absFunction()
{
    if(m_parameters.size()>1)
    {
        m_result = QObject::tr("abs : too many arguments");
    }
    if(m_parameters.isEmpty())
    {
        m_result = QObject::tr("abs : too few arguments");
    }
    else
    {
        FormulaNode* node = m_parameters.at(0);
        node->run(this);
        node = getLatestNode(node);
        if(nullptr!=node)
        {
            QVariant var = node->getResult();
            m_result = fabs(var.toDouble());
        }

    }
}
void OperatorFNode::ceilFunction()
{
    if(m_parameters.size()>1)
    {
        m_result = QObject::tr("ceil : too many arguments");
    }
    if(m_parameters.isEmpty())
    {
        m_result = QObject::tr("ceil : too few arguments");
    }
    else
    {
        FormulaNode* node = m_parameters.at(0);
        node = getLatestNode(node);
        if(nullptr!=node)
        {
            QVariant var = node->getResult();
            m_result = ceil(var.toDouble());
        }

    }
}
void OperatorFNode::floorFunction()
{
    if(m_parameters.size()>1)
    {
        m_result = QObject::tr("floor : too many arguments");
    }
    if(m_parameters.isEmpty())
    {
        m_result = QObject::tr("floor : too few arguments");
    }
    else
    {
        FormulaNode* node = m_parameters.at(0);
        node = getLatestNode(node);
        if(nullptr!=node)
        {
            QVariant var = node->getResult();
            m_result = floor(var.toDouble());
        }

    }
}
void OperatorFNode::concatenate()
{
    if(m_parameters.isEmpty())
    {
        m_result = QObject::tr("concatenate : too few arguments");
    }
    else
    {
        QString concat("");
        for(int i = 0; i< m_parameters.size() ; ++i)
        {
            FormulaNode* node = m_parameters.at(i);

            node = getLatestNode(node);
            if(nullptr!=node)
            {
                QVariant var = node->getResult();
                concat += var.toString();
            }

        }
        m_result = concat;

    }
}
void OperatorFNode::min()
{
    QVariant min;
    for(int i = 0; i< m_parameters.size() ; ++i)
    {
        FormulaNode* node = m_parameters.at(i);
        node = getLatestNode(node);
        if(nullptr!=node)
        {
            QVariant var = node->getResult();
            if(0==i)
            {
                min = var;
            }
            else if(var<min)
            {
                min = var;
            }
        }

    }
    m_result = min;
}
void OperatorFNode::max()
{
    QVariant max;
    for(int i = 0; i< m_parameters.size() ; ++i)
    {
        FormulaNode* node = m_parameters.at(i);
        node = getLatestNode(node);
        if(nullptr!=node)
        {
            QVariant var = node->getResult();
            if(0==i)
            {
                max = var;
            }
            else if(var>max)
            {
                max = var;
            }
        }

    }
    m_result = max;
}
void OperatorFNode::avg()
{
    if(m_parameters.isEmpty())
    {
        m_result = QObject::tr("abs : too few arguments");

    }
    else
    {
        QVariant avg(0);
        for(int i = 0; i< m_parameters.size() ; ++i)
        {
            FormulaNode* node = m_parameters.at(i);
            node = getLatestNode(node);
            if(nullptr!=node)
            {
                avg = avg.toDouble() + node->getResult().toDouble();
            }

        }
        m_result = avg.toDouble() / m_parameters.size();
    }
}
QVariant OperatorFNode::getResult()
{
    return m_result;
}

}
