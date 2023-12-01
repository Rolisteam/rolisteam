/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SHAREDNOTES_CODEEDITOR_H
#define SHAREDNOTES_CODEEDITOR_H

#include "enu.h"
#include "rwidgets_global.h"
#include <QObject>
#include <QPlainTextEdit>
#include <QSettings>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
class SharedNoteController;
namespace sharedNotes
{
class RWIDGET_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(SharedNoteController* ctrl, QWidget* parent= nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

    void collabTextChange(int pos, int charsRemoved, int charsAdded, QString data);

    void unCommentSelection();
    void shiftLeft();
    void shiftRight();

    // Find functions
    bool findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    bool findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    bool replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode);
    bool replace(QString replaceString);
    bool findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround,
                     Enu::FindMode mode);

public slots:
    bool findAll(QString searchString);

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect&, int);

private:
    SharedNoteController* m_sharedCtrl;
    bool isFirstTime;

    QWidget* lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(sharedNotes::CodeEditor* editor) : QWidget(editor) { codeEditor= editor; }

    QSize sizeHint() const { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent* event) { codeEditor->lineNumberAreaPaintEvent(event); }

private:
    sharedNotes::CodeEditor* codeEditor;
};
}
#endif // SHAREDNOTES_CODEEDITOR_H
