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
#include "scalaroperatorfnode.h"

namespace Formula
{
ScalarOperatorFNode::ScalarOperatorFNode()
{

}
ScalarOperatorFNode::~ScalarOperatorFNode()
{

}
QVariant ScalarOperatorFNode::getResult()
{
    return m_value;
}

bool ScalarOperatorFNode::run(FormulaNode* previous)
{

    if(nullptr!=m_internalNode)
    {
            m_internalNode->run(this);
    }
    if(nullptr!=previous)
    {
        QVariant presult = previous->getResult();

        if(!presult.isNull())
        {
             FormulaNode* internal = m_internalNode;
             while(nullptr != internal->next() )
             {
                    internal = internal->next();
             }


            QVariant internalresult = internal->getResult();
            //m_result->setPrevious(internalResult);
            /*if(nullptr!=m_internalNode->getResult())
            {
                m_internalNode->getResult()->setPrevious(previousResult);
            }*/
            qreal result=0;
            switch(m_arithmeticOperator)
            {
                case PLUS:
                    result = presult.toDouble()+internalresult.toDouble();
                    break;
                case MINUS:
                    result = presult.toDouble()-internalresult.toDouble();
                    break;
                case MULTIPLICATION:
                    result = presult.toDouble()*internalresult.toDouble();
                    break;
                case DIVIDE:
                    if(internalresult.toDouble()!=0)
                    {
                        result = presult.toDouble()/internalresult.toDouble();
                    }
                    else
                    {
                        m_value = QVariant(QObject::tr("Error: division by Zero"));
                    }
                    break;
                default:
                    break;

            }
            if(!m_value.isValid())
            {
                m_value = result;
            }
            if(nullptr!=m_next)
            {
                m_next->run(this);
            }
        }
    }
    return true;
}

FormulaNode *ScalarOperatorFNode::getInternalNode() const
{
    return m_internalNode;
}

void ScalarOperatorFNode::setInternalNode(FormulaNode *internalNode)
{
    m_internalNode = internalNode;
}

ScalarOperatorFNode::ArithmeticOperator ScalarOperatorFNode::getArithmeticOperator() const
{
    return m_arithmeticOperator;
}

void ScalarOperatorFNode::setArithmeticOperator(const ArithmeticOperator &arithmeticOperator)
{
    m_arithmeticOperator = arithmeticOperator;
}
int ScalarOperatorFNode::getPriority()
{
    if((m_arithmeticOperator == PLUS)||(m_arithmeticOperator == MINUS))
        return 1;
    else
        return 2;
}
}
