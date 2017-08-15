/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#include "qmlhighlighter.h"

QmlHighlighter::QmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);

    QStringList keywordPatterns;
    keywordPatterns << "\\sif\\s"
                    << "\\selsif\\s"
                    << "\\selse\\s"
                    << "\\sreturn\\s"
                    << "\\simport\\s"
                    << "import\\s"
                    << "\\ssignal\\s"
                    << "\\sproperty\\s"
                    << "\\sint\\s"
                    << "\\salias\\s"
                    << "\\sstring\\s"
                    << "\\sreal\\s";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    propertyFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\w+:");
    rule.format = propertyFormat;
    highlightingRules.append(rule);

    lookupFormat.setForeground(Qt::magenta);
    //lookupFormat.setBackground(Qt::black);
    rule.pattern = QRegularExpression("\\s\\d+\\s");
    rule.format = lookupFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("'.*'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    itemFormat.setForeground(QColor(Qt::red));
    //itemFormat.setFontWeight(QFont::Bold);
    rule.pattern =  QRegularExpression("[A-Z]\\w+ ");
    rule.format = itemFormat;
    highlightingRules.append(rule);

    cppObjectFormat.setForeground(QColor(Qt::blue).lighter());
    cppObjectFormat.setFontItalic(true);
    rule.pattern =  QRegularExpression("_[A-Z]\\w+");
    rule.format = cppObjectFormat;
    highlightingRules.append(rule);


    commentFormat.setForeground(QColor(Qt::green).lighter());
    commentFormat.setFontItalic(true);
    rule.pattern =  QRegularExpression("\\/\\/.*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

}

void QmlHighlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightingRule &rule, highlightingRules)
    {
        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatch match;// = expression.match(text,0);
        int index = text.indexOf(expression,0,&match);
        while (index >= 0)
        {
            int length = match.capturedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length,&match);
        }

    }
    setCurrentBlockState(0);
}
