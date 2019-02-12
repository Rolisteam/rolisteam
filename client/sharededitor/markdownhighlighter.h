/***************************************************************************
 * Copyright (C) 2014-2018 by Renaud Guezennec                              *
 * http://www.rolisteam.org/                                                *
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com       *
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

#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QTextDocument;
class QTimer;

class MarkDownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum HighlightingOption
    {
        None= 0,
        FullyHighlightedBlockQuote= 0x01
    };
    Q_DECLARE_FLAGS(HighlightingOptions, HighlightingOption)

    MarkDownHighlighter(
        QTextDocument* parent= nullptr, HighlightingOptions highlightingOptions= HighlightingOption::None);

    // we use some predefined numbers here to be compatible with
    // the peg-markdown parser
    enum HighlighterState
    {
        NoState= -1,
        Link= 0,
        Image= 3,
        CodeBlock,
        Italic= 7,
        Bold,
        List,
        Comment= 11,
        H1,
        H2,
        H3,
        H4,
        H5,
        H6,
        BlockQuote,
        HorizontalRuler= 21,
        Table,
        InlineCodeBlock,
        MaskedSyntax,
        CurrentLineBackgroundColor,
        BrokenLink,

        // internal
        CodeBlockEnd= 100,
        HeadlineEnd
    };

    void setTextFormats(QHash<HighlighterState, QTextCharFormat> formats);
    void setTextFormat(HighlighterState state, QTextCharFormat format);
    void clearDirtyBlocks();
    void setHighlightingOptions(HighlightingOptions options);
    void initHighlightingRules();

signals:
    void highlightingFinished();

protected slots:
    void timerTick();

protected:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        HighlighterState state;
        int capturingGroup;
        int maskedGroup;
        bool useStateAsCurrentBlockState;
        bool disableIfCurrentStateIsSet;
    };

    void highlightBlock(const QString& text) Q_DECL_OVERRIDE;

    void initTextFormats(int defaultFontSize= 12);

    void highlightMarkdown(QString text);

    void highlightHeadline(QString text);

    void highlightAdditionalRules(QVector<HighlightingRule>& rules, QString text);

    void highlightCodeBlock(QString text);

    void highlightCommentBlock(QString text);

    void addDirtyBlock(QTextBlock block);

    void reHighlightDirtyBlocks();
    void setCurrentBlockMargin(HighlighterState state);

private:
    QVector<HighlightingRule> m_highlightingRulesPre;
    QVector<HighlightingRule> m_highlightingRulesAfter;
    QVector<QTextBlock> m_dirtyTextBlocks;
    QHash<HighlighterState, QTextCharFormat> m_formats;
    QTimer* m_timer;
    bool m_highlightingFinished;
    HighlightingOptions m_highlightingOptions;
};
#endif // MARKDOWNHIGHLIGHTER_H
