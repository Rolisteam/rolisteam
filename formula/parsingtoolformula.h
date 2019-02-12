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
#ifndef PARSINGTOOLFORMULA_H
#define PARSINGTOOLFORMULA_H

#include <QHash>
#include <QString>

#include "nodes/formulanode.h"
#include "nodes/scalaroperatorfnode.h"
namespace Formula
{
    /**
     * @brief The ParsingToolFormula class provides methods for parsing formula.
     */
    class ParsingToolFormula
    {
    public:
        enum FormulaOperator
        {
            ABS,
            MIN,
            MAX,
            FLOOR,
            CEIL,
            AVG,
            CONCAT
        }; //,IF
        /**
         * @brief ParsingToolFormula
         */
        ParsingToolFormula();
        ~ParsingToolFormula();
        /**
         * @brief readFormula
         * @return
         */
        bool readFormula(QString&, FormulaNode*&);
        /**
         * @brief readScalarOperator
         * @return
         */
        bool readScalarOperator(QString&, FormulaNode*);
        /**
         * @brief readOperand
         * @return
         */
        bool readOperand(QString&, FormulaNode*&);
        /**
         * @brief readOperator
         * @return
         */
        bool readOperator(QString&, FormulaNode*&);
        /**
         * @brief readFieldRef
         * @return
         */
        bool readFieldRef(QString&, FormulaNode*&);
        /**
         * @brief readNumber
         * @return
         */
        bool readNumber(QString&, FormulaNode*&);

        FormulaNode* getLatestNode(FormulaNode* node);

        QHash<QString, QString>* getVariableHash() const;
        void setVariableHash(QHash<QString, QString>* variableHash);

        bool readStringValue(QString& str, FormulaNode*& previous);
        bool readParenthese(QString& str, FormulaNode*& previous);

    private:
        QHash<QString, ParsingToolFormula::FormulaOperator>* m_hashOp;
        QHash<QString, QString>* m_variableHash;

        QHash<QString, ScalarOperatorFNode::ArithmeticOperator>* m_arithmeticOperation;
    };
} // namespace Formula
#endif // PARSINGTOOLFORMULA_H
