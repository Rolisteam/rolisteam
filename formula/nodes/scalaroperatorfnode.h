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
 * @brief the Formula namespace is gathering all classes to manage the computation formula.
 */
namespace Formula
{
    /**
     * @brief The ScalarOperatorFNode class manages basic arithmetic operation.
     */
    class ScalarOperatorFNode : public FormulaNode
    {
    public:
        /**
         * @brief The ArithmeticOperator enum
         */
        enum ArithmeticOperator
        {
            PLUS,
            MINUS,
            DIVIDE,
            MULTIPLICATION
        };
        /**
         * @brief ScalarOperatorFNode
         */
        ScalarOperatorFNode();
        /**
         * @brief ~ScalarOperatorFNode
         */
        virtual ~ScalarOperatorFNode();
        /**
         * @brief run
         * @param previous
         * @return
         */
        bool run(FormulaNode* previous);
        /**
         * @brief getInternalNode
         * @return
         */
        FormulaNode* getInternalNode() const;
        /**
         * @brief setInternalNode
         * @param internalNode
         */
        void setInternalNode(FormulaNode* internalNode);
        /**
         * @brief getArithmeticOperator
         * @return
         */
        ArithmeticOperator getArithmeticOperator() const;
        /**
         * @brief setArithmeticOperator
         * @param arithmeticOperator
         */
        void setArithmeticOperator(const ArithmeticOperator& arithmeticOperator);
        /**
         * @brief getResult
         * @return
         */
        virtual QVariant getResult();
        /**
         * @brief getPriority
         * @return
         */
        int getPriority();

    private:
        FormulaNode* m_internalNode;
        ArithmeticOperator m_arithmeticOperator;
        QVariant m_value;
    };
} // namespace Formula

#endif // SCALAROPERATORFNODE_H
