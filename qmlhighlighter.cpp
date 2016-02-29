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
    keywordPatterns << "\\bif\\b" << "\\belsif\\b" << "\\belse\\b" << "\\breturn\\b"<< "\\bimport\\b" << "\\bsignal\\b" << "\\bproperty\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    propertyFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("[A-z]+:");
    rule.format = propertyFormat;
    highlightingRules.append(rule);

    lookupFormat.setForeground(Qt::magenta);
    //lookupFormat.setBackground(Qt::black);
    rule.pattern = QRegExp("\\b[0-9]+\\b");
    rule.format = lookupFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegExp("'.*'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    itemFormat.setForeground(QColor(Qt::red));
    //itemFormat.setFontWeight(QFont::Bold);
    rule.pattern =  QRegExp("[A-Z][a-z]+ ");
    rule.format = itemFormat;
    highlightingRules.append(rule);

    cppObjectFormat.setForeground(QColor(Qt::blue).lighter());
    cppObjectFormat.setFontItalic(true);
    rule.pattern =  QRegExp("_[A-Z][A-z|0-9]+");
    rule.format = cppObjectFormat;
    highlightingRules.append(rule);

}

void QmlHighlighter::highlightBlock(const QString &text)
{
    foreach(const HighlightingRule &rule, highlightingRules){
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
}
