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
#include "document.h"
#include "ui_document.h"

#include "utilities.h"
#include "enu.h"

#include <QDialog>
#include <QRegExp>
#include <QDebug>
#include <QFontMetrics>
#include <QWebEngineView>
#include <QLayout>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QDebug>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    // Set up the editor
    delete ui->editorFrame;
    editor = new CodeEditor(this);
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, editor);
    
    // editor for split pane editing
    delete ui->bottomEditorFrame;
    bottomEditor = new CodeEditor(this);
    bottomEditor->setFont(editor->font());
    bottomEditor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(1, bottomEditor);

    // we don't need the default document since we're using the document of the original editor
    bottomEditor->document()->deleteLater();
    bottomEditor->setDocument(editor->document());
    bottomEditor->hide();

    // editor highlighter
    // highlighter = new CppHighlighter(editor->document());

    highlighter = NULL;

    // Participant frame
    delete ui->participantFrame;
    participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, participantPane);

    // Chat frame
   /* delete ui->chatFrame;
    chatPane = new ChatPane();
    ui->codeChatSplitter->insertWidget(1, chatPane);*/

    // Find all toolbar widget
    delete ui->findAllFrame;
    findAllToolbar = new FindToolBar(this);
    ui->editorVerticalLayout->insertWidget(1, findAllToolbar);
    findAllToolbar->hide();

    connect(findAllToolbar, SIGNAL(findAll(QString)), editor, SLOT(findAll(QString)));
    connect(findAllToolbar, SIGNAL(findNext(QString)), this, SLOT(findNext(QString)));
    connect(findAllToolbar, SIGNAL(findPrevious(QString)), this, SLOT(findPrevious(QString)));

    // Emit signals to the mainwindow when redoability/undoability changes
    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    QList<int> sizeList;
    sizeList << 9000 << 1;
    ui->codeChatSplitter->setSizes(sizeList);
    ui->participantSplitter->setSizes(sizeList);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    myName = "Owner"; // temporary

    startedCollaborating = false;
}

Document::~Document()
{    
    delete ui;
}

void Document::displayParticipantPanel()
{
    startedCollaborating = true;
    setChatHidden(true);
    setParticipantsHidden(false);
    //participantPane->setConnectInfo(ipAddress, port);
}

void Document::connectToDocument(QStringList list)
{
    myName = list.at(0);    
    QString address = list.at(1);
    QString portString = list.at(2);

    int port = portString.toInt();
    participantPane->setOwnership(false);
    startedCollaborating = true;

    setChatHidden(false);
    setParticipantsHidden(false);
    editor->setReadOnly(true);
    participantPane->setDisabled(true);

  /*  client = new Client(editor, participantPane, chatPane, this);
    client->setUsername(myName);
    client->connectToHost(QHostAddress(address), port);
    participantPane->setConnectInfo(address, portString);*/
}

void Document::setEditorFont(QFont font)
{
    editor->setFont(font);
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
}

void Document::setChatFont(QFont font)
{
   // chatPane->setFont(font);
}

void Document::setParticipantsFont(QFont font)
{
    participantPane->setFont(font);
}

void Document::undo()
{
    if (editor->hasFocus()) {
        editor->undo();
    }
 /*   else if (chatPane->hasFocus()) {
        chatPane->undo();
    }*/
}

void Document::redo()
{
    if (editor->hasFocus()) {
        editor->redo();
    }
   /* else if (chatPane->hasFocus()) {
        chatPane->redo();
    }*/
}

void Document::cut()
{
    if (editor->hasFocus())
    {
        editor->cut();
    }
    else if (participantPane->hasFocus())
    {
        // do nothing
    }
   /* else if (chatPane->hasFocus())
    {
        chatPane->cut();
    }*/
}

