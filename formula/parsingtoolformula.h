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

#include <QString>
#include <QHash>

#include "nodes/formulanode.h"

class ParsingToolFormula
{
public:
    enum FormulaOperator {ABS,MIN,MAX,IF,FLOOR,CEIL,AVG};
    ParsingToolFormula();



    bool readFormula(QString&, FormulaNode*);
    bool readScalarOperator(QString&, FormulaNode*);
    bool readOperand(QString&, FormulaNode* );
    bool readOperator(QString&, FormulaNode*);
    bool readFieldRef(QString&, FormulaNode*);
    bool readNumber(QString&, FormulaNode*);

private:
    QHash<QString,ParsingToolFormula::FormulaOperator>* m_hashOp;
};

#endif // PARSINGTOOLFORMULA_H
