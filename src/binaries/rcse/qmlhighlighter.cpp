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

#include <QTextStream>

QmlHighlighter::QmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkYellow);
    rule.format = keywordFormat;
    loadDictionary(":/resources/dictionaries/keywords.txt",rule);

    keywordFormat.setForeground(Qt::blue);
    rule.format = keywordFormat;
    loadDictionary(":/resources/dictionaries/javascript.txt",rule);

    propertyFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("\\s*[0-9a-zA-Z_\\.]*\\s*:");
    rule.format = propertyFormat;
    m_highlightingRules.append(rule);

    itemFormat.setForeground(QColor(Qt::red));
    //itemFormat.setFontWeight(QFont::Medium);
    rule.pattern =  QRegularExpression("^\\s*[A-Z]\\w+");
    rule.format = itemFormat;
    m_highlightingRules.append(rule);

    commentFormat.setForeground(QColor(Qt::darkBlue).lighter());
    commentFormat.setFontItalic(true);
    rule.pattern =  QRegularExpression("\\/\\/.*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);

    cppObjectFormat.setForeground(QColor(Qt::blue).lighter());
    cppObjectFormat.setFontItalic(true);
    rule.pattern =  QRegularExpression("_[A-Za-z]\\w+");
    rule.format = cppObjectFormat;
    m_highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("['\"].*['\"]");
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);

    lookupFormat.setForeground(Qt::magenta);
    rule.pattern = QRegularExpression("\\s\\d+\\s");
    rule.format = lookupFormat;
    m_highlightingRules.append(rule);

}

void QmlHighlighter::highlightBlock(const QString &text)
{
    for(const HighlightingRule &rule : m_highlightingRules)
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

void QmlHighlighter::loadDictionary(QString filepath, HighlightingRule& rule)
{
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            rule.pattern = QRegularExpression(QStringLiteral("%1\\s").arg(textStream.readLine().trimmed()));
            m_highlightingRules.append(rule);
        }
    }
}
