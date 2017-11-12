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
#include "markdownhighlighter.h"
#include <QDebug>

MarkDownHighlighter::MarkDownHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    listFormat.setForeground(Qt::darkRed);
    listFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^\\d*\\.");
    rule.format = listFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("^\\* ");
    rule.format = listFormat;
    highlightingRules.append(rule);

    codeFormat.setBackground(Qt::lightGray);
    codeFormat.setFontStyleHint(QFont::Monospace);
    m_codeRule.pattern = QRegularExpression("```");
    m_codeRule.pattern2 = QRegularExpression("```");
    m_codeRule.isMultiline = true;
    m_codeRule.format = codeFormat;
    //highlightingRules.append(rule);



    titleFormat.setForeground(Qt::darkGreen);
    titleFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^#+.*");
    rule.format = titleFormat;
    highlightingRules.append(rule);

    boldFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\*\\*\\*.*\\*\\*\\*");
    rule.format = boldFormat;
    highlightingRules.append(rule);

    lineReturnFormat.setBackground(QColor(Qt::yellow));
    //itemFormat.setFontWeight(QFont::Bold);
    rule.pattern =  QRegularExpression("  $");
    rule.format = lineReturnFormat;
    highlightingRules.append(rule);

    italicFormat.setFontItalic(true);
    rule.pattern =  QRegularExpression("\\*.*\\*");
    rule.format = italicFormat;
    highlightingRules.append(rule);


    linkFormat.setForeground(QColor(Qt::darkBlue));
    linkFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("\\[.*\\]");
    rule.format = linkFormat;
    highlightingRules.append(rule);



    quoteFormat.setBackground(QColor(255,165,0));
    rule.pattern = QRegularExpression("^>.*");
    rule.format = quoteFormat;
    highlightingRules.append(rule);


}

void MarkDownHighlighter::highlightBlock(const QString &text)
{
    for(const HighlightingRule &rule : highlightingRules)
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

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(m_codeRule.pattern);

    while (startIndex >= 0)
    {
            QRegularExpressionMatch endMatch;
            if(previousBlockState() == 0)
            {
                endMatch = m_codeRule.pattern2.match(text,startIndex+m_codeRule.pattern.pattern().size());
            }
            else
            {
                endMatch = m_codeRule.pattern2.match(text,startIndex);
            }

            int endIndex = endMatch.capturedStart();//text.indexOf(m_quoteRule.pattern2, startIndex+m_quoteRule.pattern.pattern().size(), &endMatch);
            int commentLength = 0;
            if (endIndex == -1)
            {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            }
            else
            {
                commentLength = endIndex - startIndex
                        + endMatch.capturedLength();
            }
            setFormat(startIndex, commentLength, m_codeRule.format);
            startIndex = text.indexOf(m_codeRule.pattern,startIndex + commentLength);


    }
        //}



}
