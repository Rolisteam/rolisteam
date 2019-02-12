/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com       *
 *                                                                          *
 *  This file is part of rolisteam                                          *
 *                                                                          *
 * rolisteam is free software; you can redistribute it and/or modify        *
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
#include <QTimer>

MarkDownHighlighter::MarkDownHighlighter(QTextDocument* parent, HighlightingOptions highlightingOptions)
    : QSyntaxHighlighter(parent)
{
    m_highlightingOptions= highlightingOptions;
    m_timer= new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MarkDownHighlighter::timerTick);
    m_timer->start(1000);

    // initialize the highlighting rules
    initHighlightingRules();

    // initialize the text formats
    initTextFormats();
}

void MarkDownHighlighter::timerTick()
{
    // re-highlight all dirty blocks
    reHighlightDirtyBlocks();

    // emit a signal every second if there was some highlighting done
    if(m_highlightingFinished)
    {
        m_highlightingFinished= false;
        emit highlightingFinished();
    }
}

void MarkDownHighlighter::reHighlightDirtyBlocks()
{
    while(m_dirtyTextBlocks.count() > 0)
    {
        QTextBlock block= m_dirtyTextBlocks.at(0);
        rehighlightBlock(block);
        m_dirtyTextBlocks.removeFirst();
    }
}

void MarkDownHighlighter::clearDirtyBlocks()
{
    m_dirtyTextBlocks.clear();
}

