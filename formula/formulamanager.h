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
#ifndef FORMULAMANAGER_H
#define FORMULAMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
/**
 * @page FormulaManager Formula Manager
 *
 * @section Intro Introduction
 * FormulaManager is the software component dedicated to compute charactersheet formula in rolisteam.<br/>
 *
 * @section grammar The Grammar
 *
 * The grammar looks like this:
 *
 * Formula =: Operand | ScalarOperator Operand<br/>
 * Operand =: number | Field Ref | operator <br/>
 * Field Ref =: ${[A-z[A-z|0-9]+}<br/>
 * Operator =: [abs | min | max | floor | ceil | avg](Formula[,Formula]*)
 * ScalarOperator =: [x,-,*,/]<br/>
 * number =: [0-9]+<br/>
 * Word =: [A-z]+<br/>
 *
 */

#include "parsingtoolformula.h"
#include "nodes/startnode.h"
/**
 * Formala namespace is gathering all classes required for formula management.
 */
namespace Formula
{
/**
 * @brief The FormulaManager class
 */
class FormulaManager
{
public:
    FormulaManager();

    QVariant getValue(QString i);
protected:
    bool parseLine(QString& str);
    QVariant startComputing();


    bool readFormula(QString& str);


private:
    QString m_formula;
    ParsingToolFormula* m_parsingTool;
    StartNode* m_startingNode;

};
}
#endif // FORMULAMANAGER_H
