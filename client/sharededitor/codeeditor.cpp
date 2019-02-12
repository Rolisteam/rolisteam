#include <QtGui>

#include <QTextCursor>

#include "codeeditor.h"
#include "enu.h"
#include "utilities.h"

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    lineNumberArea= new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    isFirstTime= true;
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits= 1;
    int max= qMax(1, blockCount());
    while(max >= 10)
    {
        max/= 10;
        ++digits;
    }

    int space= 4 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::collabTextChange(int pos, int charsRemoved, int charsAdded, QString data)
{
    if(charsRemoved > 0 && charsAdded == 0)
    {
        QTextCursor cursor= textCursor();
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
        cursor.removeSelectedText();
    }
    else if(charsRemoved > 0 && charsAdded > 0)
    {
        QTextCursor cursor= textCursor();
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
        cursor.insertText(data);
    }
    else if(charsRemoved == 0 && charsAdded > 0)
    {
        QTextCursor cursor= textCursor();
        cursor.setPosition(pos);
        cursor.insertText(data);
    }
}

void CodeEditor::unCommentSelection()
{
    QTextCursor cursor= textCursor();
    QTextDocument* doc= cursor.document();
    cursor.beginEditBlock();

    int pos= cursor.position();
    int anchor= cursor.anchor();
    int start= qMin(anchor, pos);
    int end= qMax(anchor, pos);
    bool anchorIsStart= (anchor == start);

    QTextBlock startBlock= doc->findBlock(start);
    QTextBlock endBlock= doc->findBlock(end);

    if(end > start && endBlock.position() == end)
    {
        --end;
        endBlock= endBlock.previous();
    }

    bool doCStyleUncomment= false;
    bool doCStyleComment= false;
    bool doCppStyleUncomment= false;

    bool hasSelection= cursor.hasSelection();

    if(hasSelection)
    {
        QString startText= startBlock.text();
        int startPos= start - startBlock.position();
        bool hasLeadingCharacters= !startText.left(startPos).trimmed().isEmpty();
        if((startPos >= 2 && startText.at(startPos - 2) == QLatin1Char('/')
               && startText.at(startPos - 1) == QLatin1Char('*')))
        {
            startPos-= 2;
            start-= 2;
        }

        bool hasSelStart= (startPos < startText.length() - 2 && startText.at(startPos) == QLatin1Char('/')
                           && startText.at(startPos + 1) == QLatin1Char('*'));

        QString endText= endBlock.text();
        int endPos= end - endBlock.position();
        bool hasTrailingCharacters= !endText.left(endPos).remove(QStringLiteral("//")).trimmed().isEmpty()
                                    && !endText.mid(endPos).trimmed().isEmpty();
        if((endPos <= endText.length() - 2 && endText.at(endPos) == QLatin1Char('*')
               && endText.at(endPos + 1) == QLatin1Char('/')))
        {
            endPos+= 2;
            end+= 2;
        }

        bool hasSelEnd
            = (endPos >= 2 && endText.at(endPos - 2) == QLatin1Char('*') && endText.at(endPos - 1) == QLatin1Char('/'));

        doCStyleUncomment= hasSelStart && hasSelEnd;
        doCStyleComment= !doCStyleUncomment && (hasLeadingCharacters || hasTrailingCharacters);
    }

    if(doCStyleUncomment)
    {
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 2);
        cursor.removeSelectedText();
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
        cursor.removeSelectedText();
    }
    else if(doCStyleComment)
    {
        cursor.setPosition(end);
        cursor.insertText(QStringLiteral("*/"));
        cursor.setPosition(start);
        cursor.insertText(QStringLiteral("/*"));
    }
    else
    {
        endBlock= endBlock.next();
        doCppStyleUncomment= true;
        for(QTextBlock block= startBlock; block != endBlock; block= block.next())
        {
            QString text= block.text();
            if(!text.trimmed().startsWith(QStringLiteral("//")))
            {
                doCppStyleUncomment= false;
                break;
            }
        }
        for(QTextBlock block= startBlock; block != endBlock; block= block.next())
        {
            if(doCppStyleUncomment)
            {
                QString text= block.text();
                int i= 0;
                while(i < text.size() - 1)
                {
                    if(text.at(i) == QLatin1Char('/') && text.at(i + 1) == QLatin1Char('/'))
                    {
                        cursor.setPosition(block.position() + i);
                        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
                        cursor.removeSelectedText();
                        break;
                    }
                    if(!text.at(i).isSpace())
                        break;
                    ++i;
                }
            }
            else
            {
                cursor.setPosition(block.position());
                cursor.insertText(QLatin1String("//"));
            }
        }
    }

    // adjust selection when commenting out
    if(hasSelection && !doCStyleUncomment && !doCppStyleUncomment)
    {
        cursor= textCursor();
        if(!doCStyleComment)
            start= startBlock.position(); // move the double slashes into the selection
        int lastSelPos= anchorIsStart ? cursor.position() : cursor.anchor();
        if(anchorIsStart)
        {
            cursor.setPosition(start);
            cursor.setPosition(lastSelPos, QTextCursor::KeepAnchor);
        }
        else
        {
            cursor.setPosition(lastSelPos);
            cursor.setPosition(start, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }

    cursor.endEditBlock();
}

void CodeEditor::shiftLeft()
{
    QTextCursor cursor= textCursor();
    if(cursor.hasSelection())
    {
        int start= cursor.selectionStart();
        int end= cursor.selectionEnd();
        cursor.setPosition(start);
        int i= cursor.position();
        cursor.beginEditBlock();
        while(i < end)
        {
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            QString line= cursor.selectedText();
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            if(line.startsWith("    "))
            {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
                cursor.removeSelectedText();
                end-= 4;
            }
            else if(line.startsWith("\t"))
            {
                cursor.deleteChar();
            }
            cursor.movePosition(QTextCursor::EndOfLine);
            if(cursor.atEnd())
            {
                break;
            }
            else
            {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i= cursor.position();
            }
        }
        cursor.endEditBlock();
    }
    else
    {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString line= cursor.selectedText();
        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        if(line.startsWith("    "))
        {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
            cursor.removeSelectedText();
        }
        else if(line.startsWith("\t"))
        {
            cursor.deleteChar();
        }
    }
}

void CodeEditor::shiftRight()
{
    QTextCursor cursor= textCursor();
    int end= cursor.selectionEnd();
    int start= cursor.selectionStart();
    if(cursor.hasSelection())
    {
        //        setWordWrapMode(QTextOption::NoWrap);    //I need to figure out how to handle if WordWrap is enabled
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::StartOfLine);
        int i= cursor.position();
        cursor.beginEditBlock();
        while(i < end)
        {
            cursor.insertText("    ");
            end+= 4;
            cursor.movePosition(QTextCursor::EndOfLine);
            if(cursor.atEnd())
            {
                break;
            }
            else
            {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i= cursor.position();
            }
            //            if (wordWrapMode()) {
            //
            //            }
            //            Need to figure out what to do here
        }
        start+= 4;
        cursor.endEditBlock();
    }
    else
    {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText("    ");
    }
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

bool CodeEditor::findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    QString documentString= document()->toPlainText();
    QTextCursor cursor(document());
    int position;
    bool found= false;

    QRegExp rx;
    switch(mode)
    {
    case Enu::Contains:
        rx= QRegExp(searchString, sensitivity);
        break;
    case Enu::StartsWith:
        rx= QRegExp("\\b" + searchString, sensitivity);
        break;
    case Enu::EntireWord:
        rx= QRegExp("\\b" + searchString + "\\b", sensitivity);
    }

    position= textCursor().position();

    position= documentString.indexOf(rx, position);
    int length= searchString.size();

    if(position != -1)
    {
        cursor.setPosition(position, QTextCursor::MoveAnchor);
        for(int i= 0; i < length; i++)
        {
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
        found= true;
    }
    else if(wrapAround)
    {
        position= 0; // move cursor to the beginning and begin searching again
        position= documentString.indexOf(rx, position);
        length= searchString.size();

        if(position != -1)
        {
            cursor.setPosition(position, QTextCursor::MoveAnchor);
            for(int i= 0; i < length; i++)
            {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            }
            setTextCursor(cursor);
            found= true;
        }
    }

    return found;
}

bool CodeEditor::findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    QString documentString= document()->toPlainText();
    QTextCursor cursor(document());
    int position;
    bool found= false;

    QRegExp rx;
    switch(mode)
    {
    case Enu::Contains:
        rx= QRegExp(searchString, sensitivity);
        break;
    case Enu::StartsWith:
        rx= QRegExp("\\b" + searchString, sensitivity);
        break;
    case Enu::EntireWord:
        rx= QRegExp("\\b" + searchString + "\\b", sensitivity);
    }

    if(textCursor().hasSelection())
    {
        position= textCursor().selectionStart() - 1;
    }
    else
    {
        position= textCursor().position();
    }

    position= documentString.lastIndexOf(rx, position);
    int length= searchString.size();

    if(position != -1)
    {
        found= true;
        cursor.setPosition(position, QTextCursor::MoveAnchor);
        for(int i= 0; i < length; i++)
        {
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }
    else if(wrapAround)
    {
        // Move position to the end of the document.
        // magic # 2 is magic, anything less and it's beyond the scope of the document?
        position= documentString.lastIndexOf(rx, document()->characterCount() - 2);
        if(position != -1)
        {
            found= true;
            cursor.setPosition(position, QTextCursor::MoveAnchor);
            for(int i= 0; i < length; i++)
            {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            }
            setTextCursor(cursor);
        }
    }

    return found;
}

bool CodeEditor::replaceAll(
    QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    QString documentString= document()->toPlainText();
    bool isFound= false;
    bool isReplaced= false;

    QTextCursor cursor(document());
    int position= 0;
    cursor.setPosition(position);

    int length= searchString.size();

    QRegExp rx;
    switch(mode)
    {
    case Enu::Contains:
        rx= QRegExp(searchString, sensitivity);
        break;
    case Enu::StartsWith:
        rx= QRegExp("\\b" + searchString, sensitivity);
        break;
    case Enu::EntireWord:
        rx= QRegExp("\\b" + searchString + "\\b", sensitivity);
    }

    //    int count = documentString.count(rx);

    cursor.beginEditBlock();

    while(position != -1)
    {
        position= documentString.indexOf(rx, position + 1);

        if(position != -1)
        {
            cursor.setPosition(position, QTextCursor::MoveAnchor);
            for(int i= 0; i < length; i++)
            {
                cursor.deleteChar();
            }
            isFound= true;
            cursor.insertText(replaceString);
            isReplaced= true;
        }
    }

    cursor.endEditBlock();

    if(isFound && isReplaced)
    {
        return true;
    }
    return false;
}

bool CodeEditor::replace(QString replaceString)
{
    QTextCursor cursor(document());

    if(textCursor().hasSelection())
    {
        cursor.setPosition(textCursor().position());
        textCursor().removeSelectedText();
        cursor.insertText(replaceString);
        return true;
    }
    return false;
}

bool CodeEditor::findReplace(
    QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    bool isReplaced= replace(replaceString);
    bool isFound= findNext(searchString, sensitivity, wrapAround, mode);
    if(isReplaced && isFound)
    {
        return true;
    }
    return false;
}

bool CodeEditor::findAll(QString searchString)
{
    QString documentString= document()->toPlainText();
    bool isFound= false;
    if(!isFirstTime)
    {
        undo();
    }

    QTextCursor cursor(document());

    cursor.select(QTextCursor::Document);
    QTextCharFormat format;
    format.setBackground(Qt::white);
    cursor.mergeCharFormat(format);

    QTextCharFormat plainFormat(cursor.charFormat());
    QTextCharFormat colorFormat= plainFormat;
    colorFormat.setBackground(Qt::yellow);

    int position= 0;
    cursor.setPosition(position);

    int length= searchString.size();

    cursor.beginEditBlock();

    while(position != -1)
    {
        position= documentString.indexOf(searchString, position + 1, Qt::CaseInsensitive);

        if(position != -1)
        {
            cursor.setPosition(position, QTextCursor::MoveAnchor);
            for(int i= 0; i < length; i++)
            {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                cursor.mergeCharFormat(colorFormat);
            }
            setTextCursor(cursor);
            isFound= true;
        }
    }

    cursor.endEditBlock();
    isFirstTime= false;

    return isFound;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if(dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr= contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor= QColor(233, 243, 255); // light blue

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor= textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(232, 232, 232)); // light grey

    QTextBlock block= firstVisibleBlock();
    int blockNumber= block.blockNumber();
    int top= static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom= top + static_cast<int>(blockBoundingRect(block).height());

    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom >= event->rect().top())
        {
            QString number= QString::number(blockNumber + 1);
            painter.setPen(QColor(128, 128, 130)); // grey
            painter.drawText(-1, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block= block.next();
        top= bottom;
        bottom= top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
