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
#ifndef OPERATOR_H
#define OPERATOR_H
#include <QList>

#include "formulanode.h"
#include "../parsingtoolformula.h"
namespace Formula
{
/**
 * @brief The OperatorFNode class manages function in formula such as AVG,floor…
 */
class OperatorFNode : public FormulaNode
{
public:
    OperatorFNode();

    ParsingToolFormula::FormulaOperator getOperator() const;
    void setOperator(const ParsingToolFormula::FormulaOperator &ope);

    virtual bool run(FormulaNode* previous);
    void addParameter(FormulaNode* node);

    virtual QVariant getResult();

    //Implementation of operator.
    void min();
    void max();
    void absFunction();
    void avg();
    void floorFunction();
    void ceilFunction();
    void concatenate();


private:
    ParsingToolFormula::FormulaOperator m_operator;
    QList<FormulaNode*> m_parameters;
    QVariant m_result;
};
}
#endif // OPERATOR_H
