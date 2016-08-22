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
#ifndef SCALAROPERATORFNODE_H
#define SCALAROPERATORFNODE_H

#include "formulanode.h"
/**
 * @brief The ScalarOperatorFNode class manages basic arithmetic operation.
 */
namespace Formula
{
class ScalarOperatorFNode : public FormulaNode
{
public:
    enum ArithmeticOperator {PLUS,MINUS,DIVIDE,MULTIPLICATION};
    ScalarOperatorFNode();
    virtual ~ScalarOperatorFNode();

    bool run(FormulaNode* previous);

    FormulaNode* getInternalNode() const;
    void setInternalNode(FormulaNode* internalNode);

    ArithmeticOperator getArithmeticOperator() const;
    void setArithmeticOperator(const ArithmeticOperator &arithmeticOperator);

    virtual QVariant getResult();
    int getPriority();
private:
    FormulaNode* m_internalNode;
    ArithmeticOperator m_arithmeticOperator;
    QVariant m_value;
};
}

#endif // SCALAROPERATORFNODE_H