void MarkDownHighlighter::addDirtyBlock(QTextBlock block)
{
    if(!m_dirtyTextBlocks.contains(block))
    {
        m_dirtyTextBlocks.append(block);
    }
}
void MarkDownHighlighter::initHighlightingRules()
{
    HighlightingRule rule;

    // highlight the reference of reference links
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("^\\[.+?\\]: \\w+://.+$");
    rule.state= HighlighterState::MaskedSyntax;
    m_highlightingRulesPre.append(rule);

    // highlight unordered lists
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("^\\s*[-*+]\\s");
    rule.state= HighlighterState::List;
    rule.useStateAsCurrentBlockState= true;
    m_highlightingRulesPre.append(rule);

    // highlight ordered lists
    rule.pattern= QRegularExpression("^\\s*\\d+\\.\\s");
    m_highlightingRulesPre.append(rule);

    // highlight block quotes
    rule= HighlightingRule();
    rule.pattern= QRegularExpression(m_highlightingOptions.testFlag(HighlightingOption::FullyHighlightedBlockQuote) ?
                                         "^\\s*(>\\s*.+)" :
                                         "^\\s*(>\\s*)+");
    rule.state= HighlighterState::BlockQuote;
    m_highlightingRulesPre.append(rule);

    // highlight horizontal rulers
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("^([*\\-_]\\s?){3,}$");
    rule.state= HighlighterState::HorizontalRuler;
    m_highlightingRulesPre.append(rule);

    // highlight tables without starting |
    // we drop that for now, it's far too messy to deal with
    //    rule = HighlightingRule();
    //    rule.pattern = QRegularExpression("^.+? \\| .+? \\| .+$");
    //    rule.state = HighlighterState::Table;
    //    _highlightingRulesPre.append(rule);

    /*
     * highlight italic
     * this goes before bold so that bold can overwrite italic
     *
     * text to test:
     * **bold** normal **bold**
     * *start of line* normal
     * normal *end of line*
     * * list item *italic*
     */
    rule= HighlightingRule();
    // we don't allow a space after the starting * to prevent problems with
    // unordered lists starting with a *
    rule.pattern= QRegularExpression("(^|[^\\*\\b])(\\*([^\\* ][^\\*]*?)\\*)([^\\*\\b]|$)");
    rule.state= HighlighterState::Italic;
    rule.maskedGroup= 2;
    rule.capturingGroup= 3;
    m_highlightingRulesAfter.append(rule);

    rule.maskedGroup= 0;
    rule.capturingGroup= 1;
    rule.pattern= QRegularExpression("\\b_([^_]+)_\\b");
    m_highlightingRulesAfter.append(rule);

    // highlight bold
    rule.pattern= QRegularExpression("\\B\\*{2}(.+?)\\*{2}\\B");
    rule.state= HighlighterState::Bold;
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);
    rule.pattern= QRegularExpression("\\b__(.+?)__\\b");
    m_highlightingRulesAfter.append(rule);

    // highlight urls
    rule= HighlightingRule();
    rule.state= HighlighterState::Link;

    // highlight urls without any other markup
    rule.pattern= QRegularExpression("\\b\\w+?:\\/\\/[^\\s]+");
    rule.capturingGroup= 0;
    m_highlightingRulesAfter.append(rule);

    //    rule.pattern = QRegularExpression("<(.+?:\\/\\/.+?)>");
    rule.pattern= QRegularExpression("<([^\\s`][^`]*?[^\\s`])>");
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);

    // highlight urls with title
    rule.pattern= QRegularExpression("\\[([^\\[\\]]+)\\]\\((\\S+|.+?)\\)\\B");
    m_highlightingRulesAfter.append(rule);

    // highlight urls with empty title
    rule.pattern= QRegularExpression("\\[\\]\\((.+?)\\)");
    m_highlightingRulesAfter.append(rule);

    // highlight email links
    rule.pattern= QRegularExpression("<(.+?@.+?)>");
    m_highlightingRulesAfter.append(rule);

    // highlight reference links
    rule.pattern= QRegularExpression("\\[(.+?)\\]\\s?\\[.+?\\]");
    m_highlightingRulesAfter.append(rule);

    // highlight images with text
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("!\\[(.+?)\\]\\(.+?\\)");
    rule.state= HighlighterState::Image;
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);

    // highlight images without text
    rule.pattern= QRegularExpression("!\\[\\]\\((.+?)\\)");
    m_highlightingRulesAfter.append(rule);

    // highlight images links
    rule= HighlightingRule();
    rule.state= HighlighterState::Link;
    rule.pattern= QRegularExpression("\\[!\\[(.+?)\\]\\(.+?\\)\\]\\(.+?\\)");
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);

    // highlight images links without text
    rule.pattern= QRegularExpression("\\[!\\[\\]\\(.+?\\)\\]\\((.+?)\\)");
    m_highlightingRulesAfter.append(rule);

    // highlight inline code
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("`(.+?)`");
    rule.state= HighlighterState::InlineCodeBlock;
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);

    // highlight code blocks with four spaces or tabs in front of them
    // and no list character after that
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("^((\\t)|( {4,})).+$");
    rule.state= HighlighterState::CodeBlock;
    rule.disableIfCurrentStateIsSet= true;
    m_highlightingRulesAfter.append(rule);

    // highlight inline comments
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("<!\\-\\-(.+?)\\-\\->");
    rule.state= HighlighterState::Comment;
    rule.capturingGroup= 1;
    m_highlightingRulesAfter.append(rule);

    // highlight comments for Rmarkdown for academic papers
    rule.pattern= QRegularExpression("^\\[.+?\\]: # \\(.+?\\)$");
    m_highlightingRulesAfter.append(rule);

    // highlight tables with starting |
    rule= HighlightingRule();
    rule.pattern= QRegularExpression("^\\|.+?\\|$");
    rule.state= HighlighterState::Table;
    m_highlightingRulesAfter.append(rule);
}

