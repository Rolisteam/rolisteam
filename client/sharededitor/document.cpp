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
    m_editor = new CodeEditor(this);
    QFontMetrics fm(m_editor->font());
    m_editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, m_editor);
    
    m_participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, m_participantPane);


    // Find all toolbar widget
    /*delete ui->findAllFrame;
    findAllToolbar = new FindToolBar(this);
    ui->editorVerticalLayout->insertWidget(1, findAllToolbar);
    findAllToolbar->hide();

    connect(findAllToolbar, SIGNAL(findAll(QString)), m_editor, SLOT(findAll(QString)));
    connect(findAllToolbar, SIGNAL(findNext(QString)), this, SLOT(findNext(QString)));
    connect(findAllToolbar, SIGNAL(findPrevious(QString)), this, SLOT(findPrevious(QString)));*/

    // Emit signals to the mainwindow when redoability/undoability changes
    connect(m_editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(m_editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    /*QList<int> sizeList;
    sizeList << 9000 << 1;
    ui->codeChatSplitter->setSizes(sizeList);
    ui->participantSplitter->setSizes(sizeList);*/

    // Hide the panels that only matter if we're using the collaborative portion of the app
    //setChatHidden(true);
    setParticipantsHidden(true);

    myName = "Owner"; // temporary

    startedCollaborating = false;
}

Document::~Document()
{    
    delete ui;
}
/*
 *
 */
void Document::runUpdateCmd(QString cmd)
{
    QRegExp rx;
    if (cmd.startsWith("doc:"))
    {
        cmd.remove(0, 4);
        rx = QRegExp("(\\d+)\\s(\\d+)\\s(\\d+)\\s(.*)");
        if (cmd.contains(rx))
        {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            cmd = rx.cap(4);
            m_editor->blockSignals(true);
            m_editor->collabTextChange(pos, charsRemoved, charsAdded, cmd);
            m_editor->blockSignals(true);
        }
    }
}
void Document::displayParticipantPanel()
{
    startedCollaborating = true;
    setChatHidden(true);
    setParticipantsHidden(false);
}

void Document::connectToDocument(QStringList list)
{
    myName = list.at(0);    
    QString address = list.at(1);
    QString portString = list.at(2);

    int port = portString.toInt();
    startedCollaborating = true;

    setChatHidden(false);
    setParticipantsHidden(false);
    m_editor->setReadOnly(true);
    m_participantPane->setDisabled(true);
}

void Document::setEditorFont(QFont font)
{
    m_editor->setFont(font);
    QFontMetrics fm(m_editor->font());
    m_editor->setTabStopWidth(fm.averageCharWidth() * 4);
}

void Document::setChatFont(QFont font)
{
   // chatPane->setFont(font);
}

void Document::setParticipantsFont(QFont font)
{
    m_participantPane->setFont(font);
}

void Document::undo()
{
    if (m_editor->hasFocus())
    {
        m_editor->undo();
    }
 /*   else if (chatPane->hasFocus()) {
        chatPane->undo();
    }*/
}

void Document::redo()
{
    if (m_editor->hasFocus())
    {
        m_editor->redo();
    }
   /* else if (chatPane->hasFocus()) {
        chatPane->redo();
    }*/
}

void Document::cut()
{
    if (m_editor->hasFocus())
    {
        m_editor->cut();
    }
    else if (m_participantPane->hasFocus())
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
    if (m_editor->hasFocus())
    {
        m_editor->copy();
    }
    else if (m_participantPane->hasFocus())
    {
        // do nothing
    }
   /* else if (chatPane->hasFocus())
    * {
        chatPane->copy();
    }*/
}

void Document::paste()
{
    if (m_editor->hasFocus())
    {
        m_editor->paste();
    }
    else if (m_participantPane->hasFocus())
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
    if (b)
    {
        ui->participantSplitter->widget(1)->hide();
    }
    else
    {
        ui->participantSplitter->widget(1)->show();
        m_editor->resize(QSize(9000, 9000));
    }
}

void Document::setChatHidden(bool b)
{
    // Hide/show the widget (contains the chat widget) below the code text edit
   /* if (b)
    {
        ui->codeChatSplitter->widget(1)->hide();
    }
    else
    {
        m_editor->setFocus();
        ui->codeChatSplitter->widget(1)->show();
    }*/
}

void Document::shiftLeft()
{
    m_editor->shiftLeft();
}

void Document::shiftRight()
{
    m_editor->shiftRight();
}

void Document::unCommentSelection()
{
    m_editor->unCommentSelection();
}


void Document::fill(NetworkMessageWriter* msg)
{
    msg->string32(m_editor->toPlainText());
    qDebug() << "text"<< m_editor->toPlainText();
    m_participantPane->fill(msg);
}
void Document::readFromMsg(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        if(msg->action() == NetMsg::updateTextAndPermission)
        {
            QString text = msg->string32();
            qDebug() << "text"<< text;
            m_editor->setPlainText(text);
            m_participantPane->readFromMsg(msg);
        }
        else if(msg->action() == NetMsg::updatePermissionOneUser)
        {
            m_participantPane->readPermissionChanged(msg);
        }
    }
}
void Document::setHighlighter(int Highlighter)
{
    switch (Highlighter)
    {
    case None:
        /*delete highlighter;
        highlighter = NULL;*/
        break;

    default:
        break;
    }
}

bool Document::isUndoable()
{
    return m_editor->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return m_editor->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return m_editor->document()->isModified();
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
    if (!m_editor->findNext(searchString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}

void Document::findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!m_editor->findPrev(searchString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}

void Document::replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    if (!m_editor->replaceAll(searchString, replaceString, sensitivity, mode))
    {
        QMessageBox::information(m_editor, tr("Cahoots"), tr("The string was not found."));
    }
}

void Document::replace(QString replaceString)
{
    if (!m_editor->replace(replaceString))
    {
        emit notFound();
    }
}

void Document::findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!m_editor->findReplace(searchString, replaceString, sensitivity, wrapAround, mode))
    {
        emit notFound();
    }
}
QTextDocument* Document::getDocument()
{
    if(nullptr != m_editor)
        return m_editor->document();
    else
        return nullptr;
}

QString Document::getPlainText()
{
    return m_editor->toPlainText();
}

void Document::setPlainText(QString text)
{
    m_editor->setPlainText(text);
}

void Document::toggleLineWrap()
{
    if (m_editor->lineWrapMode() == QPlainTextEdit::NoWrap) {
        m_editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        //bottomEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        //bottomEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void Document::setModified(bool b)
{
    m_editor->document()->setModified(b);
}

void Document::previewAsHtml()
{
    QString text = m_editor->toPlainText();
    QWebEngineView *preview = new QWebEngineView();
    preview->setHtml(text);
    preview->show();
}
bool Document::docHasCollaborated()
{
    return startedCollaborating;
}

void Document::findNext(QString string)
{
    m_editor->findNext(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::findPrevious(QString string)
{
    m_editor->findPrev(string, Qt::CaseInsensitive, true, Enu::Contains);
}

ParticipantsPane *Document::getParticipantPane() const
{
    return m_participantPane;
}

void Document::setParticipantPane(ParticipantsPane *participantPane)
{
    m_participantPane = participantPane;
}

void Document::setOwner(Player* player)
{
    m_participantPane->setOwner(player);
}
bool Document::canWrite(Player* player)
{
    return m_participantPane->canWrite(player);
}