void Document::copy()
{
    if (editor->hasFocus()) {
        editor->copy();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
   /* else if (chatPane->hasFocus()) {
        chatPane->copy();
    }*/
}

void Document::paste()
{
    if (editor->hasFocus())
    {
        editor->paste();
    }
    else if (participantPane->hasFocus())
    {
        // do nothing
    }
    /*else if (chatPane->hasFocus())
    {
        chatPane->paste();
    }*/
}

void Document::setParticipantsHidden(bool b)
{
    if (b) {
        ui->participantSplitter->widget(1)->hide();
    }
    else {
        ui->participantSplitter->widget(1)->show();
        editor->resize(QSize(9000, 9000));
    }
}

void Document::setChatHidden(bool b)
{
    // Hide/show the widget (contains the chat widget) below the code text edit
    if (b) {
        ui->codeChatSplitter->widget(1)->hide();
    }
    else {
        editor->setFocus();
        ui->codeChatSplitter->widget(1)->show();
    }
}

void Document::shiftLeft()
{
    editor->shiftLeft();
}

void Document::shiftRight()
{
    editor->shiftRight();
}

void Document::unCommentSelection()
{
    editor->unCommentSelection();
}

void Document::resynchronizeTriggered()
{
    /*if (server) {
        server->resynchronize();
    }
    else if (client) {
        client->resynchronize();
    }*/
}

void Document::setHighlighter(int Highlighter)
{
    switch (Highlighter) {
    case None:
        delete highlighter;
        highlighter = NULL;
        break;
   /* case CPlusPlus:
        if (highlighter) {
            delete highlighter;
            highlighter = NULL;
        }
        highlighter = new CppHighlighter(editor->document());
        break;
    case Python:
        if (highlighter) {
            delete highlighter;
            highlighter = NULL;
        }
        highlighter = new PythonHighlighter(editor->document());
        break;*/
    default:
        break;
    }
}

bool Document::isUndoable()
{
    return editor->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return editor->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return editor->document()->isModified();
}

bool Document::isChatHidden()
{
    return ui->codeChatSplitter->widget(1)->isHidden();
}

bool Document::isParticipantsHidden()
{
    return ui->participantSplitter->widget(1)->isHidden();
}

void Document::findAll()
{
    findAllToolbar->show();
    findAllToolbar->giveFocus();
}

void Document::findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findNext(searchString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

void Document::findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findPrev(searchString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

void Document::replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    if (!editor->replaceAll(searchString, replaceString, sensitivity, mode)) {
        QMessageBox::information(editor, tr("Cahoots"), tr("The string was not found."));
    }
}

void Document::replace(QString replaceString)
{
    if (!editor->replace(replaceString)) {
        emit notFound();
    }
}

void Document::findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findReplace(searchString, replaceString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

QString Document::getPlainText()
{
    return editor->toPlainText();
}

void Document::setPlainText(QString text)
{
    editor->setPlainText(text);
}

void Document::toggleLineWrap()
{
    if (editor->lineWrapMode() == QPlainTextEdit::NoWrap) {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        bottomEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        bottomEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void Document::setModified(bool b)
{
    editor->document()->setModified(b);
}

void Document::previewAsHtml()
{
    QString text = editor->toPlainText();
    QWebEngineView *preview = new QWebEngineView();
    preview->setHtml(text);
    preview->show();
}

void Document::splitEditor()
{
    ui->editorSplitter->setOrientation(Qt::Vertical);
    bottomEditor->show();
    QList<int> sizes;
    sizes << 500 << 500;
    ui->editorSplitter->setSizes(sizes);
}

void Document::splitEditorSideBySide()
{
    ui->editorSplitter->setOrientation(Qt::Horizontal);
    bottomEditor->show();
    QList<int> sizes;
    sizes << 500 << 500;
    ui->editorSplitter->setSizes(sizes);
}

bool Document::docHasCollaborated()
{
    return startedCollaborating;
}

void Document::unSplitEditor()
{
    if (!isEditorSplit())
    {
        return;
    }
    bottomEditor->hide();
}

bool Document::isEditorSplit()
{
    return !bottomEditor->isHidden();
}

bool Document::isEditorSplitSideBySide()
{
    return ui->editorSplitter->orientation() == Qt::Horizontal;
}

void Document::findNext(QString string)
{
    editor->findNext(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::findPrevious(QString string)
{
    editor->findPrev(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::setOwnerName(QString name)
{
   // server->setOwnerName(name);
}