void MarkDownHighlighter::initTextFormats(int defaultFontSize)
{
    QTextCharFormat format;

    // set character formats for headlines
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(0, 49, 110)));
    format.setBackground(QBrush(QColor(230, 230, 240)));
    format.setFontWeight(QFont::Bold);
    format.setFontPointSize(defaultFontSize * 1.6);
    m_formats[H1]= format;
    format.setFontPointSize(defaultFontSize * 1.5);
    m_formats[H2]= format;
    format.setFontPointSize(defaultFontSize * 1.4);
    m_formats[H3]= format;
    format.setFontPointSize(defaultFontSize * 1.3);
    m_formats[H4]= format;
    format.setFontPointSize(defaultFontSize * 1.2);
    m_formats[H5]= format;
    format.setFontPointSize(defaultFontSize * 1.1);
    m_formats[H6]= format;
    format.setFontPointSize(defaultFontSize);

    // set character format for horizontal rulers
    format= QTextCharFormat();
    format.setForeground(QBrush(Qt::darkGray));
    format.setBackground(QBrush(Qt::lightGray));
    m_formats[HorizontalRuler]= format;

    // set character format for lists
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(163, 0, 123)));
    m_formats[List]= format;

    // set character format for links
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(255, 128, 0)));
    format.setBackground(QBrush(QColor(255, 233, 211)));
    m_formats[Link]= format;

    // set character format for images
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(0, 191, 0)));
    format.setBackground(QBrush(QColor(228, 255, 228)));
    m_formats[Image]= format;

    // set character format for code blocks
    format= QTextCharFormat();
    format.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    format.setForeground(QBrush(Qt::darkGreen));
    format.setBackground(QBrush(QColor(217, 231, 217)));
    m_formats[CodeBlock]= format;
    m_formats[InlineCodeBlock]= format;

    // set character format for italic
    format= QTextCharFormat();
    format.setFontWeight(QFont::StyleItalic);
    format.setForeground(QBrush(QColor(0, 87, 174)));
    m_formats[Italic]= format;

    // set character format for bold
    format= QTextCharFormat();
    format.setFontWeight(QFont::Bold);
    format.setForeground(QBrush(QColor(0, 66, 138)));
    m_formats[Bold]= format;

    // set character format for comments
    format= QTextCharFormat();
    format.setForeground(QBrush(Qt::gray));
    m_formats[Comment]= format;

    // set character format for masked syntax
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(204, 204, 204)));
    m_formats[MaskedSyntax]= format;

    // set character format for tables
    format= QTextCharFormat();
    format.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    format.setForeground(QBrush(QColor(100, 148, 73)));
    m_formats[Table]= format;

    // set character format for block quotes
    format= QTextCharFormat();
    format.setForeground(QBrush(QColor(Qt::darkRed)));
    m_formats[BlockQuote]= format;
}

void MarkDownHighlighter::setTextFormats(QHash<HighlighterState, QTextCharFormat> formats)
{
    m_formats= formats;
}

void MarkDownHighlighter::setTextFormat(HighlighterState state, QTextCharFormat format)
{
    m_formats[state]= format;
}

void MarkDownHighlighter::highlightBlock(const QString& text)
{
    setCurrentBlockState(HighlighterState::NoState);
    currentBlock().setUserState(HighlighterState::NoState);
    highlightMarkdown(text);
    m_highlightingFinished= true;
}

void MarkDownHighlighter::highlightMarkdown(QString text)
{
    if(!text.isEmpty())
    {
        highlightAdditionalRules(m_highlightingRulesPre, text);

        // needs to be called after the horizontal ruler highlighting
        highlightHeadline(text);

        highlightAdditionalRules(m_highlightingRulesAfter, text);
    }

    highlightCommentBlock(text);
    highlightCodeBlock(text);
}

void MarkDownHighlighter::highlightHeadline(QString text)
{
    QRegularExpression regex("^(#+)\\s+(.+?)$");
    QRegularExpressionMatch match= regex.match(text);
    QTextCharFormat& maskedFormat= m_formats[HighlighterState::MaskedSyntax];

    // check for headline blocks with # in front of them
    if(match.hasMatch())
    {
        int count= match.captured(1).count();

        // we just have H1 to H6
        count= qMin(count, 6);

        HighlighterState state= HighlighterState(HighlighterState::H1 + count - 1);

        QTextCharFormat& format= m_formats[state];
        QTextCharFormat& currentMaskedFormat= maskedFormat;

        // set the font size from the current rule's font format
        maskedFormat.setFontPointSize(format.fontPointSize());

        // first highlight everything as MaskedSyntax
        setFormat(match.capturedStart(), match.capturedLength(), currentMaskedFormat);

        // then highlight with the real format
        setFormat(match.capturedStart(2), match.capturedLength(2), m_formats[state]);

        // set a margin for the current block
        setCurrentBlockMargin(state);

        setCurrentBlockState(state);
        currentBlock().setUserState(state);
        return;
    }

    // take care of ==== and ---- headlines
    QRegularExpression patternH1= QRegularExpression("^=+$");
    QRegularExpression patternH2= QRegularExpression("^-+$");
    QTextBlock previousBlock= currentBlock().previous();
    QString previousText= previousBlock.text();
    previousText.trimmed().remove(QRegularExpression("[=-]"));

    // check for ===== after a headline text and highlight as H1
    if(patternH1.match(text).hasMatch())
    {
        if(((previousBlockState() == HighlighterState::H1) || (previousBlockState() == HighlighterState::NoState))
            && (previousText.length() > 0))
        {
            // set the font size from the current rule's font format
            maskedFormat.setFontPointSize(m_formats[HighlighterState::H1].fontPointSize());

            setFormat(0, text.length(), maskedFormat);
            setCurrentBlockState(HighlighterState::HeadlineEnd);
            previousBlock.setUserState(HighlighterState::H1);

            // set a margin for the current block
            setCurrentBlockMargin(HighlighterState::H1);

            // we want to re-highlight the previous block
            // this must not done directly, but with a queue, otherwise it
            // will crash
            // setting the character format of the previous text, because this
            // causes text to be formatted the same way when writing after
            // the text
            addDirtyBlock(previousBlock);
        }

        return;
    }

    // check for ----- after a headline text and highlight as H2
    if(patternH2.match(text).hasMatch())
    {
        if(((previousBlockState() == HighlighterState::H2) || (previousBlockState() == HighlighterState::NoState))
            && (previousText.length() > 0))
        {
            // set the font size from the current rule's font format
            maskedFormat.setFontPointSize(m_formats[HighlighterState::H2].fontPointSize());

            setFormat(0, text.length(), maskedFormat);
            setCurrentBlockState(HighlighterState::HeadlineEnd);
            previousBlock.setUserState(HighlighterState::H2);

            // set a margin for the current block
            setCurrentBlockMargin(HighlighterState::H2);

            // we want to re-highlight the previous block
            addDirtyBlock(previousBlock);
        }

        return;
    }

    QTextBlock nextBlock= currentBlock().next();
    QString nextBlockText= nextBlock.text();

    // highlight as H1 if next block is =====
    if(patternH1.match(nextBlockText).hasMatch() || patternH2.match(nextBlockText).hasMatch())
    {
        setFormat(0, text.length(), m_formats[HighlighterState::H1]);
        setCurrentBlockState(HighlighterState::H1);
        currentBlock().setUserState(HighlighterState::H1);
    }

    // highlight as H2 if next block is -----
    if(patternH2.match(nextBlockText).hasMatch())
    {
        setFormat(0, text.length(), m_formats[HighlighterState::H2]);
        setCurrentBlockState(HighlighterState::H2);
        currentBlock().setUserState(HighlighterState::H2);
    }
}

void MarkDownHighlighter::setCurrentBlockMargin(MarkDownHighlighter::HighlighterState state)
{
    // this is currently disabled because it causes multiple problems:
    // - it prevents "undo" in headlines
    //   https://github.com/pbek/QOwnNotes/issues/520
    // - invisible lines at the end of a note
    //   https://github.com/pbek/QOwnNotes/issues/667
    // - a crash when reaching the invisible lines when the current line is
    //   highlighted
    //   https://github.com/pbek/QOwnNotes/issues/701
    return;

    qreal margin;

    switch(state)
    {
    case HighlighterState::H1:
        margin= 5;
        break;
    case HighlighterState::H2:
    case HighlighterState::H3:
    case HighlighterState::H4:
    case HighlighterState::H5:
    case HighlighterState::H6:
        margin= 3;
        break;
    default:
        return;
    }

    QTextBlockFormat blockFormat= currentBlock().blockFormat();
    blockFormat.setTopMargin(2);
    blockFormat.setBottomMargin(margin);

    // this prevents "undo" in headlines!
    QTextCursor* myCursor= new QTextCursor(currentBlock());
    myCursor->setBlockFormat(blockFormat);
}

void MarkDownHighlighter::highlightCodeBlock(QString text)
{
    QRegularExpression regex("^```\\w*?$");
    QRegularExpressionMatch match= regex.match(text);

    if(match.hasMatch())
    {
        setCurrentBlockState(previousBlockState() == HighlighterState::CodeBlock ? HighlighterState::CodeBlockEnd :
                                                                                   HighlighterState::CodeBlock);
        // set the font size from the current rule's font format
        QTextCharFormat& maskedFormat= m_formats[HighlighterState::MaskedSyntax];
        maskedFormat.setFontPointSize(m_formats[HighlighterState::CodeBlock].fontPointSize());

        setFormat(0, text.length(), maskedFormat);
    }
    else if(previousBlockState() == HighlighterState::CodeBlock)
    {
        setCurrentBlockState(HighlighterState::CodeBlock);
        setFormat(0, text.length(), m_formats[HighlighterState::CodeBlock]);
    }
}

void MarkDownHighlighter::highlightCommentBlock(QString text)
{
    bool highlight= false;
    text= text.trimmed();
    QString startText= "<!--";
    QString endText= "-->";

    // we will skip this case because that is an inline comment and causes
    // troubles here
    if(text.startsWith(startText) && text.contains(endText))
    {
        return;
    }

    if(text.startsWith(startText) || (!text.endsWith(endText) && (previousBlockState() == HighlighterState::Comment)))
    {
        setCurrentBlockState(HighlighterState::Comment);
        highlight= true;
    }
    else if(text.endsWith(endText))
    {
        highlight= true;
    }

    if(highlight)
    {
        setFormat(0, text.length(), m_formats[HighlighterState::Comment]);
    }
}

void MarkDownHighlighter::highlightAdditionalRules(QVector<HighlightingRule>& rules, QString text)
{
    QTextCharFormat& maskedFormat= m_formats[HighlighterState::MaskedSyntax];

    for(const auto& rule : rules)
    {
        // continue if an other current block state was already set if
        // disableIfCurrentStateIsSet is set
        if(rule.disableIfCurrentStateIsSet && (currentBlockState() != HighlighterState::NoState))
        {
            continue;
        }

        QRegularExpression expression(rule.pattern);
        QRegularExpressionMatchIterator iterator= expression.globalMatch(text);
        int capturingGroup= rule.capturingGroup;
        int maskedGroup= rule.maskedGroup;
        QTextCharFormat& format= m_formats[rule.state];

        // store the current block state if useStateAsCurrentBlockState
        // is set
        if(iterator.hasNext() && rule.useStateAsCurrentBlockState)
        {
            setCurrentBlockState(rule.state);
        }

        // find and format all occurrences
        while(iterator.hasNext())
        {
            QRegularExpressionMatch match= iterator.next();

            // if there is a capturingGroup set then first highlight
            // everything as MaskedSyntax and highlight capturingGroup
            // with the real format
            if(capturingGroup > 0)
            {
                QTextCharFormat& currentMaskedFormat= maskedFormat;
                // set the font size from the current rule's font format
                maskedFormat.setFontPointSize(format.fontPointSize());

                setFormat(match.capturedStart(maskedGroup), match.capturedLength(maskedGroup), currentMaskedFormat);
            }

            setFormat(match.capturedStart(capturingGroup), match.capturedLength(capturingGroup), format);
        }
    }
}

void MarkDownHighlighter::setHighlightingOptions(HighlightingOptions options)
{
    m_highlightingOptions= options;
}
